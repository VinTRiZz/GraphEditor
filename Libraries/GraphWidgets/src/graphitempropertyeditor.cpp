#include "graphitempropertyeditor.hpp"
#include "ui_graphitempropertyeditor.h"

#include <PluginModule/PluginMaster.h>
#include <PluginModule/PluginWidgets.h>
#include <PluginCoreInterface/Core.h>

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

void GraphItemPropertyEditor::setTargetItem(Graph::PluginObjectItnterface *pTarget)
{
    auto& pMaster = Graph::PluginMaster::getInstance();
    auto plugin = pMaster.getPlugin(pTarget->getPluginName());
    auto objectEditor = plugin->getPluginCore()->getPropertyEditor(pTarget);

    // Удаление на стороне плагина
    [[maybe_unused]] auto prevEditor = ui->editorPage->layout()->takeAt(0);
    ui->editorPage->layout()->addWidget(objectEditor);
}
