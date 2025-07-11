#ifndef GRAPHMODEBASE_H
#define GRAPHMODEBASE_H

#include <QObject>

#include "GUI/CustomWidgets/buttonmatrix.h"

namespace Graph
{

class GraphSceneBase;

class GraphModeBase : public QObject
{
    Q_OBJECT
public:
    explicit GraphModeBase(QObject *parent = nullptr);
    ~GraphModeBase();

    void setGraphScene(GraphSceneBase* pScene);

    virtual void init() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

    virtual bool isRunning() const = 0;

signals:
    void started();
    void stopped();

private:

protected:
    ButtonMatrix::HeadButton* m_buttonMatrixHead {nullptr};
    GraphSceneBase* m_pScene {nullptr};
};

}

#endif // GRAPHMODEBASE_H
