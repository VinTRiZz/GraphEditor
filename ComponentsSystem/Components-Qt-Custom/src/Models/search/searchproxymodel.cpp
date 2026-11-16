#include "searchproxymodel.hpp"

#include <Components/Logger/Logger.h>

#include <QColor>

namespace QtCustom::Models {

SearchProxyModel::SearchProxyModel(QObject *parent)
    : QIdentityProxyModel{parent}
{

}

void SearchProxyModel::setEngine(AbstractSearchEngine *pEngine)
{
    beginResetModel();
    if (m_searchEngine) {
        disconnect(m_searchEngine, nullptr, this, nullptr);
    }
    m_searchEngine = pEngine;
    if (m_searchEngine) {
        connect(m_searchEngine, &AbstractSearchEngine::sig_filterChanged,
                this, &SearchProxyModel::resetSearch);
    }
    endResetModel();
}

AbstractSearchEngine *SearchProxyModel::getEngine() const
{
    return m_searchEngine;
}

void SearchProxyModel::setRowSearchEnabled(bool isEn)
{
    m_isRowSearchEnabled = isEn;
    resetSearch();
}

void SearchProxyModel::setColumnSearchEnabled(bool isEn)
{
    m_isColumnSearchEnabled = isEn;
    resetSearch();
}

QModelIndex SearchProxyModel::getNextInclusion(const QModelIndex &offsetIndex) const
{
    if (m_searchMode == SearchMode::Filter) {
        return getNextIndex(offsetIndex);
    }
    auto nextIndex = getNextIndex(offsetIndex);
    while (nextIndex.isValid() && !isIndexWanted(nextIndex)) {
        nextIndex = getNextIndex(nextIndex);
    }
    return nextIndex;
}

QModelIndex SearchProxyModel::getPrevInclusion(const QModelIndex &offsetIndex) const
{
    if (m_searchMode == SearchMode::Filter) {
        return getPrevIndex(offsetIndex);
    }
    auto prevIndex = getPrevIndex(offsetIndex);
    while (prevIndex.isValid() && !isIndexWanted(prevIndex)) {
        prevIndex = getPrevIndex(prevIndex);
    }
    return prevIndex;
}

int SearchProxyModel::rowCount(const QModelIndex &parent) const
{
    return QIdentityProxyModel::rowCount(parent);
}

QModelIndex SearchProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return QIdentityProxyModel::mapFromSource(sourceIndex);
}

QVariant SearchProxyModel::data(const QModelIndex &index, int role) const
{
    if (m_searchMode == SearchMode::Highlight &&
        role == Qt::BackgroundRole &&
        isIndexWanted(index)) {
        return QColor(230, 250, 110, 100);
    }
    return QIdentityProxyModel::data(index, role);
}

void SearchProxyModel::resetSearch()
{
    if (m_searchMode == SearchMode::Filter) {
        // TODO: Update correctly?
        beginResetModel();
        endResetModel();
    } else {
        // TODO: Correctly update recursively?
        emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1), { Qt::BackgroundRole });
    }
}

QModelIndex SearchProxyModel::getNextIndex(const QModelIndex& offsetIndex) const
{
    if (!offsetIndex.isValid()) {
        return index(0, 0);
    }
    if (columnCount(offsetIndex.parent()) > offsetIndex.column()) {
        return index(offsetIndex.row(), offsetIndex.column() + 1, offsetIndex.parent());
    }
    if (rowCount(offsetIndex.parent()) > offsetIndex.row()) {
        return index(offsetIndex.row() + 1, 0, offsetIndex.parent());
    }
    return {};
}

QModelIndex SearchProxyModel::getPrevIndex(const QModelIndex& offsetIndex) const
{
    if (!offsetIndex.isValid()) {
        return index(rowCount() - 1, columnCount() - 1); // TODO: Return correctly
    }
    if (offsetIndex == index(0, 0)) {
        return {};
    }
    if (0 < offsetIndex.column()) {
        return index(offsetIndex.row(), offsetIndex.column() - 1, offsetIndex.parent());
    }
    if (0 < offsetIndex.row()) {
        return index(offsetIndex.row() - 1, columnCount(offsetIndex.parent()), offsetIndex.parent());
    }
    return {};
}

bool SearchProxyModel::isIndexWanted(const QModelIndex &idx) const
{
    if (!m_searchEngine) {
        return true;
    }
    return m_searchEngine->isWanted(idx);
}

} // namespace QtCustom::Search
