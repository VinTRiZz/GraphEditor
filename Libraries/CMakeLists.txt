# Subdirectories
add_subdirectory(Components-Qt-Custom)
add_subdirectory(AppInfrastructure)
add_subdirectory(GraphObject)
add_subdirectory(GraphItems)
add_subdirectory(GraphWidgets)
add_subdirectory(GraphScene)
add_subdirectory(EditorForm)
add_subdirectory(Filework)


# Custom target to build easier
add_custom_target(
    ALL_LIBRARIES DEPENDS
    ${ADDED_LIBRARIES}
)
