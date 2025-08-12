#ifndef GRAPHOPTIONSTOOLBAR_H
#define GRAPHOPTIONSTOOLBAR_H

#include <CustomWidgets/ButtonToolbar.h>

/**
 * @brief The GraphOptionsToolbar class Тулбар для работы с различными инструментами и режимами
 */
class GraphOptionsToolbar : public ButtonToolbar::HeadWidget {
    Q_OBJECT
public:
    explicit GraphOptionsToolbar(QWidget* parent = nullptr);

    /**
     * @brief The GraphEditorMode enum  Виды режимов работы редактора
     */
    enum GraphEditorMode : int {
        EditBasicGraph,
        ViewInteraction,
        Algorithmic,
        CrimeInvestigation
    };

    /**
     * @brief setModeEnabled    Задать возможность переключения в режим
     * @param editorMode
     * @param isModeEnabled
     */
    void setModeEnabled(GraphEditorMode editorMode, bool isModeEnabled);

signals:
    void openSettings();

private:
    std::map<GraphEditorMode, int> m_modeIndexes;
};

#endif  // GRAPHOPTIONSTOOLBAR_H
