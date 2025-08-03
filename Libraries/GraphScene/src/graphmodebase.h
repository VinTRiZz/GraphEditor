#ifndef GRAPHMODEBASE_H
#define GRAPHMODEBASE_H

#include <QObject>

#include <CustomWidgets/ButtonMatrix.h>

#include <QGraphicsItem>

namespace Graph
{

class GraphSceneView;

class GraphModeBase : public QObject
{
    Q_OBJECT
public:
    explicit GraphModeBase(QObject *parent = nullptr);
    ~GraphModeBase();

    void setGraphScene(GraphSceneView* pScene);

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
    GraphSceneView* m_pScene {nullptr};

protected:
    void setStarted();
    void setStopped();

    GraphSceneView* getScene() const;

    std::vector<ButtonMatrix::ButtonConfig> m_modeButtonConfiguration;
};

}

#endif // GRAPHMODEBASE_H
