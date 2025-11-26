#pragma once

#include <QListView>

class PluginItemListView : public QListView
{
public:
    explicit PluginItemListView(QWidget* parent = nullptr);

    void setPluginName(const QString& pluginName);

protected:
    void startDrag(Qt::DropActions supportedActions) override;

private:
    QString m_pluginName;
};

