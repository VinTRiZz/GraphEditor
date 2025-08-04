#! /bin/bash
CREATEAPPIMAGE_SCRIPTDIR=$(realpath $(dirname "$0"))
CREATEAPPIMAGE_BUILDS_DIR="$CREATEAPPIMAGE_SCRIPTDIR/../BIN/Package"

# Очистка созданного ранее билда (для гарантий, что не использовался кэш)
rm -r "$CREATEAPPIMAGE_SCRIPTDIR/../build" &> /dev/null
rm -r "$CREATEAPPIMAGE_SCRIPTDIR/../BIN" &> /dev/null

# Настройка проекта
mkdir "$CREATEAPPIMAGE_SCRIPTDIR/../build" &> /dev/null
cd "$CREATEAPPIMAGE_SCRIPTDIR/../build"

# Сборка проекта
if (command -v ninja); then
    cmake -GNinja .. && ninja install
else
    cmake .. && cmake --build . --parallel --target install
fi

# Проверка, собралось ли
if [[ "$?" != "0" ]]; then
    echo "Error building install target"
    exit 1
fi

# Перемещаемся в директорию скрипта
cd "$CREATEAPPIMAGE_SCRIPTDIR"

# Поиск версии проекта
CURRENT_PROJECT_VERSION=$(grep -o "VERSION \\([0-9]\\{1,\}\\.\\)\\{2,\\}.*" "$CREATEAPPIMAGE_SCRIPTDIR/../CMakeLists.txt")
CURRENT_PROJECT_VERSION=${CURRENT_PROJECT_VERSION//VERSION /}

# Директория с AppImage в результате
APPIMAGE_DIR="$CREATEAPPIMAGE_SCRIPTDIR/Results/$(date +%H%M_%d%m%Y)"
mkdir $APPIMAGE_DIR
cd $APPIMAGE_DIR

# Копирование библиотек проекта
cp -r "$CREATEAPPIMAGE_BUILDS_DIR/../lib/" ../
cp "$CREATEAPPIMAGE_SCRIPTDIR/../DATA/images/icons/app/GraphEditor.png" "$CREATEAPPIMAGE_BUILDS_DIR/GraphEditor.png"
sed "s/^Exec\=.*/Exec=GraphEditor-$CURRENT_PROJECT_VERSION.bin/; s/^Name\=.*/Name=GraphEditor-$CURRENT_PROJECT_VERSION/" "$CREATEAPPIMAGE_SCRIPTDIR/../DATA/desktop/GraphEditor.desktop" > "$CREATEAPPIMAGE_BUILDS_DIR/PREPARE_GraphEditor.desktop"

# Для того, чтобы linuxdeploy нашёл плагин Qt
set PATH "$PATH:$CREATEAPPIMAGE_SCRIPTDIR/tools"

# Создание непосредственно AppImage
"$CREATEAPPIMAGE_SCRIPTDIR/tools/linuxdeploy-x86_64.AppImage" --plugin qt --appdir "$CREATEAPPIMAGE_BUILDS_DIR" --output appimage -e "$CREATEAPPIMAGE_BUILDS_DIR/GraphEditor-$CURRENT_PROJECT_VERSION.bin" --icon-file="$CREATEAPPIMAGE_BUILDS_DIR/GraphEditor.png" --desktop-file="$CREATEAPPIMAGE_BUILDS_DIR/PREPARE_GraphEditor.desktop"
rm -r ../lib
