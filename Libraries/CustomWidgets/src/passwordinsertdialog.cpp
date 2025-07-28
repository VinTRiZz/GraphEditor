#include "passwordinsertdialog.h"
#include "ui_passwordinsertdialog.h"

#include <QIcon>

PasswordInsertDialog::PasswordInsertDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordInsertDialog)
{
    ui->setupUi(this);
    ui->password_lineEdit->setEchoMode(QLineEdit::Password);

    setWindowTitle("Редактор графов: Окно ввода пароля");
    setWindowIcon(QIcon(":/common/images/icons/app/grapheditor.svg"));
}

PasswordInsertDialog::~PasswordInsertDialog()
{
    delete ui;
}

QString PasswordInsertDialog::getPassword() const
{
    return ui->password_lineEdit->text();
}
