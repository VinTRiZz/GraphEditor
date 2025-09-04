#include "objectpropertyeditorform.h"

#include <Common/CommonFunctions.h>
#include <Common/DirectoryManager.h>
#include <Common/Encryption.h>
#include <Common/Logging.h>
#include <GraphObject/Components.h>

#include <QBuffer>
#include <QFileDialog>
#include <QVariant>

#include "Items/connectionlineitem.h"
#include "Items/vertexobjectitem.h"
#include "ui_objectpropertyeditorform.h"

using namespace CommonFunctions;

ObjectPropertyEditorForm::ObjectPropertyEditorForm(QWidget* parent)
    : QWidget(parent), ui(new Ui::ObjectPropertyEditorForm) {
    ui->setupUi(this);

    initGalery();
    initIcons();

    connect(ui->accept_pushButton, &QPushButton::clicked, this,
            &ObjectPropertyEditorForm::acceptChanges);

    connect(ui->cancel_pushButton, &QPushButton::clicked, this,
            &ObjectPropertyEditorForm::cancelChanges);

    connectColorDialog(ui->selectMainColor_pushButton, ui->mainColor_label);
    connectColorDialog(ui->selectBgrColor_pushButton, ui->bgrColor_label);
    connectColorDialog(ui->selectSelectionColor_pushButton,
                       ui->selectedColor_label);

    ui->shortName_lineEdit->setMaxLength(Graph::GRAPH_MAX_SHORTNAME_SIZE);
    ui->property_tabWidget->setCurrentIndex(0);
}

ObjectPropertyEditorForm::~ObjectPropertyEditorForm() {
    delete ui;
}

void ObjectPropertyEditorForm::setTargetItem(
    ObjectViewItems::ItemBase* pTargetItem) {
    m_pTargetItem = pTargetItem;
    ui->name_lineEdit->setText(m_pTargetItem->toolTip());
    ui->shortName_lineEdit->setText(m_pTargetItem->getDisplayName());
    ui->description_plainTextEdit->setPlainText(
        m_pTargetItem->getDescription());

    setColor(ui->mainColor_label, pTargetItem->getBorderColor());
    setColor(ui->bgrColor_label, pTargetItem->getBackgroundColor());
    setColor(ui->selectedColor_label, pTargetItem->getSelectionColor());

    auto pVertex = dynamic_cast<ObjectViewItems::VertexObject*>(m_pTargetItem);

    // TODO: Setup everything
    //  if (nullptr != pVertex) {
    //    ui->iconPreview_label->setPixmap(QPixmap::fromImage(pVertex->getImage()));
    //    if (ui->iconPreview_label->pixmap(Qt::ReturnByValue).isNull()) {
    //      ui->iconPreview_label->setText("Предпросмотр");
    //    }
    //    auto imageRect = pVertex->getImageRect();
    //    ui->iconPreview_label->setFixedSize(
    //        QSize(imageRect.width(), imageRect.height()));
    //  }
    ui->property_tabWidget->setTabEnabled(1, nullptr != pVertex);

    auto isConnectionEditing =
        pTargetItem->getType() ==
        ObjectViewConstants::OBJECTTYPE_VERTEX_CONNECTION;
    ui->name_lineEdit->setEnabled(!isConnectionEditing);
    ui->description_plainTextEdit->setEnabled(!isConnectionEditing);
}

void ObjectPropertyEditorForm::acceptChanges() {
    LOG_INFO("Changing data of object");
    m_pTargetItem->setDisplayName(ui->shortName_lineEdit->text());
    m_pTargetItem->setToolTip(ui->name_lineEdit->text());
    m_pTargetItem->setDescription(ui->description_plainTextEdit->toPlainText());

    m_pTargetItem->setBorderColor(getColor(ui->mainColor_label));
    m_pTargetItem->setBackgroundColor(getColor(ui->bgrColor_label));
    m_pTargetItem->setSelectionColor(getColor(ui->selectedColor_label));

    // TODO: Setup everything
    //  if (auto pVertex =
    //          dynamic_cast<ObjectViewItems::VertexObject *>(m_pTargetItem);
    //      nullptr != pVertex) {
    //    auto pxmap = ui->iconPreview_label->pixmap(Qt::ReturnByValue);
    //    pVertex->setImage(pxmap.toImage());
    //  }

    if (auto pConnection =
            dynamic_cast<ObjectViewItems::VertexConnectionLine*>(m_pTargetItem);
        nullptr != pConnection) {
        // TODO: Do something with styles
        //        pConnection
    }

    emit changedItemData();
}

void ObjectPropertyEditorForm::cancelChanges() {
    LOG_INFO("Canceled change data of object");
    setTargetItem(m_pTargetItem);  // ez
    emit editCanceled();
}

void ObjectPropertyEditorForm::initGalery() {
    ui->imageHistoryGalery->init();
    ui->imageHistoryGalery->setSelectionColor(QColor(161, 209, 207));

    connect(ui->selectIcon_pushButton, &QPushButton::clicked, this, [this]() {
        auto targetPath = QFileDialog::getOpenFileName(
            nullptr, "Выберите изображение",
            DirectoryManager::getDirectoryPath(
                DirectoryManager::DirectoryType::Documents));

        QFile targetFile(targetPath);
        targetFile.open(QIODevice::ReadOnly);
        auto pxmapHash = Encryption::sha256(targetFile.readAll());
        auto hashExist = ui->imageHistoryGalery->containWidget(
            [&pxmapHash](QWidget* pLabel) {
                return (pxmapHash == pLabel->property("imghash").toByteArray());
            });
        if (hashExist) {
            LOG_INFO("Image exist, nothing to add into galery");
            return;
        }

        auto img = imageFromPath(targetPath);
        auto pxmap =
            QPixmap::fromImage(img.scaled(250, 250, Qt::IgnoreAspectRatio));
        auto pLabel = new QLabel;
        pLabel->setProperty("imghash", pxmapHash);
        pLabel->setToolTip(targetPath);
        pLabel->setPixmap(pxmap);
        ui->imageHistoryGalery->addWidget(pLabel,
                                          QFileInfo(targetPath).baseName());
    });
}

void ObjectPropertyEditorForm::initIcons() {
    ui->iconGalery->init();
    ui->iconGalery->setSelectionColor(QColor(161, 209, 207));

    // Добавляем элементы из ресурсов
    auto addIcon = [this](const QString& iconPath, const QString& iconName) {
        const QString iconBasepath = ":/common/images/vertexicons/%0";
        auto img = imageFromPath(iconBasepath.arg(iconPath));
        auto pxmap =
            QPixmap::fromImage(img.scaled(70, 70, Qt::IgnoreAspectRatio));
        auto pLabel = new QLabel;
        pLabel->setFixedSize(70, 70);
        pLabel->setPixmap(pxmap);
        pLabel->setToolTip(iconName);
        ui->iconGalery->addWidget(pLabel, iconName);
    };

    addIcon("person/green.svg", "Человек (З)");
    addIcon("person/red.svg", "Человек (К)");
}
