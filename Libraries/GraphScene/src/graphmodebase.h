#ifndef GRAPHMODEBASE_H
#define GRAPHMODEBASE_H

#include <CustomWidgets/ButtonMatrix.h>

#include <QGraphicsItem>
#include <QObject>

namespace Graph {

class GraphSceneView;
class GraphModeBase;



class GraphSubmodeBase
{
    GraphModeBase* m_pParentMode {nullptr};
public:
    explicit GraphSubmodeBase(GraphModeBase* parentMode) : m_pParentMode{parentMode} {}

    virtual void clearMode() = 0;

    virtual void processPress(QGraphicsItem* pItem) = 0;
    virtual void processMove(QGraphicsItem* pItem,
                             const QPointF& currentPos) = 0;
    virtual void processRelease(QGraphicsItem* pItem) = 0;

    virtual ButtonMatrix::ButtonConfig getStarterButton() = 0;

protected:
    GraphModeBase* getParentMode() const {
        return m_pParentMode;
    }
};



class GraphModeBase : public QObject {
    Q_OBJECT
public:
    explicit GraphModeBase(QObject* parent = nullptr);
    ~GraphModeBase();

    void setGraphScene(GraphSceneView* pScene);

    virtual void init() = 0;
    virtual void start();
    virtual void stop();

    bool isRunning() const;

    GraphSceneView* getScene() const;

signals:
    void started();
    void stopped();

public slots:
    void processPress(QGraphicsItem* pItem);
    void processMove(QGraphicsItem* pItem,
                             const QPointF& currentPos);
    void processRelease(QGraphicsItem* pItem);

private:
    bool m_isModeStarted{false};
    GraphSceneView* m_pScene{nullptr};

    std::list<GraphSubmodeBase*> m_submodes;
    GraphSubmodeBase* m_currentSubmode {nullptr};

protected:
    void setStarted();
    void setStopped();

    void setSubmode(GraphSubmodeBase* pMode);
    GraphSubmodeBase* getCurrentSubmode() const;

    void addSubmode(GraphSubmodeBase* pMode);
    std::list<GraphSubmodeBase*> getSubmodes() const;
};

}  // namespace Graph

#endif  // GRAPHMODEBASE_H
