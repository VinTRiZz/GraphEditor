#ifndef WAITINDICATORDIALOG_H
#define WAITINDICATORDIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class WaitIndicatorDialog;
}

class WaitIndicatorDialog : public QDialog {
    Q_OBJECT
public:
    explicit WaitIndicatorDialog(QWidget* parent = nullptr);
    ~WaitIndicatorDialog();

    static WaitIndicatorDialog& getInstance();

    void requestShow();
    void requestHide(int timeoutMs = 2000);
    void pollHide();

    int getCurrentPercent() const;

    void setTitle(const QString& txt);
    QString getTitle() const;

    void setDescription(const QString& txt);
    QString getDescription() const;
    void setDescriptionHidden(bool isHiddn = true);

    void setButtonsEnabled(bool isButtnsEnabled);
    bool getIsButtonsEnabled() const;

    void setProgressBarEnabled(bool isBarEnabled);
    bool getIsProgressBarEnabled() const;

public slots:
    void setPercent(int perc);

private:
    Ui::WaitIndicatorDialog* ui;

    QTimer m_deadTimer;
    QTimer m_hideTimer;
};

#endif  // WAITINDICATORDIALOG_H
