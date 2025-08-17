#ifndef BUTTONTOOLBAR_H
#define BUTTONTOOLBAR_H

#include <QIcon>
#include <QPushButton>

namespace ButtonToolbar {

/**
 * @brief The ButtonConfig class Конфигурация кнопки в виджете
 */
struct ButtonConfig {
    unsigned buttonPos{};  //! Положение в виджете от центра (центр -- крайняя
                           //! точка слева сверху)

    QString name;          //! Отображаемое название
    QString tooltip;       //! Описание кнопки в tooltip
    QString styleSheet;    //! QSS стиль кнопки
    QIcon icon;            //! Иконка кнопки
    bool isEnabled{true};  //! Изначальное состояние

    std::function<void(QPushButton*)>
        action;  //! Действие кнопки. Аргумент -- указатель на нажатую кнопку
};

/**
 * @brief The HeadWidget class Форма для отображения тулбара кнопок, слева направо или сверху вниз
 */
class HeadWidget : public QWidget {
public:
    explicit HeadWidget(QWidget* parent = nullptr);
    ~HeadWidget();

    /**
     * @brief setVertical Переключить режим отображения в вертикальный
     */
    void setVertical();

    /**
     * @brief setHorizontal Переключить режим отображения в горизонтальный
     */
    void setHorizontal();

    /**
     * @brief addButton Добавить кнопку
     * @param conf      Информация по кнопке для добавления
     */
    void addButton(const ButtonConfig& conf);

    /**
     * @brief updateButton  Обновить конфигурацию кнопки. Задайте верно позицию кнопки для работы
     * @param conf          Новая конфигурация
     */
    void updateButton(const ButtonConfig& conf);

    /**
     * @brief setButtonEnabled  Включить или выключить кнопку на позиции
     * @param buttonPos         Позиция кнопки, слева направо или сверху вниз
     * @param isButtonEnabled
     */
    void setButtonEnabled(unsigned buttonPos, bool isButtonEnabled);

    /**
     * @brief removeButton  Удалить кнопку из тулбара
     * @param buttonPos     Позиция кнопки, слева направо или сверху вниз
     */
    void removeButton(unsigned buttonPos);

    /**
     * @brief setButtonSize Задать размер каждой кнопки тулбара
     * @param iSize         Размеры
     */
    void setButtonSize(const QSize& iSize);

private:
    /**
     * @brief updateLayout  Обновить тулбар, разместив кнопки вертикально или горизонтально
     */
    void updateLayout();

    /**
     * @brief setupButton   Настроить кнопку
     * @param pButton       Указатель на кнопку
     * @param buttonInfo    Информация по которой надо настроить кнопку
     */
    void setupButton(QPushButton* pButton, const ButtonConfig& buttonInfo);

    /**
     * @brief The ButtonInfo class  Информация по кнопке в массиве
     */
    struct ButtonInfo {
        ButtonConfig config;
        QPushButton* pButton{nullptr};
    };
    std::list<ButtonInfo> m_buttons;    //! Кнопки

    bool m_isVertical{false};       //! Тип расположения кнопок. Если false, то горизонтальный
    QSize m_buttonsSize{50, 50};    //! Размер кнопок для коррекции из-за Qt-вских особенностей работы

    void resizeEvent(QResizeEvent* e) override;
};

}  // namespace ButtonToolbar

#endif  // BUTTONTOOLBAR_H
