#pragma once

#include <type_traits>
#include <memory>

#include <QObject>

namespace Graph {

class GraphMaintainer;
using PMaintainer = std::shared_ptr<Graph::GraphMaintainer>;


/**
 * @brief The MaintainerUserDecorator class Декоратор, добавляющий базовые методы интеграции мейнтейнера
 */
class MaintainerUserDecorator
{
public:
    void setGraph(const Graph::PMaintainer& pMaintainer);
    Graph::PMaintainer getGraph() const;

private:
    PMaintainer m_graphMaintainer;

protected:
    bool isGraphSet() const;
    virtual void processGraphChange() {}
};

} // namespace Graph

