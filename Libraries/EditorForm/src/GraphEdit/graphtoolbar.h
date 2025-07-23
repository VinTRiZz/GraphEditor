#ifndef GRAPHTOOLBAR_H
#define GRAPHTOOLBAR_H

#include <CustomWidgets/ButtonToolbar.h>

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

    void changeMode();
};

#endif // GRAPHTOOLBAR_H
