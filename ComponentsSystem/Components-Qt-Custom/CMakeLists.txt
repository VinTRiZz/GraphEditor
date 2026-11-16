COMPONENTS_CCR_ADD_QT(5 Gui Network Sql Widgets TextToSpeech)
COMPONENTS_CONFIGURE_COMPONENT(Qt-Custom "1.0.0")
option(COMPONENTS_SW_qtcustom_models "" ON)
option(COMPONENTS_SW_qtcustom_qt_charts "" OFF)
option(COMPONENTS_SW_qtcustom_graphicscene "" OFF)
option(COMPONENTS_SW_qtcustom_web "" OFF)
option(COMPONENTS_SW_qtcustom_orm "" OFF)
option(COMPONENTS_SW_qtcustom_multimedia "" OFF)
option(COMPONENTS_SW_qtcustom_secure "" OFF)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Logger)
target_link_libraries(Components-Qt-Custom PRIVATE Qt5::Gui Qt5::Network Qt5::Sql Qt5::Widgets)
COMPONENTS_ADD_COMPONENT_TEST(Qt-Custom)
if (COMPONENTS_SW_qtcustom_models)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_SW_qtcustom_models)
    COMPONENTS_LINK_COMPONENT(Components-Qt-Custom ExtraClasses)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Models/proxy/treegroupingproxymodel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Models/proxy/treegroupingproxymodel.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Models/search/abstractsearchengine.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Models/search/abstractsearchengine.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Models/search/searchproxymodel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Models/search/searchproxymodel.hpp"
    )
endif()
if (COMPONENTS_SW_qtcustom_qt_charts)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_SW_qtcustom_qt_charts)
    target_link_libraries(Components-Qt-Custom PRIVATE Qt5::Charts)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/QtCharts/callout.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/QtCharts/callout.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/QtCharts/chartwidget.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/QtCharts/chartwidget.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/QtCharts/chartwidget.ui"
    )
endif()
if (COMPONENTS_SW_qtcustom_graphicscene)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_SW_qtcustom_graphicscene)
    COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Maths)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/abstractconnectionline.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/abstractconnectionline.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/abstracttext.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/abstracttext.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/anchoritem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/anchoritem.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/arrowedconnectionline.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/arrowedconnectionline.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/basicitem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/basicitem.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/basiciteminterface.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/basiciteminterface.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/commentaryitem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/commentaryitem.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/debugmaster.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/debugmaster.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/elegantconnectionline.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/elegantconnectionline.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/groupitem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/groupitem.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/includeall.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/markeritem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/markeritem.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/textlabel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectItems/textlabel.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectView/ovcanvaslayer.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectView/ovcanvaslayer.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectView/ovconstants.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectView/ovconstants.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectView/ovcontextmenulayer.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectView/ovinformationlayer.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectView/ovinformationlayer.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectView/ovinternalscene.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsScene/ObjectView/ovinternalscene.hpp"
    )
endif()
if (COMPONENTS_SW_qtcustom_web)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_SW_qtcustom_web)
    COMPONENTS_LINK_COMPONENT(Components-Qt-Custom ExtraClasses)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Web/http/httpclientbase.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Web/http/httpclientbase.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Web/http/httpcommon.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Web/http/httpcommon.hpp"
    )
endif()
if (COMPONENTS_SW_qtcustom_orm)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_SW_qtcustom_orm)
    COMPONENTS_LINK_COMPONENT(Components-Qt-Custom ExtraClasses)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tableitembase.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tableitembase.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tableitemmaster.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tableitemmaster.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tablesourcemodel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tablesourcemodel.hpp"
    )
endif()
if (COMPONENTS_SW_qtcustom_multimedia)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_SW_qtcustom_multimedia)
    target_link_libraries(Components-Qt-Custom PRIVATE Qt5::TextToSpeech)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Multimedia/texttospeech.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Multimedia/texttospeech.hpp"
    )
endif()
if (COMPONENTS_SW_qtcustom_secure)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_SW_qtcustom_secure)
    COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Encryption)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/Etc/securelineedit.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/Etc/securelineedit.cpp"
    )
endif()