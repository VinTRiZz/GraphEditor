#include "objectpropertyeditorform.h"
#include "ui_objectpropertyeditorform.h"

#include "logging.h"

#include "Graph/gvertex.h"

ObjectPropertyEditorForm::ObjectPropertyEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ObjectPropertyEditorForm)
{
    ui->setupUi(this);

    connect(ui->accept_pushButton, &QPushButton::clicked,
            this, &ObjectPropertyEditorForm::acceptChanges);

    connect(ui->cancel_pushButton, &QPushButton::clicked,
            this, &ObjectPropertyEditorForm::cancelChanges);

    ui->shortName_lineEdit->setMaxLength(Graph::GRAPH_MAX_SHORTNAME_SIZE);
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
    emit changedItemData();
}

void ObjectPropertyEditorForm::cancelChanges()
{
    LOG_INFO("Canceled change data of object");
    setTargetItem(m_pTargetItem); // ez
    emit editCanceled();
}
