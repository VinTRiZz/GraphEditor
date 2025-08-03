#ifndef GRAPHOPTIONSTOOLBAR_H
#define GRAPHOPTIONSTOOLBAR_H

#include <CustomWidgets/ButtonToolbar.h>

class GraphOptionsToolbar : public ButtonToolbar::HeadWidget {
    Q_OBJECT
public:
    explicit GraphOptionsToolbar(QWidget* parent = nullptr);

signals:
    void openSettings();

private:
};

#endif  // GRAPHOPTIONSTOOLBAR_H
