#ifndef BUTTONMATRIX_H
#define BUTTONMATRIX_H

#include <QPushButton>
#include <boost/multi_array.hpp>
#include <optional>

namespace ButtonMatrix
{

/**
 * @brief The ButtonConfig class Конфигурация кнопки
 */
struct ButtonConfig
{
    uint    buttonId {};

    int     positionX {};   //! Координата X в матрице кнопки
    int     positionY {};   //! Координата Y в матрице кнопки

    QString name;       //! Отображаемое название
    QString tooltip;    //! Описание кнопки в tooltip
    QString styleSheet; //! QSS стиль кнопки
    QIcon   icon;       //! Иконка кнопки
    QIcon   secondIcon; //! Иконка кнопки для второго состояния (disabled, activated и т.д.). Не используется матрицей, сугубо для удобства

    bool    isEnabled   {true}; //! Изначальное состояние

    std::function<void(QPushButton*)> action;   //! Действие кнопки. Аргумент -- указатель на нажатую кнопку
};


/**
 * @brief The HeadButton class "Главная" кнопка, которая обрабатывает все внутренние. Нельзя скрыть (вынужденная мера)
 */
class HeadButton : public QPushButton
{
    Q_OBJECT

    using QPushButton::setFixedHeight;
    using QPushButton::setFixedWidth;
    using QPushButton::setFixedSize;

public:
    HeadButton(QWidget* parent = nullptr);
    ~HeadButton();

    /**
     * @brief fixPositions Позволяет избежать багов при переводе в полноэкранный режим и обратно
     */
    void fixPositions();

    /**
     * @brief addButton Добавление кнопки
     * @param bConfig   Конфигурация кнопки
     * @return          false в случае, если координаты заняты или недоступны
     */
    bool addButton(const ButtonConfig& bConfig);

    /**
     * @brief getButtonConfig   Получение конфигурации кнопки
     * @param buttonX           X кнопки
     * @param buttonY           Y кнопки
     * @return                  Конфигурация кнопки
     */
    std::optional<ButtonConfig> getButtonConfig(int buttonX, int buttonY);

    /**
     * @brief updateButton  Обновить кнопку. ID должен быть задан
     * @param bConfig       Конфигурация кнопки
     * @return              false если кнопка не была найдена
     */
    bool updateButton(const ButtonConfig& bConfig);

    /**
     * @brief removeButton  Удаление кнопки
     * @param buttonX       X кнопки
     * @param buttonY       Y кнопки
     */
    void removeButton(int buttonX, int buttonY);

    /**
     * @brief setButtonMatrix Указать матрицу, в которой можно добавлять кнопку. ВНИМАНИЕ! Удаляет предыдущие кнопки
     * @note  x и y считаются относительно положения кнопки, причём кнопка в точке 0,0
     * @param xMin  X minimal
     * @param yMin  Y minimal
     * @param xMax  X maximal
     * @param yMax  Y maximal
     * @param useDimensions Распологать по осям при true и в матрице NxM при false
     */
    void setButtonMatrix(int xMin, int yMin, int xMax, int yMax);

    /**
     * @brief setButtonPadding  Задать отступы на виджете
     * @param left
     * @param right
     * @param top
     * @param bottom
     */
    void setButtonPadding(unsigned left, unsigned right, unsigned top, unsigned bottom);

    /**
     * @brief setIcons      Задать иконки кнопке
     * @param collapsedIcon Иконка при скрытых кнопках
     * @param expandedIcon  Иконка при раскрытых кнопках
     */
    void setIcons(const QIcon& collapsedIcon, const QIcon& expandedIcon);

    /**
     * @brief setAnimationSpeed     Задать скорость анимации
     * @param animationMultiplier   Мультипликатор анимации
     */
    void setAnimationSpeed(double animationMultiplier);

    /**
     * @brief setButtonMargin   Задать длину отступов между кнопками
     * @param buttonMargin      Длина отступов
     */
    void setButtonMargin(double buttonMargins);

    /**
     * @brief setButtonsSize    Задать размеры кнопки (фиксированные)
     * @param iSize             Размеры
     */
    void setButtonsSize(const QSize& iSize);

    /**
     * @brief getButton Получить кнопку по координатам
     * @param x
     * @param y
     * @return Кнопка или nullptr если таких координат нет (например, ошибка маппинга)
     */
    QPushButton* getButton(int x, int y);

    /**
     * @brief getAllButtons Получить список всех конфигураций кнопок
     * @return              Список кнопок слева направо сверху вниз
     */
    std::list<ButtonConfig> getAllButtons() const;

public slots:
    /**
     * @brief expand Раскрыть кнопки
     */
    void expand(bool isAnimated);

    /**
     * @brief collapse Скрыть кнопки
     */
    void collapse(bool isAnimated);

signals:
    /**
     * @brief stopAnimations Призыв к остановке анимаций раскрытия / скрытия кнопок
     */
    void stopAnimations();

private:
    struct ButtonConfigProxy
    {
        ButtonConfig conf;
        QPushButton* pButton {nullptr};  //! Кнопка
    };

    //! Состояние
    bool m_isButtonsExpanded    {false};

    //! Для маппинга
    std::pair<int, int> m_buttonMatrixMinimum;

    //! Матрица NxM для кнопок
    boost::multi_array<ButtonConfigProxy, 2> m_buttonMatrix;

    //! Положение на родительском виджете
    int m_paddingLeft      {};
    int m_paddingRight     {};
    int m_paddingTop       {};
    int m_paddingBottom    {};

    double m_buttonMargins          {15.0}; //! Отступы между кнопками в открытом состоянии
    double m_animationMultiplier    {1.0};  //! Скорость анимации

    QSize m_fixedSize {50, 50};

    QIcon m_expandedIcon;   //! Иконка открытой кнопки
    QIcon m_collapsedIcon;  //! Иконка закрытой кнопки

    void setupSignals();

    bool isValid(int xpos, int ypos);
    int  mapToMatrixX(int xpos);
    int  mapToMatrixY(int ypos);
    std::pair<int, int> mapToMatrix(int xpos, int ypos);
    std::pair<int, int> mapFromMatrix(int xpos, int ypos);

    void moveSelf();
    void moveButton(int xpos, int ypos, bool isAnimated, bool hideOnFinish);
    void setupButton(QPushButton* pButton, const ButtonConfig& buttonInfo);

    void paintEvent(QPaintEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void moveEvent(QMoveEvent *event) override;
};

}

#endif // BUTTONMATRIX_H
