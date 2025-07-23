# Qt configuration
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Subdirectories
add_subdirectory(Common)
add_subdirectory(CustomWidgets)
add_subdirectory(GraphObject)
add_subdirectory(ObjectScene)
add_subdirectory(EditorForm)
add_subdirectory(Filework)


# Custom target to build easier
add_custom_target(
    ALL_LIBRARIES DEPENDS
    ${ADDED_LIBRARIES}
)
