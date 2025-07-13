#ifndef GRAPHMODEBASE_H
#define GRAPHMODEBASE_H

#include <QObject>

#include "GUI/CustomWidgets/buttonmatrix.h"

#include <QGraphicsItem>

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

    virtual void processPress(QGraphicsItem* pItem) = 0;
    virtual void processMove(QGraphicsItem* pItem, const QPointF& currentPos) = 0;
    virtual void processRelease(QGraphicsItem* pItem) = 0;

    virtual void init() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

    bool isRunning() const;

signals:
    void started();
    void stopped();

private:
    bool m_isModeStarted {false};
    GraphSceneBase* m_pScene {nullptr};
    ButtonMatrix::HeadButton* m_buttonMatrixHead {nullptr};

protected:
    void setStarted();
    void setStopped();

    GraphSceneBase* getScene() const;
    ButtonMatrix::HeadButton* getButtonMatrixHead() const;

    std::vector<ButtonMatrix::ButtonConfig> m_modeButtonConfiguration;
};

}

#endif // GRAPHMODEBASE_H
