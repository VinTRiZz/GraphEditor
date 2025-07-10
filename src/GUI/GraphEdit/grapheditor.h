#ifndef GRAPHEDITOR_H
#define GRAPHEDITOR_H

#include "graphviewbase.h"

namespace Graph
{

class GraphEditor : virtual public GraphViewBase
{
public:
    GraphEditor(QWidget* parent = nullptr);

    void init() override;

    void startMode() override;
    bool isModeStarted() const override;
    void stopMode() override;

private:
    bool m_isModeStarted {false};
};

}

#endif // GRAPHEDITOR_H
