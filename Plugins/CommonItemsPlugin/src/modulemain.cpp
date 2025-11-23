#include "modulemain.hpp"

#include <GraphItems/VertexItem.h>
#include <GraphItems/SimpleVertexItem.h>

#include <Components/Logger/Logger.h>


std::string *getErrorText() {
    return new std::string("All's good!");
}

int initPlugin() {
    LOG_DEBUG("Inited plugin");
    return 0;
}

int restoreState() {
    LOG_DEBUG("Restored state");
    return 0;
}

int deinitPlugin() {
    LOG_DEBUG("Deinited plugin");
    return 0;
}



long long getTypeCount()
{
    return 2;
}

std::string *getItemName(long long itemId)
{
    if (itemId == 0) {
        return new std::string("Aboba object");
    }

    if (itemId == 1) {
        return new std::string("Second object");
    }
    return {};
}

std::string* getTypeString(long long itemId) {
    if (itemId == 0) {
        return new std::string("vertex");
    }

    if (itemId == 1) {
        return new std::string("connection");
    }
    return {};
}



Graph::PluginObjectItnterface *createObject(const std::string &name)
{
    return new Graph::SimpleVertexItem();
}
