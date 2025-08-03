#include "commonfunctions.h"

#include <QColorDialog>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QPainter>
#include <QPropertyAnimation>

#include "logging.h"

namespace CommonFunctions {

void showAnimatedVertical(QWidget* pTarget, int maxHeight, int timeMs,
                          const std::function<void(void)>& animationCallback) {
    pTarget->show();

    QPropertyAnimation* animation =
        new QPropertyAnimation(pTarget, "minimumHeight");
    animation->setDuration(150);

    pTarget->setMaximumHeight(maxHeight);
    pTarget->setMinimumHeight(0);
    pTarget->setFixedHeight(0);
    animation->setStartValue(0);
    animation->setEndValue(maxHeight);

    if (animationCallback) {
        animation->connect(animation, &QPropertyAnimation::finished,
                           [=]() { animationCallback(); });
    }
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void hideAnimatedVertical(QWidget* pTarget, int maxHeight, int timeMs) {
    QPropertyAnimation* animation =
        new QPropertyAnimation(pTarget, "maximumHeight");
    animation->setDuration(150);

    pTarget->setMaximumHeight(maxHeight);
    pTarget->setMinimumHeight(0);
    pTarget->setFixedHeight(0);
    animation->setStartValue(maxHeight);
    animation->setEndValue(0);

    pTarget->connect(animation, &QPropertyAnimation::finished, pTarget,
                     &QWidget::hide);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void showAnimatedHorizontal(QWidget* pTarget, int maxWidth, int timeMs) {
    pTarget->show();

    QPropertyAnimation* animation =
        new QPropertyAnimation(pTarget, "minimumWidth");
    animation->setDuration(150);

    pTarget->setMaximumWidth(maxWidth);
    pTarget->setMinimumWidth(0);
    pTarget->setFixedWidth(0);
    animation->setStartValue(0);
    animation->setEndValue(maxWidth);

    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void hideAnimatedHorizontal(QWidget* pTarget, int maxWidth, int timeMs) {
    QPropertyAnimation* animation =
        new QPropertyAnimation(pTarget, "maximumWidth");
    animation->setDuration(150);

    pTarget->setMaximumWidth(maxWidth);
    pTarget->setMinimumWidth(0);
    pTarget->setFixedWidth(0);
    animation->setStartValue(maxWidth);
    animation->setEndValue(0);

    pTarget->connect(animation, &QPropertyAnimation::finished, pTarget,
                     &QWidget::hide);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

static const auto LABEL_COLOR_PROPERTY_NAME = "labelDisplayColor";
void connectColorDialog(QPushButton* pButton, QLabel* pTargetLabel) {
    auto callColorDialog = [](QLabel* pLabel) {
        auto currentColorName =
            pLabel->property(LABEL_COLOR_PROPERTY_NAME).toString();
        auto userDefinedColor =
            QColorDialog::getColor(currentColorName, nullptr, "Выберите цвет");
        if (!userDefinedColor.isValid()) {  // Пользователь не выбрал цвет
            return;
        }

        setColor(pLabel, userDefinedColor);
    };

    pButton->connect(
        pButton, &QPushButton::clicked, pTargetLabel,
        [callColorDialog, pTargetLabel]() { callColorDialog(pTargetLabel); });
}

void setColor(QLabel* pLabel, const QColor& color) {
    QImage labelImage(pLabel->width(), pLabel->height(), QImage::Format_RGBA64);
    labelImage.fill(color);
    QPainter p(&labelImage);

    auto negativeColor =
        QColor(color.red() > 125 ? 0 : 255, color.green() > 125 ? 0 : 255,
               color.blue() > 125 ? 0 : 255);
    p.setPen(negativeColor);
    p.setBrush(Qt::transparent);
    auto drawRect = labelImage.rect();
    drawRect.setWidth(drawRect.width() - 6);
    drawRect.setHeight(drawRect.height() - 6);
    drawRect.moveTo(drawRect.x() + 3, drawRect.y() + 3);
    p.drawRect(drawRect);

    auto displayColorName = QString("#%1%2%3")
                                .arg(color.red(), 2, 16, QLatin1Char('0'))
                                .arg(color.green(), 2, 16, QLatin1Char('0'))
                                .arg(color.blue(), 2, 16, QLatin1Char('0'))
                                .toUpper();

    p.drawText(drawRect, Qt::AlignHCenter, displayColorName);

    pLabel->setPixmap(QPixmap::fromImage(labelImage));
    pLabel->setProperty(LABEL_COLOR_PROPERTY_NAME, displayColorName);
}

QColor getColor(QLabel* pLabel) {
    return QColor(pLabel->property(LABEL_COLOR_PROPERTY_NAME).toString());
}

QPixmap pixmapFromPath(const QString& targetPath, const QSize& scaleSize) {
    QImageReader imgReader;
    imgReader.setAutoDetectImageFormat(true);
    imgReader.setAutoTransform(true);
    imgReader.setDecideFormatFromContent(true);
    imgReader.setFileName(targetPath);

    if (imgReader.imageCount() == 0) {
        LOG_WARNING("Not an image:", targetPath);
        return {};
    }

    return QPixmap::fromImage(imgReader.read().scaled(scaleSize));
}

}  // namespace CommonFunctions
