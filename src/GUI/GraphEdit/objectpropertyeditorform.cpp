#include "objectpropertyeditorform.h"
#include "ui_objectpropertyeditorform.h"

ObjectPropertyEditorForm::ObjectPropertyEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ObjectPropertyEditorForm)
{
    ui->setupUi(this);
}

ObjectPropertyEditorForm::~ObjectPropertyEditorForm()
{
    delete ui;
}

void ObjectPropertyEditorForm::setTargetItem(ObjectViewItems::ItemBase *pTargetItem)
{
    m_pTargetItem = pTargetItem;
}
