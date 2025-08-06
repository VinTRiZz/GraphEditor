
# Переменную MXE_ROOT необходимо заменять на рут репозитория с MXE.
# Перед сборкой под Windows необходимо собрать MXE:
# user@hostname Deploy/tools/MXE ~> make MXE_TARGETS="x86_64-w64-mingw32.static" qtbase qttools

set(
    MXE_ROOT
    ${CMAKE_CURRENT_SOURCE_DIR}/Deploy/tools/MXE
)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_FIND_ROOT_PATH "${MXE_ROOT}/usr/x86_64-w64-mingw32.static")
message(STATUS "Root path: " ${CMAKE_FIND_ROOT_PATH})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(QT_DIR "${MXE_ROOT}/usr/x86_64-w64-mingw32.static/qt5/lib/cmake/Qt5")
message(STATUS "Qt dir set to: " ${QT_DIR})
