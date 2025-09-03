#! /bin/bash

PRPTC_SCRIPTDIR=$(realpath "$(dirname $0)")

cd "$PRPTC_SCRIPTDIR/../.."
rm -r build BIN &> /dev/null
mkdir build && cd build
if [ "$?" != "0" ]; then
    echo "Did not created builds dir"
    exit 1
fi

cmake -GNinja .. && cmake --build . --target all
if [ "$?" != "0" ]; then
    echo "Error building the project"
    exit 2
fi

cd "$PRPTC_SCRIPTDIR/../../BIN"
for iTest in $PRPTC_SCRIPTDIR/../../BIN/*_TEST; do
    echo "Performing test target: $iTest";
    "$iTest"
    if [ "$?" != "0" ]; then
        echo "Test failed!"
        exit 1
    fi
done

echo "Prepare complete"
