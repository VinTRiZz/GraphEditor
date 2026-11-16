#include "waitindicatorconfigurations.hpp"

namespace QtCustom::Widgets {

void IndicatorConfigurationBase::setTitle(const QString &text)
{
    if (text.size() > m_titleTextSymbolLimit) {
        m_titleText = text.left(m_titleTextSymbolLimit);
        emit visibleDataChanged();
        return;
    }
    m_titleText = text;
    emit visibleDataChanged();
}

void IndicatorConfigurationBase::setDescription(const QString &text)
{
    if (text.size() > m_descriptionTextSymbolLimit) {
        m_descriptionText = text.left(m_descriptionTextSymbolLimit);
        emit visibleDataChanged();
        return;
    }
    m_descriptionText = text;
    emit visibleDataChanged();
}

QSize IndicatorConfigurationBase::getSize() const
{
    return m_size;
}

void IndicatorConfigurationBase::setSize(const QSize &iSize)
{
    m_size = iSize;
    emit visibleDataChanged();
}

bool IndicatorConfigurationBase::getIsDisablingParent() const
{
    return m_isDisablingParent;
}

void IndicatorConfigurationBase::setIsDisablingParent(bool iIsDisablingParent)
{
    m_isDisablingParent = iIsDisablingParent;
    emit visibleDataChanged();
}

uint IndicatorConfigurationBase::getTitleTextSymbolLimit() const
{
    return m_titleTextSymbolLimit;
}

void IndicatorConfigurationBase::setTitleTextSymbolLimit(uint iTitleTextSymbolLimit)
{
    m_titleTextSymbolLimit = iTitleTextSymbolLimit;
    emit visibleDataChanged();
}

uint IndicatorConfigurationBase::getDescriptionTextSymbolLimit() const
{
    return m_descriptionTextSymbolLimit;
}

void IndicatorConfigurationBase::setDescriptionTextSymbolLimit(uint iDescriptionTextSymbolLimit)
{
    m_descriptionTextSymbolLimit = iDescriptionTextSymbolLimit;
    emit visibleDataChanged();
}

QPen IndicatorConfigurationBase::getTextPen() const
{
    return m_textPen;
}

void IndicatorConfigurationBase::setTextPen(const QPen &iTextPen)
{
    m_textPen = iTextPen;
    emit visibleDataChanged();
}

uint16_t IndicatorCircleLinedConfiguration::getLineCount() const
{
    return m_lineCount;
}

void IndicatorCircleLinedConfiguration::setLineCount(uint16_t iLineCount)
{
    m_lineCount = iLineCount;
    emit visibleDataChanged();
}

bool IndicatorCircleLinedConfiguration::getIsPercentVisible() const
{
    return m_isPercentVisible;
}

void IndicatorCircleLinedConfiguration::setIsPercentVisible(bool iIsPercentVisible)
{
    m_isPercentVisible = iIsPercentVisible;
    emit visibleDataChanged();
}

bool IndicatorCircleLinedConfiguration::getIsAnimationEnabled() const
{
    return m_isAnimationEnabled;
}

void IndicatorCircleLinedConfiguration::setIsAnimationEnabled(bool iIsAnimationEnabled)
{
    m_isAnimationEnabled = iIsAnimationEnabled;
    emit visibleDataChanged();
}

double IndicatorCircleLinedConfiguration::getLineOffsetCoefficient() const
{
    return m_lineOffsetCoefficient;
}

void IndicatorCircleLinedConfiguration::setLineOffsetCoefficient(double iLineOffsetCoefficient)
{
    m_lineOffsetCoefficient = iLineOffsetCoefficient;
    emit visibleDataChanged();
}

QPen IndicatorCircleLinedConfiguration::getPrimaryPen() const
{
    return m_primaryPen;
}

void IndicatorCircleLinedConfiguration::setPrimaryPen(const QPen &iPrimaryPen)
{
    m_primaryPen = iPrimaryPen;
    emit visibleDataChanged();
}

QPen IndicatorCircleLinedConfiguration::getSecondaryPen() const
{
    return m_secondaryPen;
}

void IndicatorCircleLinedConfiguration::setSecondaryPen(const QPen &iSecondaryPen)
{
    m_secondaryPen = iSecondaryPen;
    emit visibleDataChanged();
}

bool IndicatorCircleConfiguration::getIsPercentVisible() const
{
    return m_isPercentVisible;
}

void IndicatorCircleConfiguration::setIsPercentVisible(bool iIsPercentVisible)
{
    m_isPercentVisible = iIsPercentVisible;
    emit visibleDataChanged();
}

bool IndicatorCircleConfiguration::getIsAnimationEnabled() const
{
    return m_isAnimationEnabled;
}

void IndicatorCircleConfiguration::setIsAnimationEnabled(bool iIsAnimationEnabled)
{
    m_isAnimationEnabled = iIsAnimationEnabled;
    emit visibleDataChanged();
}

QPen IndicatorCircleConfiguration::getPrimaryPen() const
{
    return m_primaryPen;
}

void IndicatorCircleConfiguration::setPrimaryPen(const QPen &iPrimaryPen)
{
    m_primaryPen = iPrimaryPen;
    emit visibleDataChanged();
}

QPen IndicatorCircleConfiguration::getSecondaryPen() const
{
    return m_secondaryPen;
}

void IndicatorCircleConfiguration::setSecondaryPen(const QPen &iSecondaryPen)
{
    m_secondaryPen = iSecondaryPen;
    emit visibleDataChanged();
}

}
