# Parses a component manifest into tab-separated records:
# <section><TAB><key><TAB><value>

{
    LOCAL_LINE = $0
    sub(/\r$/, "", LOCAL_LINE)
    sub(/^[ \t]+/, "", LOCAL_LINE)
    sub(/[ \t]+$/, "", LOCAL_LINE)

    if (LOCAL_LINE == "" || LOCAL_LINE ~ /^[;#]/) {
        next
    }

    if (LOCAL_LINE ~ /^\[.*\]$/) {
        LOCAL_SECTION = LOCAL_LINE
        sub(/^\[/, "", LOCAL_SECTION)
        sub(/\][ \t]*$/, "", LOCAL_SECTION)
        gsub(/^[ \t]+|[ \t]+$/, "", LOCAL_SECTION)
        next
    }

    LOCAL_POSITION = index(LOCAL_LINE, "=")
    if (LOCAL_POSITION == 0) {
        next
    }

    LOCAL_KEY = substr(LOCAL_LINE, 1, LOCAL_POSITION - 1)
    LOCAL_VALUE = substr(LOCAL_LINE, LOCAL_POSITION + 1)
    gsub(/^[ \t]+|[ \t]+$/, "", LOCAL_KEY)
    gsub(/^[ \t]+|[ \t]+$/, "", LOCAL_VALUE)

    if (LOCAL_SECTION == "" || LOCAL_KEY == "") {
        next
    }

    print LOCAL_SECTION "\t" LOCAL_KEY "\t" LOCAL_VALUE
}
