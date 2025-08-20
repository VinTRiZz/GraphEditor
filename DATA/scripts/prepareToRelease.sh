#! /bin/bash

PRPTC_SCRIPTDIR=$(realpath "$(dirname $0)")

cd "$PRPTC_SCRIPTDIR/../.."
#rm -r build BIN &> /dev/null
# mkdir build && cd build
if [ "$?" != "0" ]; then
    echo "Did not created builds dir"
    exit 1
fi

#cmake -GNinja .. && cmake --build . --target all
if [ "$?" != "0" ]; then
    echo "Error building the project"
    exit 2
fi

doxygen "$PRPTC_SCRIPTDIR/../etc/Doxyfile"

cd "$PRPTC_SCRIPTDIR/../etc"
clang-format-19 -i $(find "$PRPTC_SCRIPTDIR/../.." -iname '*.cpp' -o -iname '*.hpp')
