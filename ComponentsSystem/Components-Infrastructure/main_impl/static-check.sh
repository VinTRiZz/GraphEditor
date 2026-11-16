#!/bin/bash

# Runs cppcheck over project sources

LOG_SEPARATOR
LOG_INFO "Starting static check..."

if ! command -v cppcheck >/dev/null; then
    LOG_ERROR "cppcheck is not installed"
    exit 1
fi

LOCAL_includeArgs=()
while IFS= read -r LOCAL_dir; do
    [[ -z $LOCAL_dir ]] && continue
    while IFS= read -r LOCAL_match; do
        LOCAL_includeArgs+=(-I "$LOCAL_match")
    done < <(compgen -G "$MAIN_PROJECT_ROOTDIR/$LOCAL_dir")
done < "$LOC_INCLUDES"

LOCAL_sourceArgs=()
while IFS= read -r LOCAL_dir; do
    [[ -z $LOCAL_dir ]] && continue
    LOCAL_sourceArgs+=("$MAIN_PROJECT_ROOTDIR/$LOCAL_dir")
done < "$LOC_SOURCES"

LOG_DEBUG "Checking files: ${LOCAL_sourceArgs[*]}"
LOG_DEBUG "Includes:       ${LOCAL_includeArgs[*]}"

cppcheck --enable=all --std=c++20 --platform=unix64 --suppress=missingInclude \
    "${LOCAL_includeArgs[@]}" "${LOCAL_sourceArgs[@]}" \
    &> "$LOC_LOGS_DIR/static_check_$LOG_START_TIMESTAMP.log"

if [[ $? -ne 0 ]]; then
    LOG_ERROR "Static check failed, see $LOC_LOGS_DIR/static_check_$LOG_START_TIMESTAMP.log"
    exit 1
fi

LOG_OK "Static check completed"
LOG_SEPARATOR
