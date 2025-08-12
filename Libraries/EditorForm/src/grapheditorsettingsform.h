#ifndef GRAPHEDITORSETTINGSFORM_H
#define GRAPHEDITORSETTINGSFORM_H

#include <QWidget>

namespace Ui {
class GraphEditorSettingsForm;
}

/**
 * @brief The GraphEditorSettingsForm class Форма для редактирования настроек
 */
class GraphEditorSettingsForm : public QWidget {
    Q_OBJECT

public:
    explicit GraphEditorSettingsForm(QWidget* parent = nullptr);
    ~GraphEditorSettingsForm();

signals:
    /**
     * @brief updateCanvasSize  Сигнал, что необходимо изменить размер полотна
     */
    void updateCanvasSize();

    /**
     * @brief updateSceneBrush  Сигнал, что надо задать новую кисть сцены
     * @param sceneBrush        Новая кисть
     */
    void updateSceneBrush(const QBrush& sceneBrush);

    /**
     * @brief updateCanvasBrush Сигнал, что надо задать новую кисть полотна
     * @param canvasBrush       Новая кисть
     */
    void updateCanvasBrush(const QBrush& canvasBrush);

    /**
     * @brief updateCanvasOpacity   Сигнал, что надо задать прозрачность полотна
     * @param opac                  Прозрачность, от 0 до 1
     */
    void updateCanvasOpacity(double opac);

    /**
     * @brief updateGridSize    Сигнал, что надо обновить длину стороны сетки
     * @param gSize             Длина стороны сетки
     */
    void updateGridSize(int gSize);

    /**
     * @brief updateGridLineWidth   Сигнал, что надо обновить ширину линии сетки
     * @param gSize                 Ширина линии сетки
     */
    void updateGridLineWidth(double gSize);

    /**
     * @brief updateGridColor   Сигнал, что надо обновить цвет линий сетки
     * @param gColor            Цвет линий сетки
     */
    void updateGridColor(const QColor& gColor);

public slots:
    // Загрузка и выгрузка настроек в класс-синглетон настроек
    void loadSettings();
    void applySettings();

private slots:
    /**
     * @brief setCanvasSizeType Слот задания типа соотношения размеров в полотне (внутренний)
     * @param sizesTypeInt      Тип соотношения размеров (см. enum внутри cpp файла)
     */
    void setCanvasSizeType(int sizesTypeInt);

private:
    Ui::GraphEditorSettingsForm* ui;

    QSize m_screenSize;
    double m_widthMultiplyCoefficient {1};

    /**
     * @brief connectSizesSignals   Соединяет сигналы, связанные с заданием размеров и соотношений
     */
    void connectSizesSignals();

    // QWidget
    void showEvent(QShowEvent* e) override;
};

#endif  // GRAPHEDITORSETTINGSFORM_H
