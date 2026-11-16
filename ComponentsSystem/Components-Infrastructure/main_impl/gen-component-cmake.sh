#!/bin/bash

# Generates CMakeLists.txt for a component from cs_sys/manifest.ini

set -euo pipefail

LOCAL_scriptBasedir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
LOCAL_dataDir="$LOCAL_scriptBasedir/../data"
LOCAL_MANIFEST=cs_sys/manifest.ini
LOCAL_PARSER="$LOCAL_scriptBasedir/gen-component-cmake_impl/parse_manifest.awk"

# MAIN_PROJECT_ROOTDIR wins when it points to a component, otherwise process the current directory
if [[ -n ${MAIN_PROJECT_ROOTDIR:-} && -f "$MAIN_PROJECT_ROOTDIR/$LOCAL_MANIFEST" ]]; then
    cd -- "$MAIN_PROJECT_ROOTDIR" || {
        LOG_ERROR "Cannot enter MAIN_PROJECT_ROOTDIR: $MAIN_PROJECT_ROOTDIR"
        exit 1
    }
fi

function SPLIT_LIST() {
    local LOCAL_VALUE=$1
    local LOCAL_ITEM
    local -a LOCAL_ITEMS

    IFS=',' read -r -a LOCAL_ITEMS <<< "$LOCAL_VALUE"
    for LOCAL_ITEM in "${LOCAL_ITEMS[@]}"; do
        LOCAL_ITEM=${LOCAL_ITEM#"${LOCAL_ITEM%%[![:space:]]*}"}
        LOCAL_ITEM=${LOCAL_ITEM%"${LOCAL_ITEM##*[![:space:]]}"}
        if [[ -n $LOCAL_ITEM ]]; then
            printf '%s\n' "$LOCAL_ITEM"
        fi
    done
}

function SWITCH_VARIABLE() {
    case "$1" in
        tests_enabled) printf 'COMPONENTS_ENABLE_TESTS\n' ;;
        qt_enabled) printf 'COMPONENTS_IS_ENABLED_QT\n' ;;
        *) printf 'COMPONENTS_SW_%s\n' "$1" ;;
    esac
}

LOG_SEPARATOR
LOG_INFO "Generating CMakeLists.txt for component in $PWD"

if (! test -d src); then
    LOG_ERROR "Missing source directory: $PWD/src"
    exit 1
fi

if (! test -f "$LOCAL_MANIFEST"); then
    LOG_ERROR "Missing component manifest: $PWD/$LOCAL_MANIFEST"
    exit 1
fi

if (! test -f "$LOCAL_PARSER"); then
    LOG_ERROR "Missing manifest parser: $LOCAL_PARSER"
    exit 1
fi

declare -A LOCAL_SYSTEM=()
declare -a LOCAL_SWITCHES=()
declare -A LOCAL_SW_DEFAULT=()
declare -A LOCAL_SW_DEPENDS=()
declare -A LOCAL_SW_COMPONENTS=()
declare -A LOCAL_SW_SOURCES=()
declare -A LOCAL_SW_PRECOMPILED=()
declare -A LOCAL_SW_INCLUDES=()

while IFS=$'\t' read -r LOCAL_SECTION LOCAL_KEY LOCAL_VALUE; do
    if [[ $LOCAL_SECTION == system ]]; then
        LOCAL_SYSTEM[$LOCAL_KEY]=$LOCAL_VALUE
    elif [[ $LOCAL_SECTION == switches.* ]]; then
        LOCAL_SWITCH=${LOCAL_SECTION#switches.}
        if [[ -z ${LOCAL_SW_DEFAULT[$LOCAL_SWITCH]+set} ]]; then
            LOCAL_SWITCHES+=("$LOCAL_SWITCH")
            LOCAL_SW_DEFAULT[$LOCAL_SWITCH]=none
        fi
        case $LOCAL_KEY in
            default) LOCAL_SW_DEFAULT[$LOCAL_SWITCH]=$LOCAL_VALUE ;;
            depends) LOCAL_SW_DEPENDS[$LOCAL_SWITCH]=$LOCAL_VALUE ;;
            depends-components) LOCAL_SW_COMPONENTS[$LOCAL_SWITCH]=$LOCAL_VALUE ;;
            sources) LOCAL_SW_SOURCES[$LOCAL_SWITCH]=$LOCAL_VALUE ;;
            precompiled) LOCAL_SW_PRECOMPILED[$LOCAL_SWITCH]=$LOCAL_VALUE ;;
            includes) LOCAL_SW_INCLUDES[$LOCAL_SWITCH]=$LOCAL_VALUE ;;
        esac
    fi
done < <(awk -f "$LOCAL_PARSER" "$LOCAL_MANIFEST")

LOCAL_NAME=${LOCAL_SYSTEM[name]:-}
LOCAL_VERSION=${LOCAL_SYSTEM[version]:-}
LOCAL_VERSION=${LOCAL_VERSION#\"}
LOCAL_VERSION=${LOCAL_VERSION%\"}

if [[ -z $LOCAL_NAME ]]; then
    LOG_ERROR "Missing 'name' in [system]"
    exit 1
fi

if [[ -z $LOCAL_VERSION ]]; then
    LOG_ERROR "Missing 'version' in [system]"
    exit 1
fi

LOCAL_COMPONENT=${LOCAL_NAME#Components-}
LOCAL_TARGET=Components-$LOCAL_COMPONENT

for LOCAL_SWITCH in "${LOCAL_SWITCHES[@]}"; do
    LOCAL_DEFAULT=${LOCAL_SW_DEFAULT[$LOCAL_SWITCH]:-none}
    case ${LOCAL_DEFAULT,,} in
        none|'') LOCAL_SW_DEFAULT[$LOCAL_SWITCH]=none ;;
        on) LOCAL_SW_DEFAULT[$LOCAL_SWITCH]=ON ;;
        off) LOCAL_SW_DEFAULT[$LOCAL_SWITCH]=OFF ;;
        *)
            LOG_ERROR "Invalid default '$LOCAL_DEFAULT' for switch '$LOCAL_SWITCH'"
            exit 1
            ;;
    esac
done

declare -A LOCAL_DEP_MAP=()
LOCAL_DEP_MAP_READY=0
LOCAL_DEP_MAP_FILE=
LOCAL_RESOLVED_DEP=

function LOAD_DEP_MAP() {
    local LOCAL_OS_NAME
    local LOCAL_LINE
    local LOCAL_ORIGINAL_LINE
    local LOCAL_KEY
    local LOCAL_VALUE

    LOCAL_OS_NAME=$(awk -F= '/^PRETTY_NAME=/{sub(/^PRETTY_NAME=/, ""); gsub(/^"|"$/, ""); print; exit}' /etc/os-release)
    if [[ -z $LOCAL_OS_NAME ]]; then
        LOG_WARNING "Cannot determine OS name from /etc/os-release, using dependency names as-is"
        LOCAL_DEP_MAP_READY=1
        return
    fi

    LOCAL_DEP_MAP_FILE="$LOCAL_dataDir/depends/$LOCAL_OS_NAME.txt"
    if (! test -f "$LOCAL_DEP_MAP_FILE"); then
        LOG_WARNING "Dependency map not found: $LOCAL_DEP_MAP_FILE, using dependency names as-is"
        LOCAL_DEP_MAP_READY=1
        return
    fi

    LOG_INFO "Loading dependency map: $LOCAL_DEP_MAP_FILE"

    while IFS= read -r LOCAL_LINE || [[ -n $LOCAL_LINE ]]; do
        LOCAL_LINE=${LOCAL_LINE%$'\r'}
        if [[ $LOCAL_LINE =~ ^[[:space:]]*(#|;) ]] || [[ $LOCAL_LINE =~ ^[[:space:]]*$ ]]; then
            continue
        fi
        if [[ $LOCAL_LINE != *:* ]]; then
            LOG_WARNING "Invalid dependency map line: $LOCAL_LINE"
            continue
        fi

        LOCAL_ORIGINAL_LINE=$LOCAL_LINE
        LOCAL_LINE=$(printf '%s\n' "$LOCAL_LINE" | sed 's/\([^:]\):\([^:]\)/\1\t\2/')
        if [[ $LOCAL_LINE != *$'\t'* ]]; then
            LOG_WARNING "Invalid dependency map line: $LOCAL_ORIGINAL_LINE"
            continue
        fi

        LOCAL_KEY=${LOCAL_LINE%%$'\t'*}
        LOCAL_VALUE=${LOCAL_LINE#*$'\t'}
        LOCAL_KEY=${LOCAL_KEY#"${LOCAL_KEY%%[![:space:]]*}"}
        LOCAL_KEY=${LOCAL_KEY%"${LOCAL_KEY##*[![:space:]]}"}
        LOCAL_VALUE=${LOCAL_VALUE#"${LOCAL_VALUE%%[![:space:]]*}"}
        LOCAL_VALUE=${LOCAL_VALUE%"${LOCAL_VALUE##*[![:space:]]}"}
        if [[ -z $LOCAL_KEY || -z $LOCAL_VALUE ]]; then
            LOG_WARNING "Invalid dependency map line: $LOCAL_LINE"
            continue
        fi

        LOCAL_DEP_MAP[$LOCAL_KEY]=$LOCAL_VALUE
    done < "$LOCAL_DEP_MAP_FILE"

    LOCAL_DEP_MAP_READY=1
}

function RESOLVE_DEP() {
    local LOCAL_DEP=$1

    if [[ $LOCAL_DEP_MAP_READY -eq 0 ]]; then
        LOAD_DEP_MAP
    fi

    if [[ -n ${LOCAL_DEP_MAP[$LOCAL_DEP]+set} ]]; then
        LOCAL_RESOLVED_DEP=${LOCAL_DEP_MAP[$LOCAL_DEP]}
        return
    fi

    if [[ $LOCAL_DEP != *::* ]]; then
        if [[ -n $LOCAL_DEP_MAP_FILE ]]; then
            LOG_WARNING "No mapping for dependency '$LOCAL_DEP' in $LOCAL_DEP_MAP_FILE, using name as-is"
        else
            LOG_WARNING "No mapping for dependency '$LOCAL_DEP', using name as-is"
        fi
    fi

    LOCAL_RESOLVED_DEP=$LOCAL_DEP
}

mapfile -t LOCAL_SYSTEM_DEPS < <(SPLIT_LIST "${LOCAL_SYSTEM[depends]:-}")
mapfile -t LOCAL_SYSTEM_COMPONENTS < <(SPLIT_LIST "${LOCAL_SYSTEM[depends-components]:-}")
mapfile -t LOCAL_SYSTEM_PRECOMPILED < <(SPLIT_LIST "${LOCAL_SYSTEM[precompiled]:-}")

LOCAL_MAPPED_SYSTEM_DEPS=()
LOCAL_CONAN_DEPS=()
for LOCAL_DEP in "${LOCAL_SYSTEM_DEPS[@]}"; do
    RESOLVE_DEP "$LOCAL_DEP"
    LOCAL_MAPPED_SYSTEM_DEPS+=("$LOCAL_RESOLVED_DEP")
    if [[ $LOCAL_DEP != *::* ]]; then
        LOCAL_CONAN_DEPS+=("$LOCAL_DEP")
    fi
done

declare -A LOCAL_SW_MAPPED_DEPS=()
for LOCAL_SWITCH in "${LOCAL_SWITCHES[@]}"; do
    if [[ $LOCAL_SWITCH == tests_enabled ]]; then
        continue
    fi
    mapfile -t LOCAL_SWITCH_DEPS < <(SPLIT_LIST "${LOCAL_SW_DEPENDS[$LOCAL_SWITCH]:-}")
    LOCAL_SW_MAPPED_DEPS[$LOCAL_SWITCH]=""
    for LOCAL_DEP in "${LOCAL_SWITCH_DEPS[@]}"; do
        RESOLVE_DEP "$LOCAL_DEP"
        LOCAL_SW_MAPPED_DEPS[$LOCAL_SWITCH]+=" $LOCAL_RESOLVED_DEP"
    done
done

LOCAL_TMP="CMakeLists.txt.tmp.$$"
trap 'rm -f "$LOCAL_TMP"' EXIT

{
    if ((${#LOCAL_MAPPED_SYSTEM_DEPS[@]})); then
        printf 'COMPONENTS_CONFIGURE_COMPONENT(%s "%s"\n' "$LOCAL_COMPONENT" "$LOCAL_VERSION"
        for LOCAL_DEP in "${LOCAL_MAPPED_SYSTEM_DEPS[@]}"; do
            printf '    %s\n' "$LOCAL_DEP"
        done
        printf ')\n'
    else
        printf 'COMPONENTS_CONFIGURE_COMPONENT(%s "%s")\n' "$LOCAL_COMPONENT" "$LOCAL_VERSION"
    fi

    if ((${#LOCAL_SYSTEM_PRECOMPILED[@]})); then
        printf 'target_precompile_headers(%s PUBLIC\n' "$LOCAL_TARGET"
        for LOCAL_HEADER in "${LOCAL_SYSTEM_PRECOMPILED[@]}"; do
            printf '    "%s"\n' "$LOCAL_HEADER"
        done
        printf ')\n'
    fi

    for LOCAL_SWITCH in "${LOCAL_SWITCHES[@]}"; do
        if [[ $LOCAL_SWITCH == tests_enabled || $LOCAL_SWITCH == qt_enabled ]]; then
            continue
        fi
        if [[ ${LOCAL_SW_DEFAULT[$LOCAL_SWITCH]} == none ]]; then
            continue
        fi
        printf 'option(COMPONENTS_SW_%s "" %s)\n' "$LOCAL_SWITCH" "${LOCAL_SW_DEFAULT[$LOCAL_SWITCH]}"
    done

    if ((${#LOCAL_CONAN_DEPS[@]})); then
        printf 'COMPONENTS_CCR_CONAN_IS_CONFIGURED(COMPONENTS_CONAN_CONFIGURED)\n'
        printf 'if (COMPONENTS_CONAN_CONFIGURED)\n'
        for LOCAL_DEP in "${LOCAL_CONAN_DEPS[@]}"; do
            printf '    COMPONENTS_CCR_CONAN_REQUIRE(%s)\n' "$LOCAL_DEP"
        done
        printf 'endif()\n'
    fi

    for LOCAL_DEPENDENCY in "${LOCAL_SYSTEM_COMPONENTS[@]}"; do
        printf 'COMPONENTS_LINK_COMPONENT(%s %s)\n' "$LOCAL_TARGET" "$LOCAL_DEPENDENCY"
    done

    for LOCAL_SWITCH in "${LOCAL_SWITCHES[@]}"; do
        mapfile -t LOCAL_SWITCH_COMPONENTS < <(SPLIT_LIST "${LOCAL_SW_COMPONENTS[$LOCAL_SWITCH]:-}")
        mapfile -t LOCAL_SWITCH_SOURCES < <(SPLIT_LIST "${LOCAL_SW_SOURCES[$LOCAL_SWITCH]:-}")
        mapfile -t LOCAL_SWITCH_PRECOMPILED < <(SPLIT_LIST "${LOCAL_SW_PRECOMPILED[$LOCAL_SWITCH]:-}")
        mapfile -t LOCAL_SWITCH_INCLUDES < <(SPLIT_LIST "${LOCAL_SW_INCLUDES[$LOCAL_SWITCH]:-}")

        LOCAL_REMOVE_SOURCES=()
        for LOCAL_SOURCE in "${LOCAL_SWITCH_SOURCES[@]}"; do
            if [[ -d $LOCAL_SOURCE ]]; then
                while IFS= read -r -d '' LOCAL_SOURCE_FILE; do
                    LOCAL_SOURCE_FILE=${LOCAL_SOURCE_FILE#./}
                    LOCAL_REMOVE_SOURCES+=("$LOCAL_SOURCE_FILE")
                done < <(find "$LOCAL_SOURCE" -type f -print0 | sort -z)
            else
                LOCAL_REMOVE_SOURCES+=("$LOCAL_SOURCE")
            fi
        done

        LOCAL_MAPPED_SWITCH_DEPS=()
        if [[ $LOCAL_SWITCH == tests_enabled ]]; then
            LOCAL_SWITCH_COMPONENTS=()
            LOCAL_SWITCH_PRECOMPILED=()
        elif [[ -n ${LOCAL_SW_MAPPED_DEPS[$LOCAL_SWITCH]:-} ]]; then
            read -r -a LOCAL_MAPPED_SWITCH_DEPS <<< "${LOCAL_SW_MAPPED_DEPS[$LOCAL_SWITCH]}"
        fi

        LOCAL_HAS_ENABLED=0
        LOCAL_HAS_DISABLED=0
        if ((${#LOCAL_MAPPED_SWITCH_DEPS[@]} || ${#LOCAL_SWITCH_COMPONENTS[@]} || ${#LOCAL_SWITCH_PRECOMPILED[@]} || ${#LOCAL_SWITCH_INCLUDES[@]})); then
            LOCAL_HAS_ENABLED=1
        fi
        if ((${#LOCAL_REMOVE_SOURCES[@]})); then
            LOCAL_HAS_DISABLED=1
        fi
        if ((LOCAL_HAS_ENABLED == 0 && LOCAL_HAS_DISABLED == 0)); then
            continue
        fi

        LOCAL_VARIABLE=$(SWITCH_VARIABLE "$LOCAL_SWITCH")

        if ((LOCAL_HAS_ENABLED == 0 && LOCAL_HAS_DISABLED == 1)); then
            printf 'if (NOT %s)\n' "$LOCAL_VARIABLE"
        else
            printf 'if (%s)\n' "$LOCAL_VARIABLE"
        fi

        if ((LOCAL_HAS_ENABLED == 1)); then
            printf '    target_compile_definitions(%s PRIVATE %s)\n' "$LOCAL_TARGET" "$LOCAL_VARIABLE"
            if ((${#LOCAL_MAPPED_SWITCH_DEPS[@]})); then
                printf '    target_link_libraries(%s PRIVATE' "$LOCAL_TARGET"
                for LOCAL_DEP in "${LOCAL_MAPPED_SWITCH_DEPS[@]}"; do
                    printf ' %s' "$LOCAL_DEP"
                done
                printf ')\n'
            fi
            for LOCAL_DEPENDENCY in "${LOCAL_SWITCH_COMPONENTS[@]}"; do
                printf '    COMPONENTS_LINK_COMPONENT(%s %s)\n' "$LOCAL_TARGET" "$LOCAL_DEPENDENCY"
            done
            for LOCAL_INCLUDE in "${LOCAL_SWITCH_INCLUDES[@]}"; do
                LOCAL_INCLUDE=${LOCAL_INCLUDE#\"}
                LOCAL_INCLUDE=${LOCAL_INCLUDE%\"}
                if [[ -n $LOCAL_INCLUDE ]]; then
                    printf '    target_include_directories(%s PUBLIC "%s")\n' "$LOCAL_TARGET" "$LOCAL_INCLUDE"
                fi
            done
            if ((${#LOCAL_SWITCH_PRECOMPILED[@]})); then
                printf '    target_precompile_headers(%s PUBLIC\n' "$LOCAL_TARGET"
                for LOCAL_HEADER in "${LOCAL_SWITCH_PRECOMPILED[@]}"; do
                    printf '        "%s"\n' "$LOCAL_HEADER"
                done
                printf '    )\n'
            fi
        fi

        if ((LOCAL_HAS_DISABLED == 1)); then
            if ((LOCAL_HAS_ENABLED == 1)); then
                printf 'else()\n'
            fi
            printf '    COMPONENTS_REMOVE_SOURCES(%s\n' "$LOCAL_TARGET"
            for LOCAL_SOURCE in "${LOCAL_REMOVE_SOURCES[@]}"; do
                printf '        "${CMAKE_CURRENT_SOURCE_DIR}/%s"\n' "$LOCAL_SOURCE"
            done
            printf '    )\n'
        fi

        printf 'endif()\n'
    done

    if [[ -d tests ]]; then
        printf 'COMPONENTS_ADD_COMPONENT_TEST(%s)\n' "$LOCAL_COMPONENT"
    fi
} > "$LOCAL_TMP"

mv "$LOCAL_TMP" CMakeLists.txt
trap - EXIT

LOG_OK "Generated CMakeLists.txt for $LOCAL_TARGET"
LOG_SEPARATOR
