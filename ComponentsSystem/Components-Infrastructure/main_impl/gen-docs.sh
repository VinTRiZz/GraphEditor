#!/bin/bash

# Generates Doxygen documentation into the project root

LOCAL_baseDoxyfile="$LOC_MAIN_BASEDIR/data/Doxyfile"
LOCAL_mergedConfig=""

LOG_SEPARATOR
LOG_INFO "Starting documentation generation..."

if ! command -v doxygen >/dev/null; then
    LOG_ERROR "doxygen is not installed"
    exit 1
fi

if [[ ! -f $LOCAL_baseDoxyfile ]]; then
    LOG_ERROR "Doxyfile not found: $LOCAL_baseDoxyfile"
    exit 1
fi

# LOC_DOXYCONF holds optional overrides appended to the bundled Doxyfile
if [[ -n $(tr -d '[:space:]' < "$LOC_DOXYCONF" 2>/dev/null) ]]; then
    LOCAL_mergedConfig="$LOC_LOGS_DIR/Doxyfile_$LOG_START_TIMESTAMP"
    cat "$LOCAL_baseDoxyfile" "$LOC_DOXYCONF" > "$LOCAL_mergedConfig" ||
        { LOG_ERROR "Failed to prepare Doxyfile"; exit 1; }
    trap 'rm -f -- "$LOCAL_mergedConfig"' EXIT
fi

LOCAL_doxyfile="${LOCAL_mergedConfig:-$LOCAL_baseDoxyfile}"
LOCAL_log="$LOC_LOGS_DIR/gen_docs_$LOG_START_TIMESTAMP.log"

if ( cd "$MAIN_PROJECT_ROOTDIR" && doxygen "$LOCAL_doxyfile" ) &> "$LOCAL_log"; then
    LOG_OK "Documentation generated in \"$MAIN_PROJECT_ROOTDIR/Project Documentation\""
else
    LOG_ERROR "Documentation generation failed, see $LOCAL_log"
    exit 1
fi

LOG_SEPARATOR
