#!/bin/bash

# =============================================================== #
# Logging helpers (exported, so subscripts can reuse them)
# =============================================================== #

function LOG_ERROR() {
    echo -e "[ \033[31mFAIL\033[0m ] " "$@"
}
export -f LOG_ERROR

function LOG_OK() {
    echo -e "[  \033[32mOK\033[0m  ] " "$@"
}
export -f LOG_OK

function LOG_INFO() {
    echo -e "[ \033[36mINFO\033[0m ] " "$@"
}
export -f LOG_INFO

function LOG_WARNING() {
    echo -e "[ \033[33mWARN\033[0m ] " "$@"
}
export -f LOG_WARNING

function LOG_SEPARATOR() {
    echo -e "============================================="
}
export -f LOG_SEPARATOR

function LOG_DEBUG() {
    echo -e "[ \033[35mDEBG\033[0m ] " "$@"
}
export -f LOG_DEBUG

# =============================================================== #
# Common paths and environment for subscripts
# =============================================================== #

LOCAL_scriptBasedir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
export LOCAL_scriptBasedir

export LOC_MAIN_BASEDIR="$LOCAL_scriptBasedir"
export LOC_SCRIPTS_DIR="$LOCAL_scriptBasedir/main_impl"
export LOC_LOGS_DIR="$LOCAL_scriptBasedir/logs"
export LOG_START_TIMESTAMP=$(date +%d.%m.%Y_%H-%M-%S)

export LOC_DEPSFILE="$LOCAL_scriptBasedir/config/depends.txt"
export LOC_SOURCES="$LOCAL_scriptBasedir/config/sourcedirs.txt"
export LOC_INCLUDES="$LOCAL_scriptBasedir/config/includedirs.txt"
export LOC_DOXYCONF="$LOCAL_scriptBasedir/config/doxyconf.txt"

if [[ ! -e $LOC_DEPSFILE || ! -e $LOC_SOURCES || ! -e $LOC_INCLUDES ||
      ! -e $LOC_DOXYCONF || ! -d $LOC_SCRIPTS_DIR ]]; then
    LOG_ERROR "Script components and constants check failed. Check if repo is correct"
    exit 1
fi

mkdir -p "$LOC_LOGS_DIR"

# Project root defaults to the caller's working directory
: "${MAIN_PROJECT_ROOTDIR:=$PWD}"
MAIN_PROJECT_ROOTDIR=$(realpath -- "$MAIN_PROJECT_ROOTDIR" 2>/dev/null)
while [[ ! -d $MAIN_PROJECT_ROOTDIR ]]; do
    LOG_ERROR "Invalid project root directory: ${MAIN_PROJECT_ROOTDIR:-<empty>}"
    read -e -r -p "Write project directory: >>> " MAIN_PROJECT_ROOTDIR || exit 1
    MAIN_PROJECT_ROOTDIR=$(realpath -- "$MAIN_PROJECT_ROOTDIR" 2>/dev/null)
done
export MAIN_PROJECT_ROOTDIR

# =============================================================== #
# Available functionality
# =============================================================== #

declare -A LOCAL_functionality=(
    ["fix-style"]="Format project C++ sources"
    ["gen-docs"]="Generate Doxygen documentation"
    ["install-deps"]="Install project dependencies from $(basename "$LOC_DEPSFILE")"
    ["install-docker"]="Install Docker engine and Compose plugin"
    ["static-check"]="Run cppcheck over project sources"
    ["build"]="Build project into [ ROOT/build ] with CMake"
    ["build-release"]="Build project and run the installation target"
    ["gen-component-cmake"]="Regenerate CMakeLists.txt from cs_sys/manifest.ini"
    ["setup-repo"]="Install Git hooks, .gitignore entries and main.sh launcher"
)

# Register any additional subscripts not described above
for LOCAL_script in "$LOC_SCRIPTS_DIR"/*.sh; do
    [[ -f $LOCAL_script ]] || continue
    LOCAL_scriptname=$(basename -- "$LOCAL_script" .sh)
    [[ -v LOCAL_functionality[$LOCAL_scriptname] ]] ||
        LOCAL_functionality[$LOCAL_scriptname]="Run $LOCAL_scriptname.sh"
done

if [[ $# -eq 0 || $1 == "--help" || $1 == "-h" ]]; then
    echo "============ Welcome to repository master =============="
    echo "Most cases, script is all you need to handle a project"
    echo "Project root: $MAIN_PROJECT_ROOTDIR (override with MAIN_PROJECT_ROOTDIR)"
    echo "Functionality (flags do not combine, first one wins):"

    for LOCAL_scriptname in $(printf '%s\n' "${!LOCAL_functionality[@]}" | sort); do
        printf "%-22s | %s\n" "$LOCAL_scriptname" "${LOCAL_functionality[$LOCAL_scriptname]}"
    done
    exit 0
fi

# =============================================================== #
# Dispatch
# =============================================================== #

LOG_DEBUG "Args: $*"
LOG_DEBUG "Project root: $MAIN_PROJECT_ROOTDIR"
LOG_DEBUG "Logs directory: $LOC_LOGS_DIR"

LOCAL_subscript="$LOC_SCRIPTS_DIR/$1.sh"

if [[ -v LOCAL_functionality[$1] ]]; then
    if [[ ! -f $LOCAL_subscript ]]; then
        LOG_ERROR "Subscript not found: $LOCAL_subscript"
        exit 1
    fi

    bash "$LOCAL_subscript" "${@:2}"
    exit $?
fi

LOG_ERROR "Invalid argument: $1"
exit 1
