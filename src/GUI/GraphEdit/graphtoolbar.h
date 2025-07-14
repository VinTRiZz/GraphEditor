#ifndef GRAPHTOOLBAR_H
#define GRAPHTOOLBAR_H

#include "GUI/CustomWidgets/buttontoolbar.h"

class GraphToolbar : public ButtonToolbar::HeadWidget
{
    Q_OBJECT
public:
    explicit GraphToolbar(QWidget* parent = nullptr);

    void init();

signals:
    void showProperties();
    void hideProperties();

    void saveGraph(const QString& sPath);
    void loadGraph(const QString& lPath);
};

#endif // GRAPHTOOLBAR_H
