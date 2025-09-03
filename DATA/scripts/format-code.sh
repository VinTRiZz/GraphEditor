#! /bin/bash

PRPTC_SCRIPTDIR=$(realpath "$(dirname $0)")

clang-format-19 "--style=file:$PRPTC_SCRIPTDIR/../etc/.clang-format" -i $(find "$PRPTC_SCRIPTDIR/../../Libraries" "$PRPTC_SCRIPTDIR/../../App" -iname '*.cpp' -o -iname '*.hpp' -o -iname '*.h')
