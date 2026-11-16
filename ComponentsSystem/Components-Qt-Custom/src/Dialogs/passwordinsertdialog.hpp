#pragma once

#include <QDialog>

namespace Ui {
class PasswordInsertDialog;
}

namespace QtCustom::Dialogs {

/**
 * @brief The PasswordInsertDialog class Форма ввода пароля
 */
class PasswordInsertDialog : public QDialog {
    Q_OBJECT

public:
    explicit PasswordInsertDialog(QWidget* parent = nullptr);
    ~PasswordInsertDialog();

    /**
     * @brief getPassword   Получить пароль после ввода
     * @return              NULL QString, если пароль не был введён или
     * предыдущий введённый пароль
     */
    QString getPassword() const;

private:
    Ui::PasswordInsertDialog* ui;
};

}