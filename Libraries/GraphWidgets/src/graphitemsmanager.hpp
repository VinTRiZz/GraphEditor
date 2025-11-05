#pragma once

#include <QWidget>

namespace Ui {
class GraphItemsManager;
}

class GraphItemsManager : public QWidget
{
    Q_OBJECT

public:
    explicit GraphItemsManager(QWidget *parent = nullptr);
    ~GraphItemsManager();

private:
    Ui::GraphItemsManager *ui;
};

