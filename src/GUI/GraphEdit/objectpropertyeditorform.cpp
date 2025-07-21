#include "objectpropertyeditorform.h"
#include "ui_objectpropertyeditorform.h"

#include "logging.h"

#include "Graph/gvertex.h"

#include <QColorDialog>
#include <QFileDialog>

#include <QImageReader>

#include <QPainter>
#include <QImage>

static const auto LABEL_COLOR_PROPERTY_NAME = "labelDisplayColor";

ObjectPropertyEditorForm::ObjectPropertyEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ObjectPropertyEditorForm)
{
    ui->setupUi(this);

    connect(ui->accept_pushButton, &QPushButton::clicked,
            this, &ObjectPropertyEditorForm::acceptChanges);

    connect(ui->cancel_pushButton, &QPushButton::clicked,
            this, &ObjectPropertyEditorForm::cancelChanges);

    auto callColorDialog = [this](QLabel* pLabel){
        auto currentColorName = pLabel->property(LABEL_COLOR_PROPERTY_NAME).toString();
        auto userDefinedColor = QColorDialog::getColor(currentColorName, nullptr, "Выберите цвет");
        if (!userDefinedColor.isValid()) { // Пользователь не выбрал цвет
            return;
        }

        QImage labelImage(pLabel->width(), pLabel->height(), QImage::Format_RGBA64);
        labelImage.fill(userDefinedColor);
        QPainter p(&labelImage);

        auto negativeColor = QColor(userDefinedColor.red() > 125 ? 0 : 255,
                                    userDefinedColor.green() > 125 ? 0 : 255,
                                    userDefinedColor.blue() > 125 ? 0 : 255);
        p.setPen(negativeColor);
        p.setBrush(Qt::transparent);
        auto drawRect = labelImage.rect();
        drawRect.setWidth(drawRect.width() - 6);
        drawRect.setHeight(drawRect.height() - 6);
        drawRect.moveTo(drawRect.x() + 3, drawRect.y() + 3);
        p.drawRect(drawRect);

        auto displayColorName = QString("#%1%2%3")
                .arg(userDefinedColor.red(), 2, 16, QLatin1Char('0'))
                .arg(userDefinedColor.green(), 2, 16, QLatin1Char('0'))
                .arg(userDefinedColor.blue(), 2, 16, QLatin1Char('0'))
                .toUpper();

        p.drawText(drawRect, Qt::AlignHCenter, displayColorName);

        pLabel->setPixmap(QPixmap::fromImage(labelImage));
        pLabel->setProperty(LABEL_COLOR_PROPERTY_NAME, displayColorName);
    };

    connect(ui->selectMainColor_pushButton, &QPushButton::clicked,
            this, [callColorDialog, this](){
        callColorDialog(ui->mainColor_label);
    });

    connect(ui->selectBgrColor_pushButton, &QPushButton::clicked,
            this, [callColorDialog, this](){
        callColorDialog(ui->bgrColor_label);
    });

    connect(ui->selectSelectionColor_pushButton, &QPushButton::clicked,
            this, [callColorDialog, this](){
        callColorDialog(ui->selectedColor_label);
    });

    connect(ui->selectIcon_pushButton, &QPushButton::clicked,
            this, [this](){
        auto targetPath = QFileDialog::getOpenFileName(nullptr, "Выберите изображение", QDir::currentPath());

        if (targetPath.isEmpty()) {
            ui->iconPreview_label->setText("Предпросмотр");
            ui->iconPath_lineEdit->clear();
            return;
        }

        QImageReader imgReader;
        imgReader.setAutoDetectImageFormat(true);
        imgReader.setAutoTransform(true);
        imgReader.setDecideFormatFromContent(true);
        imgReader.setFileName(targetPath);

        if (imgReader.imageCount() == 0) {
            LOG_WARNING("Not an image:", targetPath);
            return;
        }

        ui->iconPreview_label->setPixmap(QPixmap::fromImage(imgReader.read().scaled(ui->iconPreview_label->size())));
    });

    ui->shortName_lineEdit->setMaxLength(Graph::GRAPH_MAX_SHORTNAME_SIZE);
    ui->property_tabWidget->setCurrentIndex(0);
}

ObjectPropertyEditorForm::~ObjectPropertyEditorForm()
{
    delete ui;
}

void ObjectPropertyEditorForm::setTargetItem(ObjectViewItems::ItemBase *pTargetItem)
{
    m_pTargetItem = pTargetItem;
    ui->name_lineEdit->setText(m_pTargetItem->getName());
    ui->shortName_lineEdit->setText(m_pTargetItem->getShortName());
    ui->description_plainTextEdit->setPlainText(m_pTargetItem->getDescription());

    auto pVertex = dynamic_cast<ObjectViewItems::VertexObject*>(m_pTargetItem);
    if (nullptr != pVertex) {
        ui->iconPreview_label->setPixmap(QPixmap::fromImage(pVertex->getImage()));
        if (ui->iconPreview_label->pixmap().isNull()) {
            ui->iconPreview_label->setText("Предпросмотр");
        }
    }
    ui->property_tabWidget->setTabEnabled(1, nullptr != pVertex);

    auto isConnectionEditing = pTargetItem->getType() == ObjectViewConstants::OBJECTTYPE_VERTEX_CONNECTION;
    ui->shortName_lineEdit->setEnabled(!isConnectionEditing);
    ui->description_plainTextEdit->setEnabled(!isConnectionEditing);
}

void ObjectPropertyEditorForm::acceptChanges()
{
    LOG_INFO("Changing data of object");
    m_pTargetItem->setShortName(ui->shortName_lineEdit->text());
    m_pTargetItem->setName(ui->name_lineEdit->text());
    m_pTargetItem->setDescription(ui->description_plainTextEdit->toPlainText());

    if (auto pVertex = dynamic_cast<ObjectViewItems::VertexObject*>(m_pTargetItem);
        nullptr != pVertex) {
        auto pxmap = ui->iconPreview_label->pixmap();
        pVertex->setImage(pxmap.toImage());
    }

    if (auto pConnection = dynamic_cast<ObjectViewItems::VertexConnectionLine*>(m_pTargetItem);
        nullptr != pConnection) {
        // TODO: Do something with styles
//        pConnection
    }

    emit changedItemData();
}

void ObjectPropertyEditorForm::cancelChanges()
{
    LOG_INFO("Canceled change data of object");
    setTargetItem(m_pTargetItem); // ez
    emit editCanceled();
}
