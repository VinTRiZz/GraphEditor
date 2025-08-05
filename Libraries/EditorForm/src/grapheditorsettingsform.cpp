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

    // Настройки линий
    connectColorDialog(ui->pushButton_selectColorMain_line, ui->label_cMain_line);
    connectColorDialog(ui->pushButton_selectColorSecond_line, ui->label_cSecond_line);
    connectColorDialog(ui->pushButton_selectColorSelect_line, ui->label_cSelect_line);

    connect(ui->pushButton_acceptSettings, &QPushButton::clicked, this,
            &GraphEditorSettingsForm::applySettings);
}

GraphEditorSettingsForm::~GraphEditorSettingsForm() {
    delete ui;
}

void GraphEditorSettingsForm::loadSettings() {
    auto& appSettings = ApplicationSettings::getInstance();

    auto canvasSizes = appSettings.getCanvasConfig().getCanvasSize();
    ui->spinBox_width->setValue(canvasSizes.width());
    ui->spinBox_height->setValue(canvasSizes.height());
    ui->spinBox_gridSize->setValue(appSettings.getCanvasConfig().getGridSize());

    setColor(ui->label_colorTheme, appSettings.getCanvasConfig().getBackgroundColor());
    setColor(ui->label_colorCanvas, appSettings.getCanvasConfig().getCanvasColor());
    setColor(ui->label_colorGrid, appSettings.getCanvasConfig().getGridColor());

    setColor(ui->label_cMain, appSettings.getObjectsConfig().getNodeMainColor());
    setColor(ui->label_cSecond, appSettings.getObjectsConfig().getNodeSecondColor());
    setColor(ui->label_cSelect, appSettings.getObjectsConfig().getNodeSelectionColor());

    setColor(ui->label_cMain_line, appSettings.getObjectsConfig().getLineMainColor());
    setColor(ui->label_cSecond_line, appSettings.getObjectsConfig().getLineSecondColor());
    setColor(ui->label_cSelect_line, appSettings.getObjectsConfig().getLineSelectionColor());
}

void GraphEditorSettingsForm::applySettings() {
    auto& appSettings = ApplicationSettings::getInstance();
    appSettings.getCanvasConfig().setCanvasSize(
        QSize(ui->spinBox_width->value(), ui->spinBox_height->value()));
    emit updateCanvasSize();

    auto sceneColor = getColor(ui->label_colorTheme);
    appSettings.getCanvasConfig().setBackgroundColor(sceneColor);
    emit updateSceneBrush(sceneColor);

    auto canvasBrush = getColor(ui->label_colorCanvas);
    appSettings.getCanvasConfig().setCanvasColor(canvasBrush);
    emit updateCanvasBrush(canvasBrush);

    auto gridColor = getColor(ui->label_colorGrid);
    appSettings.getCanvasConfig().setGridColor(gridColor);
    emit updateGridColor(gridColor);

    auto mainColor = getColor(ui->label_cMain);
    auto secondColor = getColor(ui->label_cSecond);
    auto selectedColor = getColor(ui->label_cSelect);
    appSettings.getObjectsConfig().setNodeMainColor(mainColor);
    appSettings.getObjectsConfig().setNodeSecondColor(secondColor);
    appSettings.getObjectsConfig().setNodeSelectionColor(selectedColor);

    mainColor = getColor(ui->label_cMain_line);
    secondColor = getColor(ui->label_cSecond_line);
    selectedColor = getColor(ui->label_cSelect_line);
    appSettings.getObjectsConfig().setLineMainColor(mainColor);
    appSettings.getObjectsConfig().setLineSecondColor(secondColor);
    appSettings.getObjectsConfig().setLineSelectionColor(selectedColor);

    appSettings.getCanvasConfig().setGridSize(ui->spinBox_gridSize->value());
    emit updateGridSize(ui->spinBox_gridSize->value());
}

void GraphEditorSettingsForm::showEvent(QShowEvent* e) {
    QWidget::showEvent(e);
    loadSettings();
}
