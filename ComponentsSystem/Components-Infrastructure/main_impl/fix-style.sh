#!/bin/bash

# Formats project C++ sources with clang-format

LOCAL_stylesClangfile="$LOC_MAIN_BASEDIR/data/.clang-format"

LOG_SEPARATOR
LOG_INFO "Starting style fix..."

if [[ ! -f $LOCAL_stylesClangfile ]]; then
    LOG_ERROR "clang-format config not found: $LOCAL_stylesClangfile"
    exit 1
fi

LOCAL_clangFormat=$(command -v clang-format-19 || command -v clang-format)
if [[ -z $LOCAL_clangFormat ]]; then
    LOG_ERROR "clang-format is not installed"
    exit 1
fi

while IFS= read -r LOCAL_dir; do
    [[ -z $LOCAL_dir ]] && continue

    LOCAL_checkdir="$MAIN_PROJECT_ROOTDIR/$LOCAL_dir"
    if [[ ! -e $LOCAL_checkdir ]]; then
        LOG_WARNING "Directory \"$LOCAL_dir\" not found, skipped"
        continue
    fi

    LOCAL_files=()
    mapfile -d '' -t LOCAL_files < <(
        find "$LOCAL_checkdir" -type f \
            \( -iname '*.cpp' -o -iname '*.hpp' -o -iname '*.h' \) -print0
    )

    LOG_INFO "Fixing styles in: $LOCAL_dir (${#LOCAL_files[@]} files)"
    if (( ${#LOCAL_files[@]} )); then
        "$LOCAL_clangFormat" "--style=file:$LOCAL_stylesClangfile" -i "${LOCAL_files[@]}" ||
            { LOG_ERROR "Failed to format \"$LOCAL_dir\""; exit 1; }
    fi
    LOG_OK "Complete"
done < "$LOC_SOURCES"

LOG_OK "Style fix completed"
LOG_SEPARATOR
