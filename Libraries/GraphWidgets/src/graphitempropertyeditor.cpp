#include "graphitempropertyeditor.hpp"
#include "ui_graphitempropertyeditor.h"

GraphItemPropertyEditor::GraphItemPropertyEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphItemPropertyEditor)
{
    ui->setupUi(this);
}

GraphItemPropertyEditor::~GraphItemPropertyEditor()
{
    delete ui;
}
