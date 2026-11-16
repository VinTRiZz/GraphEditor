#!/bin/bash

# Installs the Git configuration bundle and a main.sh launcher into a project.

if ! declare -F LOG_INFO >/dev/null 2>&1; then
    LOG_INFO() { printf '[ INFO ] %s\n' "$*"; }
fi
if ! declare -F LOG_OK >/dev/null 2>&1; then
    LOG_OK() { printf '[  OK  ] %s\n' "$*"; }
fi
if ! declare -F LOG_WARNING >/dev/null 2>&1; then
    LOG_WARNING() { printf '[ WARN ] %s\n' "$*" >&2; }
fi
if ! declare -F LOG_ERROR >/dev/null 2>&1; then
    LOG_ERROR() { printf '[ FAIL ] %s\n' "$*" >&2; }
fi
if ! declare -F LOG_SEPARATOR >/dev/null 2>&1; then
    LOG_SEPARATOR() { printf '=============================================\n'; }
fi

LOCAL_scriptBasedir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
LOCAL_infraRoot=${LOC_MAIN_BASEDIR:-$(cd -- "$LOCAL_scriptBasedir/.." && pwd)}
LOCAL_dataDir="$LOCAL_infraRoot/data"
LOCAL_gitconfigDir="$LOCAL_dataDir/gitconfig"
LOCAL_starterTemplate="$LOCAL_dataDir/setup-repo/main.sh.template"
LOCAL_infraMain="$LOCAL_infraRoot/main.sh"
LOCAL_projectRoot=${MAIN_PROJECT_ROOTDIR:-$PWD}
LOCAL_force=0

function MERGE_GITIGNORE() {
    LOCAL_ignoreTemplate=$1
    LOCAL_ignoreTarget=$2
    LOCAL_added=0
    LOCAL_line=""

    if [[ ! -f $LOCAL_ignoreTarget ]]; then
        cp -a -- "$LOCAL_ignoreTemplate" "$LOCAL_ignoreTarget" || return 1
        LOG_OK "Created .gitignore"
        return 0
    fi

    if [[ -s $LOCAL_ignoreTarget && -n $(tail -c 1 -- "$LOCAL_ignoreTarget") ]]; then
        printf '\n' >> "$LOCAL_ignoreTarget"
    fi

    while IFS= read -r LOCAL_line || [[ -n $LOCAL_line ]]; do
        [[ -z $LOCAL_line || $LOCAL_line == '#'* ]] && continue
        grep -Fqx -- "$LOCAL_line" "$LOCAL_ignoreTarget" && continue
        if (( LOCAL_added == 0 )); then
            printf '\n# Added by Components-Infrastructure setup-repo\n' >> "$LOCAL_ignoreTarget"
        fi
        printf '%s\n' "$LOCAL_line" >> "$LOCAL_ignoreTarget"
        LOCAL_added=$(( LOCAL_added + 1 ))
    done < "$LOCAL_ignoreTemplate"

    if (( LOCAL_added > 0 )); then
        LOG_OK "Added $LOCAL_added .gitignore entries"
    else
        LOG_INFO ".gitignore already contains all entries"
    fi
    return 0
}

for LOCAL_arg in "$@"; do
    case "$LOCAL_arg" in
        --force)
            LOCAL_force=1
            ;;
        -h|--help)
            LOG_INFO "Usage: setup-repo.sh [--force]"
            LOG_INFO "Installs .githooks, core.hooksPath, .gitignore entries and main.sh."
            LOG_INFO "Project root: ${MAIN_PROJECT_ROOTDIR:-$PWD} (override with MAIN_PROJECT_ROOTDIR)"
            exit 0
            ;;
        *)
            LOG_ERROR "Unknown argument: $LOCAL_arg"
            exit 1
            ;;
    esac
done

LOG_SEPARATOR
LOG_INFO "Starting repository setup ..."

LOCAL_projectRoot=$(realpath -- "$LOCAL_projectRoot" 2>/dev/null)
if [[ -z $LOCAL_projectRoot || ! -d $LOCAL_projectRoot ]]; then
    LOG_ERROR "Invalid project root: ${MAIN_PROJECT_ROOTDIR:-$PWD}"
    exit 1
fi

for LOCAL_required in "$LOCAL_gitconfigDir" "$LOCAL_starterTemplate" "$LOCAL_infraMain"; do
    if [[ ! -e $LOCAL_required ]]; then
        LOG_ERROR "Required path not found: $LOCAL_required"
        exit 1
    fi
done

LOCAL_infraRoot=$(realpath -- "$LOCAL_infraRoot" 2>/dev/null || true)
if [[ $LOCAL_projectRoot == "$LOCAL_infraRoot" ]]; then
    LOG_ERROR "Project root is the Components-Infrastructure repository itself"
    LOG_ERROR "Run setup-repo from the project that should use the infrastructure."
    exit 1
fi

LOCAL_gitRoot=$(git -C "$LOCAL_projectRoot" rev-parse --show-toplevel 2>/dev/null || true)
if [[ -z $LOCAL_gitRoot ]]; then
    LOG_INFO "Initializing Git repository in $LOCAL_projectRoot"
    if ! git -C "$LOCAL_projectRoot" init -q; then
        LOG_ERROR "Failed to initialize Git repository"
        exit 1
    fi
    LOCAL_gitRoot=$(git -C "$LOCAL_projectRoot" rev-parse --show-toplevel 2>/dev/null || true)
fi
LOCAL_gitRoot=$(realpath -- "$LOCAL_gitRoot" 2>/dev/null || true)
if [[ $LOCAL_gitRoot != "$LOCAL_projectRoot" ]]; then
    LOG_ERROR "Project root is not the Git repository root"
    LOG_ERROR "  project root: ${LOCAL_projectRoot:-<empty>}"
    LOG_ERROR "  repository:   ${LOCAL_gitRoot:-<none>}"
    exit 1
fi

LOCAL_hooksDir="$LOCAL_projectRoot/.githooks"
if ! mkdir -p -- "$LOCAL_hooksDir"; then
    LOG_ERROR "Failed to create hooks directory: $LOCAL_hooksDir"
    exit 1
fi

for LOCAL_entry in "$LOCAL_gitconfigDir"/* "$LOCAL_gitconfigDir"/.[!.]*; do
    [[ -e $LOCAL_entry ]] || continue
    LOCAL_name=$(basename -- "$LOCAL_entry")
    [[ $LOCAL_name == .gitignore ]] && continue
    if [[ -d $LOCAL_entry ]]; then
        mkdir -p -- "$LOCAL_hooksDir/$LOCAL_name" || {
            LOG_ERROR "Failed to create directory: $LOCAL_hooksDir/$LOCAL_name"
            exit 1
        }
        cp -a -- "$LOCAL_entry/." "$LOCAL_hooksDir/$LOCAL_name/" || {
            LOG_ERROR "Failed to copy: $LOCAL_entry"
            exit 1
        }
    else
        cp -a -- "$LOCAL_entry" "$LOCAL_hooksDir/$LOCAL_name" || {
            LOG_ERROR "Failed to copy: $LOCAL_entry"
            exit 1
        }
    fi
done

for LOCAL_hook in commit-msg check-debug-words.sh check-secrets.sh update-cmake-version.sh; do
    [[ -f $LOCAL_hooksDir/$LOCAL_hook ]] && chmod +x -- "$LOCAL_hooksDir/$LOCAL_hook"
done
LOG_OK "Hooks installed in $LOCAL_hooksDir"

LOCAL_oldHooksPath=$(git -C "$LOCAL_projectRoot" config --local --get core.hooksPath 2>/dev/null || true)
if [[ -n $LOCAL_oldHooksPath && $LOCAL_oldHooksPath != ".githooks" ]]; then
    LOG_WARNING "Overriding core.hooksPath: $LOCAL_oldHooksPath -> .githooks"
fi
if ! git -C "$LOCAL_projectRoot" config --local core.hooksPath .githooks; then
    LOG_ERROR "Failed to set core.hooksPath"
    exit 1
fi
LOG_OK "core.hooksPath set to .githooks"

if ! MERGE_GITIGNORE "$LOCAL_gitconfigDir/.gitignore" "$LOCAL_projectRoot/.gitignore"; then
    LOG_ERROR "Failed to update .gitignore"
    exit 1
fi

LOCAL_infraRel=$(realpath --relative-to="$LOCAL_projectRoot" -- "$LOCAL_infraMain")
LOCAL_starterContent=$(<"$LOCAL_starterTemplate")
LOCAL_starterContent=${LOCAL_starterContent//@INFRA_MAIN_REL@/$LOCAL_infraRel}
LOCAL_starterTarget="$LOCAL_projectRoot/main.sh"
LOCAL_writeStarter=1

if [[ -f $LOCAL_starterTarget ]]; then
    LOCAL_existingContent=$(<"$LOCAL_starterTarget")
    if [[ $LOCAL_existingContent == "$LOCAL_starterContent" ]]; then
        LOG_INFO "main.sh launcher is up to date"
        LOCAL_writeStarter=0
    elif (( LOCAL_force == 0 )); then
        LOG_WARNING "main.sh exists and differs; use --force to overwrite"
        LOCAL_writeStarter=0
    fi
fi

if (( LOCAL_writeStarter )); then
    if printf '%s\n' "$LOCAL_starterContent" > "$LOCAL_starterTarget"; then
        chmod +x -- "$LOCAL_starterTarget"
        LOG_OK "Created main.sh launcher"
    else
        LOG_ERROR "Failed to write $LOCAL_starterTarget"
        exit 1
    fi
fi

LOG_OK "Repository setup completed"
LOG_SEPARATOR
