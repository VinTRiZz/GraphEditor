#! /bin/bash
CREATEAPPIMAGE_SCRIPTDIR=$(realpath $(dirname "$0"))
CREATEAPPIMAGE_BUILDS_DIR="$CREATEAPPIMAGE_SCRIPTDIR/../BIN/Package"

rm -r "$CREATEAPPIMAGE_SCRIPTDIR/../build" &> /dev/null
rm -r "$CREATEAPPIMAGE_SCRIPTDIR/../BIN" &> /dev/null

# Build install target
mkdir "$CREATEAPPIMAGE_SCRIPTDIR/../build" &> /dev/null
cd "$CREATEAPPIMAGE_SCRIPTDIR/../build" && cmake ..
cmake --build . --parallel --target install

if [[ "$?" != "0" ]]; then
    echo "Error building install target"
    exit 1
fi

# Configure for building
cd "$CREATEAPPIMAGE_SCRIPTDIR"

# Setup binaries
CURRENT_PROJECT_VERSION=$(grep -o "VERSION \\([0-9]\\{1,\}\\.\\)\\{2,\\}.*" "$CREATEAPPIMAGE_SCRIPTDIR/../CMakeLists.txt")
CURRENT_PROJECT_VERSION=${CURRENT_PROJECT_VERSION//VERSION /}

# Dir to contain App image
APPIMAGE_DIR="$CREATEAPPIMAGE_SCRIPTDIR/Results/$(date +%H%M_%d%m%Y)"
mkdir $APPIMAGE_DIR
cd $APPIMAGE_DIR

cp -r "$CREATEAPPIMAGE_BUILDS_DIR/../lib/" ../
cp "$CREATEAPPIMAGE_SCRIPTDIR/../DATA/images/icons/app/GraphEditor.png" "$CREATEAPPIMAGE_BUILDS_DIR/GraphEditor.png"
sed "s/^Exec\=.*/Exec=GraphEditor-$CURRENT_PROJECT_VERSION.bin/; s/^Name\=.*/Name=GraphEditor-$CURRENT_PROJECT_VERSION/" "$CREATEAPPIMAGE_SCRIPTDIR/../DATA/desktop/GraphEditor.desktop" > "$CREATEAPPIMAGE_BUILDS_DIR/PREPARE_GraphEditor.desktop"

# Process deploy
"$CREATEAPPIMAGE_SCRIPTDIR/tools/linuxdeploy-x86_64.AppImage" --appdir "$CREATEAPPIMAGE_BUILDS_DIR" --output appimage -e "$CREATEAPPIMAGE_BUILDS_DIR/GraphEditor-$CURRENT_PROJECT_VERSION.bin" --icon-file="$CREATEAPPIMAGE_BUILDS_DIR/GraphEditor.png" --desktop-file="$CREATEAPPIMAGE_BUILDS_DIR/PREPARE_GraphEditor.desktop"
rm -r ../lib

# Вернёмся откуда пришли
cd -
