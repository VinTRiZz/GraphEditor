#include <QStandardItemModel>
#include <QColor>
#include <QRandomGenerator>
#include <Qt>

/**
 * Creates a test model with Russian data, random background colours,
 * and random check states on the first column.
 */
QStandardItemModel* createTestModel()
{
    const int rowCount = 10;

    // ----- constant data (all values in Russian) -----
    const QStringList countries = {
        "Россия", "США", "Китай", "Индия", "Бразилия",
        "Германия", "Франция", "Великобритания", "Япония", "Италия"
    };
    const QStringList regions = {
        "Московская область", "Калифорния", "Гуандун", "Махараштра", "Сан-Паулу",
        "Бавария", "Иль-де-Франс", "Англия", "Канто", "Ломбардия"
    };
    const QStringList cities = {
        "Москва", "Лос-Анджелес", "Гуанчжоу", "Мумбаи", "Сан-Паулу",
        "Мюнхен", "Париж", "Лондон", "Токио", "Милан"
    };
    const QList<int> populations = {
        12600000, 4000000, 15000000, 20000000, 12000000,
        1500000, 2200000, 9000000, 14000000, 1400000
    };

    // ----- create model with 4 columns -----
    QStandardItemModel* model = new QStandardItemModel(rowCount, 4);
    model->setHorizontalHeaderLabels({"Страна", "Регион", "Город", "Население"});

    // ----- fill in a loop -----
    for (int i = 0; i < rowCount; ++i) {
        // Create items
        QStandardItem* countryItem   = new QStandardItem(countries[i]);
        QStandardItem* regionItem    = new QStandardItem(regions[i]);
        QStandardItem* cityItem      = new QStandardItem(cities[i]);
        QStandardItem* populationItem = new QStandardItem(QString::number(populations[i]));

        // ----- decoration: random background colour (light shades) -----
        QColor color(QRandomGenerator::global()->bounded(200) + 55,  // R: 55..255
                     QRandomGenerator::global()->bounded(200) + 55,
                     QRandomGenerator::global()->bounded(200) + 55);
        QBrush brush(color);

        countryItem->setBackground(brush);
        regionItem->setBackground(brush);
        cityItem->setBackground(brush);
        populationItem->setBackground(brush);

        // ----- decoration: check state (random) on the first column -----
        countryItem->setCheckable(true);
        Qt::CheckState state = (QRandomGenerator::global()->bounded(2) == 0)
                                   ? Qt::Unchecked
                                   : Qt::Checked;
        countryItem->setCheckState(state);

        // Place items into the model
        model->setItem(i, 0, countryItem);
        model->setItem(i, 1, regionItem);
        model->setItem(i, 2, cityItem);
        model->setItem(i, 3, populationItem);
    }

    return model;
}
