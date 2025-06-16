#ifndef OVERLAYBUTTONLIST_H
#define OVERLAYBUTTONLIST_H

#include <QPushButton>
#include <QPropertyAnimation>

#include <optional>

#include <map>

/**
 * @brief The OverlayButtonList class Виджет, отображающий основную и добавленные кнопки
 * @note  Представляет из себя плавающую кнопку на виджете
 */
class OverlayButtonList : public QPushButton
{
    Q_OBJECT
public:
    explicit OverlayButtonList(QWidget* parent = nullptr);

    /**
     * @brief fixPosition Переместить кнопку в правильную точку parent виджета. Выведено в public для случаев hide/show виджета (иначе багует)
     */
    void fixPosition();

    // =========================================================== //
    // =============== Настройки "головной" кнопки =============== //
    // =========================================================== //

    /**
     * @brief The ButtonOpenDirection enum Направление, в котором раскрываются кнопки меню. Можно комбинировать
     * @note   При комбинировании распределение кнопок идёт сначала на левой, потом на верхней, потом на правой, потом на левой части
     *         в направлении ОТ центральной кнопки
     */
    enum ButtonOpenDirection : int {
        Right   = 0b0001,
        Left    = 0b0010,
        Up      = 0b0100,
        Down    = 0b1000,
        All     = Right | Left | Up | Down
    };

    /**
     * @brief setOpenDirection Задать направление или направления, в которых будут открываться кнопки
     * @param direction_ Направление раскрытия. Можно совместить флаги направления через битовое ИЛИ
     */
    void setOpenDirection(ButtonOpenDirection direction_);

    /**
     * @brief setMaxButtonCount Задать максимальное количество кнопок на направлении, которое может отображаться
     * @param direction_        Направление или направления
     * @param maxButtonCount    Кол-во кнопок на направлении
     */
    void setMaxButtonCount(ButtonOpenDirection direction_, uint maxButtonCount);

    /**
     * @brief setMovable Изменить возможность пользователю двигать кнопку
     * @param isMovable При true, пользователь, удерживая ЛКМ на кнопке некоторое время, может её переместить, задав новые оффсеты
     */
    void setMovable(bool isMovable = true);

    /**
     * @brief setWidgetPadding  Задать статичный отступ виджета в parent виджете
     * @param direction_        Сторона, от которой задать отступ
     * @param offsetValue       Значение отступа. -1 для отключения отступа
     */
    void setWidgetPadding(ButtonOpenDirection direction_, int offsetValue);

    /**
     * @brief setButtonSize Задать размеры кнопок (основной и открывающихся)
     * @param bSize_ Размеры кнопок
     */
    void setButtonSize(const QSize& bSize_);

    /**
     * @brief setAnimationSpeed Изменить скорость анимации
     * @param increaser_ число не меньше 0. Скорость умножается на это число
     * @note  При неверном значении increaser_ бросает исключение std::invalid_argument
     */
    void setAnimationSpeed(double increaser_);

    /**
     * @brief setHideOnClick Включение и отключение скрытия кнопок при нажатии на одну из них
     * @param hideOnClick_ true для включения скрытия
     */
    void setHideOnClick(double hideOnClick_);


    // =========================================================== //
    // ============== Добавление и изменение кнопок ============== //
    // =========================================================== //

    /**
     * @brief The ButtonInfo class Описание кнопки в раскрывающемся меню
     */
    struct ButtonInfo
    {
        QString name;                               //! Отображаемое название
        QString tooltip;                            //! Описание кнопки в tooltip
        QString styleSheet;                         //! QSS стиль кнопки
        QIcon   icon;                               //! Иконка кнопки
        std::function<void(QPushButton*)> action;   //! Действие кнопки. Аргумент -- указатель на нажатую кнопку
    };

    /**
     * @brief addButton Добавить кнопку в список
     * @param button_ Информация о кнопке
     * @return Индекс кнопки в общем списке
     */
    uint addButton(const ButtonInfo& button_);

    /**
     * @brief updateButton Обновить информацию о кнопке. При отсутствии кнопки или неверном индексе, метод ничего не сделает
     * @param button_   Информация о кнопке
     * @param index     Индекс кнопки в общем списке
     */
    void updateButton(const ButtonInfo& button_, uint index);

    /**
     * @brief buttonCount Получить количество кнопок в общем списке
     * @return Кол-во кнопок
     */
    uint buttonCount() const;

    /**
     * @brief getDirectionButtonCount Получить кол-во кнопок по направлению (расчитывает по факту вызова)
     * @param direction_ Направление для анализа
     * @return Кол-во кнопок в данном направлении
     */
    uint getDirectionButtonCount(ButtonOpenDirection direction_);

    /**
     * @brief buttonInfo Получить информацию о кнопке по индексу
     * @param index Индекс кнопки в общем списке
     * @return      Информация о кнопке или пустой std::optional если нет кнопки с таким индексом
     */
    std::optional<ButtonInfo> buttonInfo(uint index) const;

    /**
     * @brief removeButton
     * @param buttonIndex
     */
    void removeButton(uint buttonIndex);

    /**
     * @brief getButton     Получить кнопку по индексу
     * @param buttonIndex   Индекс кнопки
     * @return              nullptr если индекс неверный
     */
    QPushButton* getButton(uint buttonIndex);

public slots:
    /**
     * @brief showButtons Отобразить кнопки ("раскрыть" виджет)
     */
    void showButtons();

    /**
     * @brief hideButtons Скрыть кнопки ("закрыть" виджет)
     */
    void hideButtons();

signals:
    /**
     * @brief stopAnimations Используется для остановки анимаций разворачивания и сворачивания кнопок
     */
    void stopAnimations();

private:
    ButtonOpenDirection                     m_openDirections {ButtonOpenDirection::Right};  //! Направления открытия кнопок
    std::map<ButtonOpenDirection, uint>     m_maxButtonCounts {                             //! Максимум кнопок по направлению
        {ButtonOpenDirection::Left,     10},
        {ButtonOpenDirection::Up,       10},
        {ButtonOpenDirection::Right,    10},
        {ButtonOpenDirection::Down,     10}
    };
    std::vector<ButtonInfo>                 m_buttonsInfo;      //! Инормация о кнопках
    std::vector<QPushButton*>               m_buttons;          //! Виджеты кнопок

    std::map<ButtonOpenDirection, int>      m_padding {         //! Отступы на виджете
        {ButtonOpenDirection::Up,       30},
        {ButtonOpenDirection::Down,     -1},
        {ButtonOpenDirection::Left,     30},
        {ButtonOpenDirection::Right,    -1}
    };

    double  m_buttonMargins         {15.0};     //! Отступы между кнопками в открытом состоянии
    double  m_animationMultiplier   {1.0};      //! Скорость анимации
    bool    m_isButtonsHidden       {true};     //! Чтобы не обращаться к вектору кнопок
    bool    m_isMovable             {false};    //! Определяет, можно ли двигать кнопку по виджету
    bool    m_hideOnClick           {true};     //! Определяет, скрывать ли кнопки по нажатии на одну из них
    QSize   m_fixedSize             {50, 50};   //! Создано из-за особенностей отрисовки в кьюте

    /**
     * @brief moveButtons Перемещает кнопки по осям в соответствии с распределением
     * @param isInverted  Направление, для скрытия true
     * @param isAnimated  Анимация, для включения true
     */
    void moveButtons(bool isInverted, bool isAnimated);

    /**
     * @brief fixButtonsPositions Для случаев, когда анимация не закончилась, а кнопки переместились
     */
    void fixButtonsPositions();

    /**
     * @brief setupButton Настроить кнопку (внутренняя функция), перезаписывает конфигурацию кнопки
     * @param pButton       Указатель на кнопку
     * @param buttonInfo    Информация о кнопке
     */
    void setupButton(QPushButton* pButton, const ButtonInfo& buttonInfo);

    /**
     * @brief setupSignals Настроить сигналы виджета
     */
    void setupSignals();

    /**
     * @brief setupWidget настроить виджет
     */
    void setupWidget();

    /**
     * @brief paintEvent Переопределение ивента отрисовки для локации кнопки на родительском виджете
     * @param e Ивент отрисовки
     */
    void paintEvent(QPaintEvent* e);

    /**
     * @brief resizeEvent Переопределение ивента ресайза из-за гениального кьюта
     * @param e Ивент ресайза
     */
    void resizeEvent(QResizeEvent* e);
};

#endif // OVERLAYBUTTONLIST_H
