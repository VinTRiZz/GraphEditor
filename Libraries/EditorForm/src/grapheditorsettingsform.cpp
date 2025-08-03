#include "grapheditorsettingsform.h"

#include <Common/CommonFunctions.h>

#include "ui_grapheditorsettingsform.h"
using namespace CommonFunctions;

#include <Common/ApplicationSettings.h>

GraphEditorSettingsForm::GraphEditorSettingsForm(QWidget* parent)
    : QWidget(parent), ui(new Ui::GraphEditorSettingsForm) {
    ui->setupUi(this);

    // Настройки полотна
    connectColorDialog(ui->pushButton_selectColorCanvas, ui->label_colorCanvas);
    connectColorDialog(ui->pushButton_selectColorTheme, ui->label_colorTheme);
    connectColorDialog(ui->pushButton_selectColorGrid, ui->label_colorGrid);

    // Настройки вершин
    connectColorDialog(ui->pushButton_selectColorMain, ui->label_cMain);
    connectColorDialog(ui->pushButton_selectColorSecond, ui->label_cSecond);
    connectColorDialog(ui->pushButton_selectColorSelect, ui->label_cSelect);

    connect(ui->pushButton_acceptSettings, &QPushButton::clicked, this,
            &GraphEditorSettingsForm::applySettings);
}

GraphEditorSettingsForm::~GraphEditorSettingsForm() {
    delete ui;
}

void GraphEditorSettingsForm::loadSettings() {
    auto& appSettings = ApplicationSettings::getInstance();

    auto canvasSizes = appSettings.getCanvasSize();
    ui->spinBox_width->setValue(canvasSizes.width());
    ui->spinBox_height->setValue(canvasSizes.height());
}

void GraphEditorSettingsForm::applySettings() {
    auto& appSettings = ApplicationSettings::getInstance();
    appSettings.setCanvasSize(
        QSize(ui->spinBox_width->value(), ui->spinBox_height->value()));
    emit updateCanvasSize();
}

void GraphEditorSettingsForm::showEvent(QShowEvent* e) {
    QWidget::showEvent(e);
    loadSettings();
}
