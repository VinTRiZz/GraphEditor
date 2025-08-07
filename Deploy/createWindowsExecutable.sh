
# СКРИПТ СБОРКИ ПАКЕТА ДЛЯ WINDOWS

#! /bin/bash
CREATEWINEXE_SCRIPTDIR=$(realpath $(dirname "$0"))
CREATEWINEXE_BUILDS_DIR="$CREATEAPPIMAGE_SCRIPTDIR/../BIN/Package"

if [ "$?" != "0" ]; then
    echo "Error changing directory"
    exit 1
fi

# Пуллим репозиторий и собираем важности
cd "$CREATEWINEXE_SCRIPTDIR/tools/MXE"
if [ "$?" != "0" ]; then
    echo "Error changing directory to MXE repository"
    exit 1
fi
# git pull origin master &&
# make update &&
# make -j12 MXE_TARGETS="x86_64-w64-mingw32.static" qtbase qttools openssl boost

if [ "$?" != "0" ]; then
    echo "Error configuring MXE for build"
    exit 1
fi

# Необходимо задать перед билдом
MXE_ROOT="$CREATEWINEXE_SCRIPTDIR/tools/MXE"
set PATH "$MXE_ROOT/usr/bin:$PATH"

# Очистка созданного ранее билда (для гарантий, что не использовался кэш)
rm -r "$CREATEWINEXE_SCRIPTDIR/../build" &> /dev/null
rm -r "$CREATEWINEXE_SCRIPTDIR/../BIN" &> /dev/null

# Настройка проекта
mkdir "$CREATEWINEXE_SCRIPTDIR/../build" &> /dev/null
cd "$CREATEWINEXE_SCRIPTDIR/../build"

# Сборка проекта
cmake .. -DCMAKE_TOOLCHAIN_FILE=$CREATEWINEXE_SCRIPTDIR/../cmake/toolchain-mxe.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXE_LINKER_FLAGS="-static -static-libgcc -static-libstdc++" && cmake --build . --parallel --target install
