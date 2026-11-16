#!/bin/bash

# Tests for main_impl helper scripts

set -u -o pipefail

f_LOG_info() { printf '[ INFO ] %s\n' "$*"; }
f_LOG_error() { printf '[ FAIL ] %s\n' "$*" >&2; }
f_LOG_ok() { printf '[  OK  ] %s\n' "$*"; }
f_LOG_warn() { printf '[ WARN ] %s\n' "$*"; }

LOCAL_testsDir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
LOCAL_infraDir=$(cd -- "$LOCAL_testsDir/.." && pwd)
LOCAL_mainImpl="$LOCAL_infraDir/main_impl"
LOCAL_workDir=$(mktemp -d)
trap 'rm -rf -- "$LOCAL_workDir"' EXIT

LOCAL_project="$LOCAL_workDir/project"
LOCAL_logs="$LOCAL_workDir/logs"
LOCAL_config="$LOCAL_workDir/config"
mkdir -p "$LOCAL_project/src" "$LOCAL_logs" "$LOCAL_config"

cat > "$LOCAL_project/CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.16)
project(MainImplTest CXX)
add_executable(main_impl_test src/main.cpp)
EOF
printf 'int main() { return 0; }\n' > "$LOCAL_project/src/main.cpp"
printf 'src\n' > "$LOCAL_config/sources.txt"
printf 'src\n' > "$LOCAL_config/includes.txt"
: > "$LOCAL_config/doxyconf.txt"
: > "$LOCAL_config/depends.txt"

# Environment for subscripts, mirroring the variables exported by main.sh
export MAIN_PROJECT_ROOTDIR="$LOCAL_project"
export LOC_LOGS_DIR="$LOCAL_logs"
export LOG_START_TIMESTAMP=test
export LOC_MAIN_BASEDIR="$LOCAL_infraDir"
export LOC_SOURCES="$LOCAL_config/sources.txt"
export LOC_INCLUDES="$LOCAL_config/includes.txt"
export LOC_DOXYCONF="$LOCAL_config/doxyconf.txt"
export LOC_DEPSFILE="$LOCAL_config/depends.txt"

LOG_ERROR() { printf '%s\n' "$*" >&2; }
LOG_OK() { :; }
LOG_INFO() { :; }
LOG_WARNING() { :; }
LOG_DEBUG() { :; }
LOG_SEPARATOR() { :; }
export -f LOG_ERROR LOG_OK LOG_INFO LOG_WARNING LOG_DEBUG LOG_SEPARATOR

LOCAL_output=""
LOCAL_rc=0
LOCAL_status=0

f_run() {
    local LOCAL_script="$1"
    shift
    LOCAL_output=$(bash "$LOCAL_mainImpl/$LOCAL_script" "$@" 2>&1)
    LOCAL_rc=$?
}

f_check_status() {
    local LOCAL_expected="$1"
    local LOCAL_actual="$2"
    local LOCAL_name="$3"
    if [[ $LOCAL_expected -eq $LOCAL_actual ]]; then
        f_LOG_ok "$LOCAL_name"
    else
        f_LOG_error "$LOCAL_name: expected status $LOCAL_expected, got $LOCAL_actual"
        f_LOG_error "  output: $LOCAL_output"
        LOCAL_status=1
    fi
}

f_check_output() {
    local LOCAL_needle="$1"
    local LOCAL_name="$2"
    if [[ $LOCAL_output == *"$LOCAL_needle"* ]]; then
        f_LOG_ok "$LOCAL_name"
    else
        f_LOG_error "$LOCAL_name: output does not contain: $LOCAL_needle"
        LOCAL_status=1
    fi
}

f_check_exists() {
    local LOCAL_name="$1"
    local LOCAL_path="$2"
    if [[ -e $LOCAL_path ]]; then
        f_LOG_ok "$LOCAL_name"
    else
        f_LOG_error "$LOCAL_name: missing $LOCAL_path"
        LOCAL_status=1
    fi
}

f_LOG_info "running main_impl tests"

if ! command -v cmake >/dev/null || ! command -v ninja >/dev/null; then
    f_LOG_warn "skipping build tests: cmake or ninja not found"
else
    f_run build.sh
    f_check_status 0 "$LOCAL_rc" "debug build succeeds"
    f_check_exists "debug build produces a binary" "$LOCAL_project/build/main_impl_test"
    f_check_exists "debug build writes a configure log" \
        "$LOCAL_logs/build_configure_test.log"

    f_run build.sh release
    f_check_status 0 "$LOCAL_rc" "release build succeeds"
    if grep -q '^CMAKE_BUILD_TYPE:STRING=Release$' "$LOCAL_project/build/CMakeCache.txt" 2>/dev/null; then
        f_LOG_ok "release build configures Release mode"
    else
        f_LOG_error "release build did not configure Release mode"
        LOCAL_status=1
    fi

    f_run build-release.sh
    f_check_status 0 "$LOCAL_rc" "build-release wrapper succeeds"
    if grep -q '^CMAKE_BUILD_TYPE:STRING=Release$' "$LOCAL_project/build/CMakeCache.txt" 2>/dev/null; then
        f_LOG_ok "build-release wrapper configures Release mode"
    else
        f_LOG_error "build-release wrapper did not configure Release mode"
        LOCAL_status=1
    fi

    export MAIN_PROJECT_ROOTDIR="$LOCAL_workDir/missing"
    f_run build.sh
    f_check_status 1 "$LOCAL_rc" "build rejects a missing project root"
    f_check_output "Invalid project root directory" \
        "build reports an invalid project root"
    export MAIN_PROJECT_ROOTDIR="$LOCAL_project"
fi

if ! command -v cppcheck >/dev/null; then
    f_LOG_warn "skipping static-check test: cppcheck not found"
else
    f_run static-check.sh
    f_check_status 0 "$LOCAL_rc" "static check succeeds on clean sources"
    f_check_exists "static check writes a log" \
        "$LOCAL_logs/static_check_test.log"
fi

if ! command -v clang-format-19 >/dev/null && ! command -v clang-format >/dev/null; then
    f_LOG_warn "skipping fix-style test: clang-format not found"
else
    f_run fix-style.sh
    f_check_status 0 "$LOCAL_rc" "style fix succeeds on project sources"
fi

if ! command -v doxygen >/dev/null; then
    f_LOG_warn "skipping gen-docs test: doxygen not found"
else
    f_run gen-docs.sh
    f_check_status 0 "$LOCAL_rc" "documentation generation succeeds"
    f_check_exists "documentation generation produces output" \
        "$LOCAL_project/Project Documentation"
fi

if [[ $(id -u) -eq 0 ]]; then
    f_LOG_warn "skipping install-deps guard test: running as root"
else
    f_run install-deps.sh
    f_check_status 1 "$LOCAL_rc" "install-deps refuses to run as a regular user"
    f_check_output "Invalid permissions" \
        "install-deps explains the permission requirement"
fi

if (( LOCAL_status == 0 )); then
    f_LOG_ok "all main_impl tests passed"
else
    f_LOG_error "main_impl tests failed"
fi
exit "$LOCAL_status"
