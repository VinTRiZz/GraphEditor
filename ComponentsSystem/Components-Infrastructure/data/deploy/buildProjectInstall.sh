
# СКРИПТ ДЛЯ УПРОЩЕНИЯ РАБОТЫ СО СБОРКОЙ ПРОЕКТА ИЗ СУБСКРИПТОВ

#! /bin/bash
BUILDPROJINST_SCRIPTDIR=$(realpath $(dirname "$0"))
BUILDPROJINST_BUILDS_DIR="$BUILDPROJINST_SCRIPTDIR/../BIN/Package"

# Очистка созданного ранее билда (для гарантий, что не использовался кэш)
#rm -r "$BUILDPROJINST_SCRIPTDIR/../build" &> /dev/null
#rm -r "$BUILDPROJINST_SCRIPTDIR/../BIN" &> /dev/null

# Настройка проекта
mkdir "$BUILDPROJINST_SCRIPTDIR/../build" &> /dev/null
cd "$BUILDPROJINST_SCRIPTDIR/../build"

if [ "$?" != "0" ]; then
    echo "Error changing directory"
    exit 1
fi

# Сборка проекта
if (command -v ninja); then
    cmake -GNinja $BUILDPROJINST_BUILDARGS .. && ninja install
else
    cmake .. ${BUILDPROJINST_BUILDARGS[@]} && cmake --build . --parallel --target install
fi

# Проверка, собралось ли
if [[ "$?" != "0" ]]; then
    exit 1
fi

exit 0
