#pragma once

#include <string>

namespace GraphEditor {

constexpr int EVENT_TYPE_UNDEFINED {};

/**
 * @brief The Event class   Used to provide events inside graph
 */
class GraphEvent
{
public:
    explicit GraphEvent(int evType) : m_type {evType} {}

    // To work with processing events
    void setAccepted() & { m_isAccepted = true; }
    bool isAccepted() const & noexcept { return m_isAccepted; }

    int getType() const & noexcept { return m_type; }

    // Used to print information in debug needs
    virtual std::string getDebugString() const & noexcept = 0;

private:
    int m_type {0};
    bool m_isAccepted {false};
};

}