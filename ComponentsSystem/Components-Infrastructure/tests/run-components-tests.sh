#!/bin/bash

# Runs generation tests for component CMakeLists

set -euo pipefail

LOG_ERROR() {
    echo -e "[ \033[31mFAIL\033[0m ] " "$@"
}
export -f LOG_ERROR

LOG_OK() {
    echo -e "[  \033[32mOK\033[0m  ] " "$@"
}
export -f LOG_OK

LOG_INFO() {
    echo -e "[ \033[36mINFO\033[0m ] " "$@"
}
export -f LOG_INFO

LOG_WARNING() {
    echo -e "[ \033[33mWARN\033[0m ] " "$@"
}
export -f LOG_WARNING

LOG_SEPARATOR() {
    echo -e "============================================="
}
export -f LOG_SEPARATOR

LOG_DEBUG() {
    echo -e "[ \033[35mDEBG\033[0m ] " "$@"
}
export -f LOG_DEBUG

LOCAL_testsDir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
LOCAL_sourceRoot=$(cd -- "$LOCAL_testsDir/.." && pwd)
LOCAL_sourceMainImpl="$LOCAL_sourceRoot/main_impl"
LOCAL_workDir=$(mktemp -d)
trap 'rm -rf "$LOCAL_workDir"' EXIT

LOCAL_envRoot="$LOCAL_workDir/root"
LOCAL_envMainImpl="$LOCAL_envRoot/main_impl"
LOCAL_envData="$LOCAL_envRoot/data"
mkdir -p "$LOCAL_envMainImpl/gen-component-cmake_impl" "$LOCAL_envData/depends"
cp -a "$LOCAL_sourceMainImpl/gen-component-cmake.sh" "$LOCAL_envMainImpl/"
cp -a "$LOCAL_sourceMainImpl/gen-component-cmake_impl/parse_manifest.awk" \
    "$LOCAL_envMainImpl/gen-component-cmake_impl/"

LOCAL_OS_NAME=$(awk -F= '/^PRETTY_NAME=/{sub(/^PRETTY_NAME=/, ""); gsub(/^"|"$/, ""); print; exit}' /etc/os-release)
if [[ -z $LOCAL_OS_NAME ]]; then
    LOG_ERROR "Cannot determine OS name from /etc/os-release"
    exit 1
fi

LOCAL_DEP_MAP_FILE="$LOCAL_envData/depends/$LOCAL_OS_NAME.txt"
mkdir -p "$(dirname -- "$LOCAL_DEP_MAP_FILE")"
cat > "$LOCAL_DEP_MAP_FILE" <<'EOF'
Boost.System:boost_system
Boost.Thread:boost_thread
C++FS:stdc++fs
OpenSSL::SSL:ssl
OpenSSL::Crypto:crypto
Sodium:sodium
SQLite3:sqlite3
EOF

LOCAL_generator="$LOCAL_envMainImpl/gen-component-cmake.sh"
LOCAL_status=0

LOG_SEPARATOR
LOG_INFO "Running component CMake generation tests..."

for LOCAL_caseDir in "$LOCAL_testsDir"/*/; do
    [[ -f "$LOCAL_caseDir/cs_sys/manifest.ini" ]] || continue

    LOCAL_caseName=$(basename "$LOCAL_caseDir")
    LOCAL_runDir="$LOCAL_workDir/$LOCAL_caseName"

    LOG_INFO "Checking: $LOCAL_caseName"

    mkdir -p "$LOCAL_runDir/src"
    cp -a "$LOCAL_caseDir/." "$LOCAL_runDir/"

    if (! (cd "$LOCAL_runDir" && unset MAIN_PROJECT_ROOTDIR && bash "$LOCAL_generator")); then
        LOG_ERROR "Generator failed: $LOCAL_caseName"
        LOCAL_status=1
        continue
    fi

    if diff -u "$LOCAL_caseDir/expected_CMakeLists.txt" "$LOCAL_runDir/CMakeLists.txt"; then
        LOG_OK "Passed: $LOCAL_caseName"
    else
        LOG_ERROR "Output differs from expected: $LOCAL_caseName"
        LOCAL_status=1
    fi
done

if [[ $LOCAL_status == 0 ]]; then
    LOG_OK "All component tests passed"
else
    LOG_ERROR "Component tests failed"
fi
LOG_SEPARATOR

exit $LOCAL_status
