#include "mainwindow.h"

#include <Common/ImageManager.h>
#include <Common/Logging.h>
#include <CustomWidgets/WaitIndicatorDialog.h>

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
    setWindowTitle(
        QString("Редактор графов (версия %0)").arg(GRAPH_EDITOR_VERSION));
    QIcon windowIcon(":/common/images/icons/app/grapheditor.svg");
    setWindowIcon(windowIcon);

    ImageManager::getInstance();  // Выгружает кэш автоматически

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
