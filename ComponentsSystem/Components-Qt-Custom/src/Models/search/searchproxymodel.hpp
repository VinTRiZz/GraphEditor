#pragma once

#include <QIdentityProxyModel>

#include "abstractsearchengine.hpp"

namespace QtCustom::Models {

class AbstractSearchEngine;

/**
 * @brief The SearchMode enum Working mode of an search model
 */
enum class SearchMode {
    Highlight = 0,  // Highight wanted indexes
    Filter,         // Hide all unwanted indexes
};

/**
 * @brief The SearchProxyModel class Model to work with index search
 * @note To work correctly, emit signal about filter changed in search engine
 */
class SearchProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit SearchProxyModel(QObject *parent = nullptr);

    void setEngine(AbstractSearchEngine* pEngine);
    AbstractSearchEngine* getEngine() const;

    // Toggles searching through indexes in rows or columns.
    // If both set, goes from left to right for every row
    void setRowSearchEnabled(bool isEn);
    void setColumnSearchEnabled(bool isEn);

    // Index by index search
    QModelIndex getNextInclusion(const QModelIndex& offsetIndex) const;
    QModelIndex getPrevInclusion(const QModelIndex& offsetIndex) const;

    // QIdentityProxyModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    AbstractSearchEngine*   m_searchEngine {nullptr};
    SearchMode              m_searchMode {SearchMode::Highlight};

    bool m_isRowSearchEnabled {true};
    bool m_isColumnSearchEnabled {true};

    void resetSearch();

    // Used to iterate
    QModelIndex getNextIndex(const QModelIndex& offsetIndex) const;
    QModelIndex getPrevIndex(const QModelIndex& offsetIndex) const;

    bool isIndexWanted(const QModelIndex& idx) const;
};

} // namespace QtCustom::Search
