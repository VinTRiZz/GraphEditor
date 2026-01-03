#include "mainwindow.h"

#include <Components/Logger/Logger.h>
#include <Components/Common/ApplicationSettings.h>
#include <Components/CustomQt/WaitIndicatorDialog.h>

#include <QFile>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    auto& waitDialog = WaitIndicatorDialog::getInstance();
    waitDialog.setWindowTitle("GraphEditor");
    waitDialog.setButtonsEnabled(false);

    waitDialog.setTitle("Загрузка приложения...");
    waitDialog.setDescription("Настройка систем...");
    waitDialog.requestShow();

    ui->setupUi(this);

    waitDialog.setProgressBarEnabled(false);
    waitDialog.setDescriptionHidden(true);
    waitDialog.setTitle("Запуск редактора...");
    waitDialog.requestHide(200);

    // Чтобы не появлялось окно раньше времени
    waitDialog.pollHide();
}

MainWindow::~MainWindow() {
    delete ui;
}
