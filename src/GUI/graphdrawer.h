#ifndef GRAPHDRAWER_H
#define GRAPHDRAWER_H

#include "GUI/ObjectScene/objectview.h"
#include "Graph/graphobject.h"
#include "GUI/overlaybuttonlist.h"

#include <QGraphicsItem>

#include <boost/noncopyable.hpp>

/**
 * @brief The GraphConversionConfiguration class Структура для настройки конверсий графов
 */
struct GraphConversionConfiguration : boost::noncopyable
{
    // Уровни расположения объектов на сцене по их типу
    uint mainRectLayer       = 1;    //! Уровень главного объекта ("полотна" графа)

    uint connectionLineLayer = 10;   //! Уровень линий соединения вершин

    uint vertexLayer         = 30;   //! Уровень вершин
    uint vertexDataRectLayer = 31;   //! Уровень рамки текста названий вершин
    uint vertexDataLayer     = 32;   //! Уровень текста названий вершин

    static GraphConversionConfiguration& getInstance() {
        static GraphConversionConfiguration inst;
        return inst;
    }
};

/**
 * @brief The GraphDrawer class Мастер отрисовки графов на сцене
 */
class GraphDrawer
{
public:
    GraphDrawer();
    ~GraphDrawer();

    /**
     * @brief The CurrentDrawerMode enum Режимы работы мастера отрисовки графов
     */
    enum class CurrentDrawerMode : uint8_t {
        None = 0,
        Edit,
        View,
    };
    CurrentDrawerMode getCurrentMode() const;

    /**
     * @brief setScene  Выбрать целевую сцену
     * @param pScene    Указатель на сцену
     */
    void setScene(ObjectView* pScene);

    /**
     * @brief setCurrentGraph   Задать текущий граф
     * @param pGraph            Указатель на текущий граф
     */
    void setCurrentGraph(Graph::GraphObject* pGraph);

    /**
     * @brief setOverlayButtonList  Задать кнопку оверлея для задания внутри неё меню
     * @param pOverlayButton        Указатель на кнопку оверлея
     */
    void setOverlayButtonList(OverlayButtonList* pOverlayButton);

    /**
     * @brief init   Инициализация класса. Обязательно необходимо вызвать после задания ВСЕХ полей
     */
    void init();

    /**
     * @brief updateGraph Обновить отрисовываемый граф
     */
    void updateGraph();

    /**
     * @brief startEditMode Запуск режима редактирования графа
     */
    void startEditMode();

    /**
     * @brief startViewMode Запуск режима обзора графа (поиск, чтение и т.д.)
     */
    void startViewMode();

    /**
     * @brief stopMode Остановить текущий режим работы мастера редактирования графа
     */
    void stopMode();

private:
    ObjectView*             m_pSceneView        {nullptr};  //! Сцена, на которой находятся объекты для отрисовки
    Graph::GraphObject*     m_pGraph            {nullptr};  //! Граф, который будет отрисовываться
    OverlayButtonList*      m_pOverlayButton    {nullptr};  //! Кнопка, в которой будет меню

    /**
     * @brief The ModeInformation class Информация о режиме
     */
    struct ModeInformation {
        CurrentDrawerMode                           mode        {CurrentDrawerMode::None};
        QMenu*                                      contextMenu {nullptr};
        std::vector<OverlayButtonList::ButtonInfo>  buttons;
    };
    std::vector<ModeInformation>            m_availableModes;
    std::vector<ModeInformation>::iterator  m_currentMode;

    /**
     * @brief setupAvailableModes   Настройка доступных режимов работы
     */
    void setupAvailableModes();

    /**
     * @brief setupCurrentMode  Настроить выбранный режим работы
     */
    void setupCurrentMode();
};

#endif // GRAPHDRAWER_H
