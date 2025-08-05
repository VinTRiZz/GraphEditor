#include "objectpropertyeditorform.h"

#include <Common/CommonFunctions.h>
#include <Common/Logging.h>
#include <GraphObject/Components.h>

#include <QFileDialog>
#include <QVariant>

#include "Items/connectionlineitem.h"
#include "Items/vertexobjectitem.h"
#include "ui_objectpropertyeditorform.h"
using namespace CommonFunctions;

ObjectPropertyEditorForm::ObjectPropertyEditorForm(QWidget* parent)
    : QWidget(parent), ui(new Ui::ObjectPropertyEditorForm) {
    ui->setupUi(this);

    connect(ui->accept_pushButton, &QPushButton::clicked, this,
            &ObjectPropertyEditorForm::acceptChanges);

    connect(ui->cancel_pushButton, &QPushButton::clicked, this,
            &ObjectPropertyEditorForm::cancelChanges);

    connectColorDialog(ui->selectMainColor_pushButton, ui->mainColor_label);
    connectColorDialog(ui->selectBgrColor_pushButton, ui->bgrColor_label);
    connectColorDialog(ui->selectSelectionColor_pushButton,
                       ui->selectedColor_label);

    connect(ui->selectIcon_pushButton, &QPushButton::clicked, this, [this]() {
        auto targetPath = QFileDialog::getOpenFileName(
            nullptr, "Выберите изображение", QDir::currentPath());

        if (targetPath.isEmpty()) {
            ui->iconPreview_label->setText("Предпросмотр");
            ui->iconPath_lineEdit->clear();
            return;
        }

        ui->iconPreview_label->setPixmap(
            pixmapFromPath(targetPath, ui->iconPreview_label->size()));
        ui->iconPath_lineEdit->setText(targetPath);
    });

    connect(ui->iconPath_lineEdit, &QLineEdit::textChanged, this,
            [this](const QString& changedText) {
                if (!QFileInfo(changedText).isFile()) {
                    return;
                }
                ui->iconPreview_label->setPixmap(
                    pixmapFromPath(changedText, ui->iconPreview_label->size()));
            });

    ui->shortName_lineEdit->setMaxLength(Graph::GRAPH_MAX_SHORTNAME_SIZE);
    ui->property_tabWidget->setCurrentIndex(0);
}

ObjectPropertyEditorForm::~ObjectPropertyEditorForm() {
    delete ui;
}

void ObjectPropertyEditorForm::setTargetItem(
    ObjectViewItems::ItemBase* pTargetItem) {
    m_pTargetItem = pTargetItem;
    ui->name_lineEdit->setText(m_pTargetItem->getName());
    ui->shortName_lineEdit->setText(m_pTargetItem->getShortName());
    ui->description_plainTextEdit->setPlainText(
        m_pTargetItem->getDescription());

    setColor(ui->mainColor_label, pTargetItem->getMainColor());
    setColor(ui->bgrColor_label, pTargetItem->getSecondColor());
    setColor(ui->selectedColor_label, pTargetItem->getSelectedColor());

    auto pVertex = dynamic_cast<ObjectViewItems::VertexObject*>(m_pTargetItem);
    if (nullptr != pVertex) {
        ui->iconPreview_label->setPixmap(
            QPixmap::fromImage(pVertex->getImage()));
        if (ui->iconPreview_label->pixmap(Qt::ReturnByValue).isNull()) {
            ui->iconPreview_label->setText("Предпросмотр");
        }
        auto imageRect = pVertex->getImageRect();
        ui->iconPreview_label->setFixedSize(
            QSize(imageRect.width(), imageRect.height()));
    }
    ui->property_tabWidget->setTabEnabled(1, nullptr != pVertex);

    auto isConnectionEditing =
        pTargetItem->getType() ==
        ObjectViewConstants::OBJECTTYPE_VERTEX_CONNECTION;
    ui->name_lineEdit->setEnabled(!isConnectionEditing);
    ui->description_plainTextEdit->setEnabled(!isConnectionEditing);
}

void ObjectPropertyEditorForm::acceptChanges() {
    LOG_INFO("Changing data of object");
    m_pTargetItem->setShortName(ui->shortName_lineEdit->text());
    m_pTargetItem->setName(ui->name_lineEdit->text());
    m_pTargetItem->setDescription(ui->description_plainTextEdit->toPlainText());

    m_pTargetItem->setMainColor(getColor(ui->mainColor_label));
    m_pTargetItem->setSecondColor(getColor(ui->bgrColor_label));
    m_pTargetItem->setSelectedColor(getColor(ui->selectedColor_label));

    if (auto pVertex =
            dynamic_cast<ObjectViewItems::VertexObject*>(m_pTargetItem);
        nullptr != pVertex) {
        auto pxmap = ui->iconPreview_label->pixmap(Qt::ReturnByValue);
        pVertex->setImage(pxmap.toImage());
    }

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
