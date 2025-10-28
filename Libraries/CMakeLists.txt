# Components
add_subdirectory(Components-Logger)
add_subdirectory(Components-ExtraClasses)
add_subdirectory(Components-Maths)
add_subdirectory(Components-Common)
add_subdirectory(Components-Filework)
add_subdirectory(Components-Encryption)
add_subdirectory(Components-Qt-Custom)

# Modules
add_subdirectory(GraphObject)
add_subdirectory(GraphItems)
add_subdirectory(GraphWidgets)
add_subdirectory(GraphScene)
add_subdirectory(EditorForm)
add_subdirectory(SavesModule)
add_subdirectory(PluginMaster)


# Custom target to build easier
add_custom_target(
    ALL_LIBRARIES DEPENDS
    ${ADDED_LIBRARIES}
)
