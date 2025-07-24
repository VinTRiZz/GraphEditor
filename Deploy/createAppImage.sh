#! /bin/bash
CAI_SCRIPTDIR=$(realpath $(pwd))
CAI_BUILDS_DIR="$CAI_SCRIPTDIR/../BIN/Package"

# Build install target
mkdir ../build &> /dev/null
cd ../build && cmake ..
cmake --build . --parallel --target install

if [[ "$?" != "0" ]]; then
    echo "Error building install target"
    exit 1
fi

# Configure for building
cd "$CAI_SCRIPTDIR"
cp -r "$CAI_SCRIPTDIR/Configurations/AppImage/usr" "$CAI_BUILDS_DIR"

# Dir to contain App image
APPIMAGE_DIR="Results/$(date +%H%M_%d%m%Y)"
mkdir $APPIMAGE_DIR
cd $APPIMAGE_DIR

# Setup binaries
mkdir "$CAI_BUILDS_DIR/usr/bin"
cp "$CAI_BUILDS_DIR/../GraphEditor.DEBUG" "$CAI_BUILDS_DIR/usr/bin/GraphEditor.bin"

# Process deploy
"$CAI_SCRIPTDIR/tools/linuxdeploy-x86_64.AppImage" --appdir "$CAI_BUILDS_DIR" --output appimage
