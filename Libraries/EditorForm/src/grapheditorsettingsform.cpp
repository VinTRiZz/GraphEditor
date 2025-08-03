#include "grapheditorsettingsform.h"
#include "ui_grapheditorsettingsform.h"

GraphEditorSettingsForm::GraphEditorSettingsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphEditorSettingsForm)
{
    ui->setupUi(this);
}

GraphEditorSettingsForm::~GraphEditorSettingsForm()
{
    delete ui;
}
