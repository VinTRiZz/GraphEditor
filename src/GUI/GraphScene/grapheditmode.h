#ifndef GRAPHEDITOR_H
#define GRAPHEDITOR_H

#include "graphmodebase.h"

namespace Graph
{

class GraphEditMode : public GraphModeBase
{
    Q_OBJECT
public:
    GraphEditMode(QWidget* parent = nullptr);

    void init() override;

    void start() override;
    bool isRunning() const override;
    void stop() override;

signals:
    void enabledEdit();
    void disabledEdit();

private:
    bool m_isModeStarted {false};
};

}

#endif // GRAPHEDITOR_H
