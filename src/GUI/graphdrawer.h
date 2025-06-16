#ifndef GRAPHDRAWER_H
#define GRAPHDRAWER_H

#include "objectscene.h"
#include "graphobject.h"
#include "overlaybuttonlist.h"

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
    uint connectionRectLayer = 11;   //! Уровень прямоугольников с названиями вершин (для контраста)
    uint connectionTextLayer = 12;   //! Уровень текста названий вершин

    uint vertexLayer         = 30;   //! Уровень вершин
    uint vertexDataLayer     = 31;   //! Уровень текста названий вершин

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
    void setScene(ObjectScene* pScene);

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
    ObjectScene*            m_pScene            {nullptr};  //! Сцена, на которой находятся объекты для отрисовки
    Graph::GraphObject*     m_pGraph            {nullptr};  //! Граф, который будет отрисовываться
    OverlayButtonList*      m_pOverlayButton    {nullptr};  //! Кнопка, в которой будет меню

    std::vector<OverlayButtonList::ButtonInfo> m_drawButtonsInfo;
    std::vector<OverlayButtonList::ButtonInfo> m_viewButtonsInfo;
    std::vector<OverlayButtonList::ButtonInfo> m_previousButtons;

    CurrentDrawerMode m_currentMode {CurrentDrawerMode::None}; //! Текущий режим работы мастера. От него зависит поведение и кнопки оверлея
};

#endif // GRAPHDRAWER_H
