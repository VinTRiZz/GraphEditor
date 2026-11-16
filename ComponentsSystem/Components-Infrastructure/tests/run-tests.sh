#!/bin/bash

# Runs every test suite in this directory

set -u -o pipefail

f_LOG_info() { printf '[ INFO ] %s\n' "$*"; }
f_LOG_error() { printf '[ FAIL ] %s\n' "$*" >&2; }

LOCAL_testsDir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
LOCAL_status=0

for LOCAL_test in \
    "$LOCAL_testsDir/run-components-tests.sh" \
    "$LOCAL_testsDir/run-gitconfig-tests.sh" \
    "$LOCAL_testsDir/check-commit-format_test.sh" \
    "$LOCAL_testsDir/check-secrets_test.sh" \
    "$LOCAL_testsDir/update-cmake-version_test.sh"; do
    f_LOG_info "Running: $(basename -- "$LOCAL_test")"
    if ! bash "$LOCAL_test"; then
        f_LOG_error "Failed: $(basename -- "$LOCAL_test")"
        LOCAL_status=1
    fi
done

exit $LOCAL_status
