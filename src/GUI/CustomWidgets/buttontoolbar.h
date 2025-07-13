#ifndef BUTTONTOOLBAR_H
#define BUTTONTOOLBAR_H

#include <QIcon>
#include <QPushButton>

namespace ButtonToolbar
{

/**
 * @brief The ButtonConfig class Конфигурация кнопки в виджете
 */
struct ButtonConfig
{
    unsigned    buttonPos {}; //! Положение в виджете от центра (центр -- крайняя точка слева сверху)

    QString     name;       //! Отображаемое название
    QString     tooltip;    //! Описание кнопки в tooltip
    QString     styleSheet; //! QSS стиль кнопки
    QIcon       icon;       //! Иконка кнопки

    std::function<void(QPushButton*)> action;   //! Действие кнопки. Аргумент -- указатель на нажатую кнопку
};

class HeadWidget : public QWidget
{
public:
    explicit HeadWidget(QWidget* parent = nullptr);
    ~HeadWidget();

    void setVertical();
    void setHorizontal();

    void addButton(const ButtonConfig& conf);
    void updateButton(const ButtonConfig& conf);
    void removeButton(unsigned buttonPos);

    void setButtonSize(const QSize& iSize);

private:
    void updateLayout();
    void setupButton(QPushButton *pButton, const ButtonConfig &buttonInfo);

    struct ButtonInfo
    {
        ButtonConfig config;
        QPushButton* pButton {nullptr};
    };
    std::list<ButtonInfo> m_buttons;

    bool m_isVertical {false};
    QSize m_buttonsSize {50, 50};
};

}

#endif // BUTTONTOOLBAR_H
