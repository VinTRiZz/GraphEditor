#! /bin/bash
CAI_SCRIPTDIR=$(realpath $(pwd))
CAI_BUILDS_DIR="$CAI_SCRIPTDIR/../BIN/Package"

# Build install target
mkdir ../build &> /dev/null
cd ../build && cmake .. -DRELEASE=1
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
CURRENT_PROJECT_VERSION=$(grep -o 'VERSION \([0-9]\.\)\{2,\}.*' "$CAI_SCRIPTDIR/../CMakeLists.txt")
CURRENT_PROJECT_VERSION=${CURRENT_PROJECT_VERSION/VERSION /}
cp "$CAI_BUILDS_DIR/GraphEditor_$CURRENT_PROJECT_VERSION.bin" "$CAI_BUILDS_DIR/usr/bin/GraphEditor.bin"
cp -r "$CAI_BUILDS_DIR/../lib/" ../
cp "$CAI_SCRIPTDIR/../DATA/images/icons/app/GraphEditor.png" "$CAI_BUILDS_DIR/GraphEditor.png"
sed "s/^Exec\=.*/Exec=GraphEditor-$CURRENT_PROJECT_VERSION.bin/" "$CAI_SCRIPTDIR/../DATA/desktop/GraphEditor.desktop" > "$CAI_BUILDS_DIR/PREPARE_GraphEditor.desktop"
sed "s/^Name\=.*/Name=GraphEditor-$CURRENT_PROJECT_VERSION/" -i "$CAI_BUILDS_DIR/PREPARE_GraphEditor.desktop"

# Process deploy
"$CAI_SCRIPTDIR/tools/linuxdeploy-x86_64.AppImage" --appdir "$CAI_BUILDS_DIR" --output appimage -e "$CAI_BUILDS_DIR/GraphEditor-$CURRENT_PROJECT_VERSION.bin" --icon-file="$CAI_BUILDS_DIR/GraphEditor.png" --desktop-file="$CAI_BUILDS_DIR/PREPARE_GraphEditor.desktop"
rm -r ../lib
