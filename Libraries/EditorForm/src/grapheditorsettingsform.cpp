#include "grapheditorsettingsform.h"

#include <Common/CommonFunctions.h>

#include "ui_grapheditorsettingsform.h"
using namespace CommonFunctions;

#include <Common/ApplicationSettings.h>

#include <QGuiApplication>
#include <QScreen>

#include <Common/Logging.h>

#include <math.h>

enum class MultiplyCoeffType
{
    Custom = 0,
    Screen,
    A0,
    A1,
    A2,
    A3,
    A4,
    A5
};

GraphEditorSettingsForm::GraphEditorSettingsForm(QWidget* parent)
    : QWidget(parent), ui(new Ui::GraphEditorSettingsForm) {
    ui->setupUi(this);

    auto appScreens = qApp->screens();
    if (appScreens.isEmpty()) {
        throw std::runtime_error("No screens found! Can not configure settings");
    }
    m_screenSize = appScreens.front()->size();

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

    connectSizesSignals();
}

GraphEditorSettingsForm::~GraphEditorSettingsForm() {
    delete ui;
}

void GraphEditorSettingsForm::loadSettings() {
    LOG_INFO_SYNC("Loading settings");

    auto& appSettings = ApplicationSettings::getInstance();

    auto canvasSizes = appSettings.getCanvasConfig().getCanvasSize();
    ui->spinBox_width->setValue(canvasSizes.width());
    ui->spinBox_height->setValue(canvasSizes.height());
    ui->spinBox_gridSize->setValue(appSettings.getCanvasConfig().getGridSize());
    ui->doubleSpinBox_gridLineWidth->setValue(appSettings.getCanvasConfig().getGridLineWidth());
    ui->checkBox_enableGrid->setChecked(appSettings.getCanvasConfig().getIsGridEnabled());

    setColor(ui->label_colorTheme, appSettings.getCanvasConfig().getBackgroundColor());
    setColor(ui->label_colorCanvas, appSettings.getCanvasConfig().getCanvasColor());
    setColor(ui->label_colorGrid, appSettings.getCanvasConfig().getGridColor());

    setColor(ui->label_cMain, appSettings.getObjectsConfig().getNodeMainColor());
    setColor(ui->label_cSecond, appSettings.getObjectsConfig().getNodeSecondColor());
    setColor(ui->label_cSelect, appSettings.getObjectsConfig().getNodeSelectionColor());

    setColor(ui->label_cMain_line, appSettings.getObjectsConfig().getLineMainColor());
    setColor(ui->label_cSecond_line, appSettings.getObjectsConfig().getLineSecondColor());
    setColor(ui->label_cSelect_line, appSettings.getObjectsConfig().getLineSelectionColor());

    LOG_OK("Settings loaded");
}

void GraphEditorSettingsForm::applySettings() {
    LOG_INFO_SYNC("Applying settings");

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

    appSettings.getCanvasConfig().setGridLineWidth(ui->doubleSpinBox_gridLineWidth->value());
    emit updateGridLineWidth(appSettings.getCanvasConfig().getGridLineWidth());

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

    appSettings.getCanvasConfig().setIsGridEnabled(ui->checkBox_enableGrid->isChecked());

    LOG_OK("Settings applied");
}

void GraphEditorSettingsForm::setCanvasSizeType(int sizesTypeInt)
{
    LOG_INFO_SYNC("Selecting multiply coeff type:", sizesTypeInt);

    auto isAlbumOrientation = ui->checkBox_isAlbumOrientation->isChecked();
    m_widthMultiplyCoefficient = 1.0; // Для задания ширины перед расчётами

    switch(MultiplyCoeffType(sizesTypeInt))
    {
    case MultiplyCoeffType::Custom:
        m_widthMultiplyCoefficient = ui->doubleSpinBox_multiplyCoefficient->value();
        break;

    case MultiplyCoeffType::Screen:
        ui->spinBox_width->setValue(isAlbumOrientation ? m_screenSize.height() : m_screenSize.width());
        m_widthMultiplyCoefficient = static_cast<double>(m_screenSize.height()) / static_cast<double>(m_screenSize.width());
        break;

    case MultiplyCoeffType::A0: ui->spinBox_width->setValue(isAlbumOrientation ? 841.0 : 1189.0); m_widthMultiplyCoefficient = (1189.0 / 841.0); break;
    case MultiplyCoeffType::A1: ui->spinBox_width->setValue(isAlbumOrientation ? 594.0 : 841.0); m_widthMultiplyCoefficient = (841.0 / 594.0); break;
    case MultiplyCoeffType::A2: ui->spinBox_width->setValue(isAlbumOrientation ? 420.0 : 594.0); m_widthMultiplyCoefficient = (594.0 / 420.0); break;
    case MultiplyCoeffType::A3: ui->spinBox_width->setValue(isAlbumOrientation ? 297.0 : 420.0); m_widthMultiplyCoefficient = (420.0 / 297.0); break;
    case MultiplyCoeffType::A4: ui->spinBox_width->setValue(isAlbumOrientation ? 210.0 : 297.0); m_widthMultiplyCoefficient = (297.0 / 210.0); break;
    case MultiplyCoeffType::A5: ui->spinBox_width->setValue(isAlbumOrientation ? 148.0 : 210.0); m_widthMultiplyCoefficient = (210.0 / 148.0); break;
    default:
        throw std::invalid_argument("Invalid type of multiply coefficient!");
    }

    // Обновляем значения
    auto canvasW = static_cast<double>(ui->spinBox_width->value());
    auto canvasH = std::round(m_widthMultiplyCoefficient * canvasW);
    ui->spinBox_height->setValue(canvasH);
}

void GraphEditorSettingsForm::connectSizesSignals()
{
    connect(ui->checkBox_calcSizes, &QCheckBox::clicked,
            this, [this](bool isCalcSizesChecked){
        ui->doubleSpinBox_multiplyCoefficient->setEnabled(isCalcSizesChecked);
        ui->checkBox_isAlbumOrientation->setEnabled(isCalcSizesChecked);
        ui->comboBox_calcSizesType->setEnabled(isCalcSizesChecked);
    });

    connect(ui->checkBox_isAlbumOrientation, &QCheckBox::clicked,
            this, [this](bool isAlbumOrient){
        auto tmpv = ui->spinBox_width->value();
        ui->spinBox_width->setValue(ui->spinBox_height->value());
        ui->spinBox_height->setValue(tmpv);
        m_widthMultiplyCoefficient = 1.0 / m_widthMultiplyCoefficient;
    });

    connect(ui->comboBox_calcSizesType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GraphEditorSettingsForm::setCanvasSizeType);

    connect(ui->spinBox_width, &QSpinBox::editingFinished,
            this, [this](){
        if (!ui->checkBox_calcSizes->isChecked()) {
            return;
        }
        auto canvasW = static_cast<double>(ui->spinBox_width->value());
        auto canvasH = std::round(m_widthMultiplyCoefficient * canvasW);
        ui->spinBox_height->setValue(canvasH);
    });

    connect(ui->spinBox_height, &QSpinBox::editingFinished,
            this, [this](){
        if (!ui->checkBox_calcSizes->isChecked()) {
            return;
        }
        auto canvasH = static_cast<double>(ui->spinBox_height->value());
        auto canvasW = std::round(canvasH / m_widthMultiplyCoefficient);
        ui->spinBox_width->setValue(canvasW);
    });

    connect(ui->doubleSpinBox_multiplyCoefficient, &QDoubleSpinBox::editingFinished,
            this, [this](){
        m_widthMultiplyCoefficient = ui->doubleSpinBox_multiplyCoefficient->value();
        auto canvasW = static_cast<double>(ui->spinBox_width->value());
        auto canvasH = std::round(m_widthMultiplyCoefficient * canvasW);
        ui->spinBox_height->setValue(canvasH);
    });
}

void GraphEditorSettingsForm::showEvent(QShowEvent* e) {
    QWidget::showEvent(e);
    loadSettings();
}
