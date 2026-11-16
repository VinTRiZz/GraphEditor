#pragma once

#include "helper_structures.hpp"

#ifdef COMPONENTS_IS_ENABLED_QT

#include <QString>
#include <QVariant>

#endif // Qt only

namespace Logger
{

template <typename StreamT, typename T>
void writeData(StreamT&& oStream, T&& oValue) {
    using BaseT = std::decay_t<T>;
    if constexpr (IsMap<BaseT>::value) {
        writeData(oStream, "{");
        if (oValue.empty()) {
            writeData(oStream, "}");
            return;
        }

        auto pIt = oValue.begin();
        writeData(oStream, pIt->first);
        writeData(oStream, ":");
        writeData(oStream, pIt->second);
        std::advance(pIt, 1);

        while (pIt != oValue.end()) {
            writeData(oStream, "|");
            writeData(oStream, pIt->first);
            writeData(oStream, ":");
            writeData(oStream, pIt->second);
            std::advance(pIt, 1);
        }
        writeData(oStream, "}");

    } else if constexpr (IsContainer<BaseT>::value) {
        writeData(oStream, "{");
        if (oValue.empty()) {
            writeData(oStream, "}");
            return;
        }

        auto pIt = oValue.begin();
        writeData(oStream, *pIt);
        std::advance(pIt, 1);

        while (pIt != oValue.end()) {
            writeData(oStream, "|");
            writeData(oStream, *pIt);
            std::advance(pIt, 1);
        }
        writeData(oStream, "}");
    } else if constexpr (IsOptional<BaseT>::value) {
        if (oValue.has_value()) {
            writeData(oStream, oValue.value());
        } else {
            writeData(oStream, "[NULL]");
        }
    }
#ifdef COMPONENTS_IS_ENABLED_QT
    else if constexpr(std::is_same_v<BaseT, std::string>) {
        oStream << oValue.c_str();
    }
    else if constexpr(std::is_same_v<BaseT, QVariant>) {
        writeData(oStream, "QVariant(");
        writeData(oStream, oValue.isNull() ? "NULL" : (oValue.typeName() + oValue.toString()));
        writeData(oStream, ")");
    }
    else if constexpr(std::is_same_v<BaseT, QPoint> || std::is_same_v<BaseT, QPointF>) {
        writeData(oStream, "{");
        writeData(oStream, oValue.x());
        writeData(oStream, ";");
        writeData(oStream, oValue.y());
        writeData(oStream, "}");
    }
#else
    else if constexpr(std::is_same_v<BaseT, bool>) {
        writeData(oStream, oValue ? "true" : "false");
    }
#endif // Qt only

    else if constexpr(std::is_same_v<BaseT, std::string_view>) {
        writeData(oStream, std::string(oValue));
    }

    else {

#ifdef COMPONENTS_IS_ENABLED_QT
    if constexpr(std::is_same_v<std::decay_t<StreamT>, QDebug>) { // corner case
        oStream << oValue;
    } else
#endif // Qt only
        oStream << oValue << " ";
    }
}

}