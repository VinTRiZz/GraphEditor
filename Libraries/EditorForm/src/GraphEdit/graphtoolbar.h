#ifndef GRAPHTOOLBAR_H
#define GRAPHTOOLBAR_H

#include <CustomWidgets/ButtonToolbar.h>

class GraphToolbar : public ButtonToolbar::HeadWidget
{
    Q_OBJECT
public:
    explicit GraphToolbar(QWidget* parent = nullptr);

    void setShowPropertiesEnabled(bool isSaveEnabled);

    enum class GraphEditorMode : int
    {
        EditBasicGraph,
        ViewInteraction,
        Algorithmic,
        CrimeInvestigation
    };
    void setModeEnabled(GraphEditorMode editorMode, bool isLoadEnabled);

    void setSaveEnabled(bool isSaveEnabled);
    void setSaveAsEnabled(bool isSaveAsEnabled);

    void setLoadEnabled(bool isLoadEnabled);

signals:
    void showProperties();
    void hideProperties();

    void createGraph();
    void saveGraph(const QString& sPath);
    void loadGraph(const QString& lPath);

    void changeMode();

private:
    std::map<GraphEditorMode, int> m_modeIndexes;
    unsigned m_showPropertiesButtonIndex {0};
    unsigned m_saveButtonIndex {0};
    unsigned m_saveAsButtonIndex {0};
    unsigned m_loadButtonIndex {0};
};

#endif // GRAPHTOOLBAR_H

