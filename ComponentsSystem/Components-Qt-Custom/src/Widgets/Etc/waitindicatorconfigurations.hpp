#pragma once

#include <QObject>
#include <QPen>
#include <QString>

namespace QtCustom::Widgets {

/**
 * @brief The IndicatorConfigurationBase class Config with basical settings of any indicator
 */
class IndicatorConfigurationBase : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    void setTitle(const QString& text);
    QString getTitle() const { return m_titleText; }

    void setDescription(const QString& text);
    QString getDescription() const { return m_descriptionText; }

    QSize getSize() const;
    void setSize(const QSize &iSize);

    bool getIsDisablingParent() const;
    void setIsDisablingParent(bool iIsDisablingParent);

    uint getTitleTextSymbolLimit() const;
    void setTitleTextSymbolLimit(uint iTitleTextSymbolLimit);

    uint getDescriptionTextSymbolLimit() const;
    void setDescriptionTextSymbolLimit(uint iDescriptionTextSymbolLimit);

    QPen getTextPen() const;
    void setTextPen(const QPen &iTextPen);

signals:
    void visibleDataChanged();

protected:
    QSize m_size {60, 60}; // Size of area to draw indicator in it

    bool    m_isDisablingParent {true};

    uint    m_titleTextSymbolLimit          {25};
    QString m_titleText                     {"Please, be patient..."};
    uint    m_descriptionTextSymbolLimit    {150};
    QString m_descriptionText               {"Operation is in process."};

    QPen m_textPen       {QPen(QColor(85, 220, 190), 1, Qt::SolidLine, Qt::RoundCap)};

};
using IndicatorConfigurationBasePtr = IndicatorConfigurationBase*;


/**
 * @brief The IndicatorCircleConfiguration class Standard loading circle configuration
 */
class IndicatorCircleConfiguration : public IndicatorConfigurationBase
{
public:
    using IndicatorConfigurationBase::IndicatorConfigurationBase;

    bool getIsPercentVisible() const;
    void setIsPercentVisible(bool iIsPercentVisible);

    bool getIsAnimationEnabled() const;
    void setIsAnimationEnabled(bool iIsAnimationEnabled);

    QPen getPrimaryPen() const;
    void setPrimaryPen(const QPen &iPrimaryPen);

    QPen getSecondaryPen() const;
    void setSecondaryPen(const QPen &iSecondaryPen);

private:
    bool    m_isPercentVisible  {true};
    bool    m_isAnimationEnabled {false};

    QPen m_primaryPen    {QPen(QColor(45, 210, 170), 4, Qt::SolidLine, Qt::RoundCap)};
    QPen m_secondaryPen  {QPen(QColor(65, 120, 110), 2, Qt::SolidLine, Qt::RoundCap)};
};


/**
 * @brief The IndicatorCircleLinedConfiguration class Circle, made of lines from center
 */
class IndicatorCircleLinedConfiguration : public IndicatorConfigurationBase
{
public:
    using IndicatorConfigurationBase::IndicatorConfigurationBase;

    uint16_t getLineCount() const;
    void setLineCount(uint16_t iLineCount);

    bool getIsPercentVisible() const;
    void setIsPercentVisible(bool iIsPercentVisible);

    bool getIsAnimationEnabled() const;
    void setIsAnimationEnabled(bool iIsAnimationEnabled);

    double getLineOffsetCoefficient() const;
    void setLineOffsetCoefficient(double iLineOffsetCoefficient);

    QPen getPrimaryPen() const;
    void setPrimaryPen(const QPen &iPrimaryPen);

    QPen getSecondaryPen() const;
    void setSecondaryPen(const QPen &iSecondaryPen);

private:
    bool    m_isPercentVisible  {true};
    bool    m_isAnimationEnabled {false};

    uint16_t m_lineCount {36};
    double   m_lineOffsetCoefficient {0.85}; // (invisibleLineLength / lineLength)

    QPen m_primaryPen    {QPen(QColor(45, 210, 170), 4, Qt::SolidLine, Qt::RoundCap)};
    QPen m_secondaryPen  {QPen(QColor(65, 120, 110), 2, Qt::SolidLine, Qt::RoundCap)};
};

}