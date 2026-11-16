#include <gtest/gtest.h>

#ifdef COMPONENTS_SW_qtcustom_models

#include <Components/CustomQt/Models/TreeGroupingProxyModel.h>

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QVector>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

namespace
{

using Proxy_t = QtCustom::Models::TreeGroupingProxyModel;

// ---------------------------------------------------------------------------
// Timeout guard.
//
// Some operations of the model can recurse forever if grouping data is
// malformed (a prototype of that is already handled by the model itself, but
// the guard protects the test binary if that protection regresses).  The guard
// aborts only the current test process: gtest_discover_tests starts every test
// in its own process, so neighbours are not affected.
// ---------------------------------------------------------------------------
class ScopedWatchdog
{
public:
    ScopedWatchdog(std::chrono::milliseconds timeout, const char* testName) :
        m_timeout {timeout},
        m_testName {testName},
        m_thread {[this]() { run(); }}
    {
    }

    ~ScopedWatchdog()
    {
        m_stop.store(true, std::memory_order_release);
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

private:
    void run()
    {
        const auto deadline = std::chrono::steady_clock::now() + m_timeout;
        while (!m_stop.load(std::memory_order_acquire)) {
            if (std::chrono::steady_clock::now() >= deadline) {
                std::fprintf(stderr,
                             "\n[WATCHDOG] Test '%s' did not finish in %lld ms, aborting.\n",
                             m_testName,
                             static_cast<long long>(m_timeout.count()));
                std::fflush(stderr);
                std::abort();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds {2});
        }
    }

    std::chrono::milliseconds m_timeout;
    const char* m_testName;
    std::atomic<bool> m_stop {false};
    std::thread m_thread;
};

#define WITH_TEST_TIMEOUT(timeoutMs) \
    ScopedWatchdog _testWatchdog {std::chrono::milliseconds {timeoutMs}, __func__}

// ---------------------------------------------------------------------------
// Source model helpers.
//
// Columns are deliberately ordered as "leaf -> closest group -> root group":
//   0 : name (a leaf, tree column)
//   1 : region
//   2 : country
// That is exactly the "right to left" grouping order declared by the model:
// country -> region -> name.
// ---------------------------------------------------------------------------
struct City
{
    QString name;
    QString region;
    QString country;
};

QList<QStandardItem*> makeCityItems(const City& city)
{
    return {
        new QStandardItem {city.name},
        new QStandardItem {city.region},
        new QStandardItem {city.country}
    };
}

std::unique_ptr<QStandardItemModel> makeCitySource(const std::vector<City>& cities)
{
    auto pSource = std::make_unique<QStandardItemModel>(0, 3);
    pSource->setHorizontalHeaderLabels({QStringLiteral("Name"), QStringLiteral("Region"), QStringLiteral("Country")});
    for (const auto& city : cities) {
        pSource->appendRow(makeCityItems(city));
    }
    return pSource;
}

void appendCity(QStandardItemModel& sourceModel, const City& city)
{
    sourceModel.appendRow(makeCityItems(city));
}

void insertCity(QStandardItemModel& sourceModel, int row, const City& city)
{
    sourceModel.insertRow(row, makeCityItems(city));
}

// ---------------------------------------------------------------------------
// A representative user of the proxy: it derives the hierarchy from table
// columns, exactly as a real application model would.
// ---------------------------------------------------------------------------
class CityGroupingProxyModel : public Proxy_t
{
public:
    using Proxy_t::Proxy_t;

    // Used by tests that need to inspect data stored through setGroupData().
    bool useStoredGroupData {false};
    // Simulates a broken getParentGroup() implementation.
    bool createParentCycle {false};

protected:
    GroupKey_t getGroup(int sourceModelRow) const override
    {
        auto pSource = sourceModel();
        if (!pSource) {
            return {};
        }

        const auto country = pSource->index(sourceModelRow, 2).data(Qt::DisplayRole).toString();
        const auto region = pSource->index(sourceModelRow, 1).data(Qt::DisplayRole).toString();
        if (country.isEmpty() || region.isEmpty()) {
            return {};
        }
        return QStringLiteral("R|%1|%2").arg(country, region);
    }

    GroupKey_t getParentGroup(const GroupKey_t& groupKey) const override
    {
        if (createParentCycle) {
            return groupKey;
        }

        const auto parts = groupKey.toString().split('|');
        if (parts.size() == 3 && parts.at(0) == QLatin1String("R")) {
            return QStringLiteral("C|%1").arg(parts.at(1));
        }
        return {};
    }

    QVariant getGroupData(GroupKey_t groupKey, int column, int role) const override
    {
        if (useStoredGroupData) {
            return Proxy_t::getGroupData(groupKey, column, role);
        }
        if (role != Qt::DisplayRole || column != 0) {
            return {};
        }

        const auto parts = groupKey.toString().split('|');
        if (parts.size() == 2 && parts.at(0) == QLatin1String("C")) {
            return parts.at(1);
        }
        if (parts.size() == 3 && parts.at(0) == QLatin1String("R")) {
            return parts.at(2);
        }
        return {};
    }
};

// ---------------------------------------------------------------------------
// Structural snapshot helpers.
//
// A signature looks like "[A,B]" for a group with two leaf children and is
// nested for deeper levels.  Group nodes that have no children ("[ ]") are
// reported as separate problems, because the model must not expose empty
// branches.
// ---------------------------------------------------------------------------
QString leafName(const Proxy_t& model, const QModelIndex& index)
{
    return model.data(index, Qt::DisplayRole).toString();
}

QString buildSignature(const Proxy_t& model,
                       const QModelIndex& parent,
                       QStringList* pProblems,
                       QVector<int>* pLeafSourceRows)
{
    QStringList parts;
    const int rows = model.rowCount(parent);
    for (int row = 0; row < rows; ++row) {
        const auto child = model.index(row, 0, parent);
        if (!child.isValid()) {
            pProblems->append(QStringLiteral("Invalid child index at row %1").arg(row));
            continue;
        }
        if (child.model() != &model) {
            pProblems->append(QStringLiteral("Child index belongs to another model at row %1").arg(row));
        }
        if (child.parent() != parent) {
            pProblems->append(QStringLiteral("parent(index(%1)) does not match the traversal parent").arg(row));
        }

        const auto sourceIndex = model.mapToSource(child);
        if (sourceIndex.isValid()) {
            // Leaf node: exactly one source row.
            if (pLeafSourceRows) {
                pLeafSourceRows->append(sourceIndex.row());
            }
            const auto name = leafName(model, child);
            if (name.isEmpty()) {
                pProblems->append(QStringLiteral("Leaf at row %1 has empty display data").arg(row));
            }
            parts.append(name.isEmpty() ? QStringLiteral("<empty>") : name);
            continue;
        }

        if (model.rowCount(child) == 0) {
            pProblems->append(QStringLiteral("Empty group branch at row %1").arg(row));
        }
        parts.append(buildSignature(model, child, pProblems, pLeafSourceRows));
    }
    return QStringLiteral("[") + parts.join(',') + QStringLiteral("]");
}

void expectTree(const Proxy_t& model,
                const QString& expectedSignature,
                const QAbstractItemModel* pSource)
{
    QStringList problems;
    QVector<int> leafSourceRows;
    const auto actualSignature = buildSignature(model, QModelIndex(), &problems, &leafSourceRows);

    for (const auto& problem : problems) {
        ADD_FAILURE() << problem.toStdString();
    }
    EXPECT_EQ(expectedSignature.toStdString(), actualSignature.toStdString());

    if (!pSource) {
        return;
    }

    // Every source row must be present exactly once (no missing and no
    // duplicated leaves).
    auto sortedRows = leafSourceRows;
    std::sort(sortedRows.begin(), sortedRows.end());
    EXPECT_EQ(pSource->rowCount(), sortedRows.size());
    for (int i = 0; i < sortedRows.size(); ++i) {
        EXPECT_EQ(i, sortedRows.at(i));
    }
}

void verifyMappings(const Proxy_t& model,
                     const QAbstractItemModel& sourceModel,
                     bool allColumns)
{
    const int firstColumn = allColumns ? 0 : model.treeColumn();
    const int lastColumn = allColumns ? sourceModel.columnCount() : model.treeColumn() + 1;

    for (int row = 0; row < sourceModel.rowCount(); ++row) {
        for (int column = firstColumn; column < lastColumn; ++column) {
            const auto sourceIndex = sourceModel.index(row, column);
            const auto proxyIndex = model.mapFromSource(sourceIndex);
            ASSERT_TRUE(proxyIndex.isValid())
                << "mapFromSource failed for source row " << row << " column " << column;
            EXPECT_EQ(&model, proxyIndex.model());
            const auto roundTrip = model.mapToSource(proxyIndex);
            ASSERT_TRUE(roundTrip.isValid());
            EXPECT_TRUE(sourceIndex == roundTrip)
                << "mapToSource(mapFromSource(index)) mismatch for source row " << row
                << " column " << column;
        }
    }
}

// ---------------------------------------------------------------------------
// Tests.
// ---------------------------------------------------------------------------

TEST(Components_QtCustom_TreeGroupingProxyModel, EmptyModelIsSafe)
{
    Proxy_t model;

    EXPECT_EQ(nullptr, model.sourceModel());
    EXPECT_EQ(0, model.treeColumn());
    EXPECT_EQ(0, model.rowCount());
    EXPECT_FALSE(model.index(0, 0).isValid());
    EXPECT_FALSE(model.index(-1, 0).isValid());
    EXPECT_FALSE(model.parent(QModelIndex()).isValid());
    EXPECT_FALSE(model.data(QModelIndex(), Qt::DisplayRole).isValid());
    EXPECT_EQ(0, static_cast<int>(model.flags(QModelIndex())));
    EXPECT_FALSE(model.headerData(0, Qt::Horizontal, Qt::DisplayRole).isValid());
    EXPECT_FALSE(model.mapToSource(QModelIndex()).isValid());
    EXPECT_FALSE(model.mapFromSource(QModelIndex()).isValid());
    EXPECT_FALSE(model.setData(QModelIndex(), 1, Qt::EditRole));

    model.setSourceModel(nullptr);
    EXPECT_EQ(nullptr, model.sourceModel());
    EXPECT_EQ(0, model.rowCount());
}

TEST(Components_QtCustom_TreeGroupingProxyModel, DefaultGroupingCreatesOneGroupPerRow)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")},
        {QStringLiteral("C"), QStringLiteral("ON"), QStringLiteral("Canada")},
        {QStringLiteral("D"), QStringLiteral("CA"), QStringLiteral("USA")}
    });

    Proxy_t model;
    model.setSourceModel(pSource.get());

    // The default implementation promises "one row - one group": every source
    // row is wrapped into its own top level group and the original order is
    // preserved.
    expectTree(model, QStringLiteral("[[A],[B],[C],[D]]"), pSource.get());
    verifyMappings(model, *pSource, false);
    EXPECT_EQ(pSource->rowCount(), model.rowCount());
}

TEST(Components_QtCustom_TreeGroupingProxyModel, CityLikeModelBuildsCountryRegionLeafTree)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")},
        {QStringLiteral("C"), QStringLiteral("ON"), QStringLiteral("Canada")},
        {QStringLiteral("D"), QStringLiteral("CA"), QStringLiteral("USA")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pSource.get());

    expectTree(model, QStringLiteral("[[[A,D],[B]],[[C]]]"), pSource.get());
    verifyMappings(model, *pSource, false);

    // Group labels must describe the groups they belong to.
    const auto usa = model.index(0, 0);
    ASSERT_TRUE(usa.isValid());
    EXPECT_EQ(QStringLiteral("USA"), model.data(usa, Qt::DisplayRole).toString());
    const auto ca = model.index(0, 0, usa);
    ASSERT_TRUE(ca.isValid());
    EXPECT_EQ(QStringLiteral("CA"), model.data(ca, Qt::DisplayRole).toString());
    EXPECT_EQ(2, model.rowCount(ca));

    const auto canada = model.index(1, 0);
    ASSERT_TRUE(canada.isValid());
    EXPECT_EQ(QStringLiteral("Canada"), model.data(canada, Qt::DisplayRole).toString());
}

TEST(Components_QtCustom_TreeGroupingProxyModel, SettingSourceModelDoesNotEmitGranularInsertions)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")},
        {QStringLiteral("C"), QStringLiteral("ON"), QStringLiteral("Canada")}
    });

    CityGroupingProxyModel model;

    int resets {0};
    int aboutToInsert {0};
    int inserted {0};
    QObject::connect(&model, &QAbstractItemModel::modelAboutToBeReset, [&resets]() { ++resets; });
    QObject::connect(&model, &QAbstractItemModel::rowsAboutToBeInserted,
                     [&aboutToInsert](const QModelIndex&, int, int) { ++aboutToInsert; });
    QObject::connect(&model, &QAbstractItemModel::rowsInserted,
                     [&inserted](const QModelIndex&, int, int) { ++inserted; });

    model.setSourceModel(pSource.get());

    // Populating a model for the first time is a reset, not a sequence of row
    // insertions.  Granular insertions during a reset make views and
    // persistent indexes inconsistent.
    EXPECT_EQ(0, aboutToInsert);
    EXPECT_EQ(0, inserted);
    EXPECT_GE(resets, 1);
}

TEST(Components_QtCustom_TreeGroupingProxyModel, AddingRowsUsesConsistentInsertPositions)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")},
        {QStringLiteral("C"), QStringLiteral("ON"), QStringLiteral("Canada")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pSource.get());

    struct InsertEvent
    {
        int first {0};
        int last {0};
        int rowCountBefore {0};
    };

    QVector<InsertEvent> events;
    int insertedCount {0};
    QObject::connect(&model, &QAbstractItemModel::rowsAboutToBeInserted,
                     [&model, &events](const QModelIndex& parent, int first, int last) {
        events.push_back(InsertEvent {first, last, model.rowCount(parent)});
    });
    QObject::connect(&model, &QAbstractItemModel::rowsInserted,
                     [&insertedCount](const QModelIndex&, int, int) { ++insertedCount; });

    // Both rows are appended to the source table.  New groups created for
    // them are therefore appended to their parents as well, so every reported
    // insertion position must equal the row count observed before insertion.
    appendCity(*pSource, {QStringLiteral("E"), QStringLiteral("QC"), QStringLiteral("Canada")});
    appendCity(*pSource, {QStringLiteral("F"), QStringLiteral("BR"), QStringLiteral("Brazil")});

    ASSERT_FALSE(events.isEmpty());
    for (const auto& event : events) {
        EXPECT_GE(event.first, 0);
        // The model only ever appends new items, therefore the reported first
        // row must be the row count observed before the insertion.
        EXPECT_EQ(event.rowCountBefore, event.first);
        EXPECT_EQ(event.first, event.last);
    }
    EXPECT_EQ(events.size(), insertedCount);
}

TEST(Components_QtCustom_TreeGroupingProxyModel, InsertingRowInMiddlePreservesSourceOrder)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")},
        {QStringLiteral("C"), QStringLiteral("ON"), QStringLiteral("Canada")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pSource.get());

    // N is inserted before A and shares A's group.  Source order is N, A, B, C.
    insertCity(*pSource, 0, {QStringLiteral("N"), QStringLiteral("CA"), QStringLiteral("USA")});

    expectTree(model, QStringLiteral("[[[N,A],[B]],[[C]]]"), pSource.get());
    verifyMappings(model, *pSource, false);
}

TEST(Components_QtCustom_TreeGroupingProxyModel, RemovingRowsPrunesEmptyGroupsAndKeepsOrder)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")},
        {QStringLiteral("C"), QStringLiteral("ON"), QStringLiteral("Canada")},
        {QStringLiteral("D"), QStringLiteral("CA"), QStringLiteral("USA")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pSource.get());
    expectTree(model, QStringLiteral("[[[A,D],[B]],[[C]]]"), pSource.get());

    pSource->removeRow(0); // A: B, C, D
    expectTree(model, QStringLiteral("[[[D],[B]],[[C]]]"), pSource.get());

    pSource->removeRow(2); // D: B, C -> the CA group must disappear
    expectTree(model, QStringLiteral("[[[B]],[[C]]]"), pSource.get());

    pSource->removeRow(1); // C: B -> the Canada group must disappear
    expectTree(model, QStringLiteral("[[[B]]]"), pSource.get());

    pSource->removeRow(0); // B: empty model
    expectTree(model, QStringLiteral("[]"), pSource.get());
    EXPECT_EQ(0, model.rowCount());
}

TEST(Components_QtCustom_TreeGroupingProxyModel, MovingRowBetweenGroupsDoesNotDuplicateIt)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("C"), QStringLiteral("NY"), QStringLiteral("USA")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pSource.get());
    expectTree(model, QStringLiteral("[[[A,B],[C]]]"), pSource.get());

    // Source order is A, B, C.  B moves from CA to NY and must be rendered
    // exactly once, before C (strict source order).  The CA branch stays the
    // first one because it appeared first in the source table.
    ASSERT_TRUE(pSource->setData(pSource->index(1, 1), QStringLiteral("NY"), Qt::EditRole));

    expectTree(model, QStringLiteral("[[[A],[B,C]]]"), pSource.get());
    verifyMappings(model, *pSource, false);
}

TEST(Components_QtCustom_TreeGroupingProxyModel, ChangingLeafDataKeepsPosition)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")},
        {QStringLiteral("C"), QStringLiteral("ON"), QStringLiteral("Canada")},
        {QStringLiteral("D"), QStringLiteral("CA"), QStringLiteral("USA")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pSource.get());

    ASSERT_TRUE(pSource->setData(pSource->index(0, 0), QStringLiteral("A-updated"), Qt::EditRole));
    // Renaming a leaf does not change grouping, so the tree shape must stay
    // intact and the new label must be visible.
    expectTree(model, QStringLiteral("[[[A-updated,D],[B]],[[C]]]"), pSource.get());
    EXPECT_EQ(QStringLiteral("A-updated"), pSource->index(0, 0).data(Qt::DisplayRole).toString());
}

TEST(Components_QtCustom_TreeGroupingProxyModel, SettingGroupDataTargetsAddressedGroup)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")},
        {QStringLiteral("C"), QStringLiteral("ON"), QStringLiteral("Canada")}
    });

    CityGroupingProxyModel model;
    model.useStoredGroupData = true;
    model.setSourceModel(pSource.get());

    const auto usa = model.index(0, 0);
    ASSERT_TRUE(usa.isValid());
    EXPECT_FALSE(model.mapToSource(usa).isValid());
    const auto ca = model.index(0, 0, usa);
    ASSERT_TRUE(ca.isValid());

    ASSERT_TRUE(model.setData(usa, QStringLiteral("United States"), Qt::DisplayRole));

    EXPECT_EQ(QStringLiteral("United States"), model.data(usa, Qt::DisplayRole).toString());
    EXPECT_TRUE(model.data(ca, Qt::DisplayRole).toString().isEmpty())
        << "setData() on a group index modified another group";
}

TEST(Components_QtCustom_TreeGroupingProxyModel, SettingLeafDataForwardsToSourceModel)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pSource.get());

    const auto leaf = model.mapFromSource(pSource->index(0, 0));
    ASSERT_TRUE(leaf.isValid());
    ASSERT_TRUE(model.setData(leaf, QStringLiteral("A-renamed"), Qt::EditRole));

    EXPECT_EQ(QStringLiteral("A-renamed"), pSource->index(0, 0).data(Qt::EditRole).toString());
    EXPECT_EQ(QStringLiteral("A-renamed"), model.data(leaf, Qt::EditRole).toString());
}

TEST(Components_QtCustom_TreeGroupingProxyModel, MapFromSourceSupportsEveryColumn)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")},
        {QStringLiteral("C"), QStringLiteral("ON"), QStringLiteral("Canada")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pSource.get());

    // A proxy must map indexes of every source column, not only the tree
    // column.  This also checks the mapToSource round trip.
    verifyMappings(model, *pSource, true);
}

TEST(Components_QtCustom_TreeGroupingProxyModel, ForeignAndInvalidIndexesAreRejected)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pSource.get());

    QStandardItemModel foreignModel(1, 1);
    foreignModel.setItem(0, 0, new QStandardItem {QStringLiteral("foreign")});
    const auto foreignIndex = foreignModel.index(0, 0);

    EXPECT_FALSE(model.mapToSource(foreignIndex).isValid());
    EXPECT_FALSE(model.mapFromSource(foreignIndex).isValid());
    EXPECT_FALSE(model.setData(foreignIndex, QStringLiteral("hacked"), Qt::EditRole));
    EXPECT_FALSE(model.data(foreignIndex, Qt::DisplayRole).isValid());

    EXPECT_FALSE(model.index(-1, 0).isValid());
    EXPECT_FALSE(model.index(model.rowCount(), 0).isValid());
    EXPECT_FALSE(model.index(0, -1).isValid());
    EXPECT_FALSE(model.parent(QModelIndex()).isValid());
    EXPECT_FALSE(model.setData(QModelIndex(), 1, Qt::EditRole));

    // A foreign index must not modify any data of the source model.
    EXPECT_EQ(QStringLiteral("A"), pSource->index(0, 0).data(Qt::DisplayRole).toString());
}

TEST(Components_QtCustom_TreeGroupingProxyModel, RowsWithoutGroupDataStayAtTopLevel)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QString(), QStringLiteral("USA")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pSource.get());

    // B has no region/group data and therefore lives at the top level.  It
    // must not create an empty group branch.
    expectTree(model, QStringLiteral("[[[A]],B]"), pSource.get());
    verifyMappings(model, *pSource, false);
}

TEST(Components_QtCustom_TreeGroupingProxyModel, SourceModelResetRebuildsTree)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pSource.get());
    expectTree(model, QStringLiteral("[[[A],[B]]]"), pSource.get());

    pSource->clear();
    EXPECT_EQ(0, model.rowCount());

    appendCity(*pSource, {QStringLiteral("D"), QStringLiteral("CA"), QStringLiteral("USA")});
    appendCity(*pSource, {QStringLiteral("C"), QStringLiteral("ON"), QStringLiteral("Canada")});

    expectTree(model, QStringLiteral("[[[D]],[[C]]]"), pSource.get());
    verifyMappings(model, *pSource, false);
}

TEST(Components_QtCustom_TreeGroupingProxyModel, ReplacingSourceModelDropsOldData)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pOldSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")},
        {QStringLiteral("B"), QStringLiteral("NY"), QStringLiteral("USA")}
    });
    const auto pNewSource = makeCitySource({
        {QStringLiteral("X"), QStringLiteral("XR"), QStringLiteral("Mexico")},
        {QStringLiteral("Y"), QStringLiteral("YR"), QStringLiteral("Mexico")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pOldSource.get());
    model.setSourceModel(pNewSource.get());

    expectTree(model, QStringLiteral("[[[X],[Y]]]"), pNewSource.get());
    EXPECT_EQ(pNewSource.get(), model.sourceModel());

    // Changes of the replaced model must not be reflected any more.
    appendCity(*pOldSource, {QStringLiteral("Z"), QStringLiteral("CA"), QStringLiteral("USA")});
    expectTree(model, QStringLiteral("[[[X],[Y]]]"), pNewSource.get());
}

TEST(Components_QtCustom_TreeGroupingProxyModel, HeaderDataColumnCountAndFlagsAreForwarded)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")}
    });

    CityGroupingProxyModel model;
    model.setSourceModel(pSource.get());

    EXPECT_EQ(pSource->columnCount(), model.columnCount());
    EXPECT_EQ(QStringLiteral("Name"), model.headerData(0, Qt::Horizontal, Qt::DisplayRole).toString());
    EXPECT_EQ(QStringLiteral("Region"), model.headerData(1, Qt::Horizontal, Qt::DisplayRole).toString());
    EXPECT_EQ(QStringLiteral("Country"), model.headerData(2, Qt::Horizontal, Qt::DisplayRole).toString());

    const auto leaf = model.mapFromSource(pSource->index(0, 0));
    ASSERT_TRUE(leaf.isValid());
    EXPECT_TRUE(pSource->flags(pSource->index(0, 0)) == model.flags(leaf));
}

TEST(Components_QtCustom_TreeGroupingProxyModel, ParentGroupCycleIsDetected)
{
    WITH_TEST_TIMEOUT(3000);

    const auto pSource = makeCitySource({
        {QStringLiteral("A"), QStringLiteral("CA"), QStringLiteral("USA")}
    });

    CityGroupingProxyModel model;
    model.createParentCycle = true;

    // A broken getParentGroup() must not hang the model forever.
    EXPECT_THROW(model.setSourceModel(pSource.get()), std::runtime_error);
}

} // namespace

#endif // COMPONENTS_SW_qtcustom_models