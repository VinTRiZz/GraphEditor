#ifndef PASSWORDINSERTDIALOG_H
#define PASSWORDINSERTDIALOG_H

#include <QDialog>

namespace Ui {
class PasswordInsertDialog;
}

class PasswordInsertDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordInsertDialog(QWidget *parent = nullptr);
    ~PasswordInsertDialog();

    QString getPassword() const;

private:
    Ui::PasswordInsertDialog *ui;
};

#endif // PASSWORDINSERTDIALOG_H
