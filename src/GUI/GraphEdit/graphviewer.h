#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include "graphviewbase.h"

namespace Graph
{

class GraphViewer : virtual public GraphViewBase
{
public:
    GraphViewer(QWidget* parent = nullptr);

    void init() override;

    void startMode() override;
    bool isModeStarted() const override;
    void stopMode() override;

private:
    bool m_isModeStarted {false};
};

}

#endif // GRAPHVIEWER_H
