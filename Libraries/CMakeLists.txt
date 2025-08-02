# Subdirectories
add_subdirectory(Common)
add_subdirectory(CustomWidgets)
add_subdirectory(GraphObject)
add_subdirectory(ObjectItems)
add_subdirectory(ObjectScene)
add_subdirectory(EditorForm)
add_subdirectory(Filework)


# Custom target to build easier
add_custom_target(
    ALL_LIBRARIES DEPENDS
    ${ADDED_LIBRARIES}
)
