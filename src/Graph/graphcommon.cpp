#include "graphcommon.h"

#include <QMessageBox>

namespace GraphCommon
{

void showInfo(const QString &text)
{
    QMessageBox::information(nullptr, "Информация", text, QMessageBox::StandardButton::Close);
}

void showWarning(const QString &text)
{
    QMessageBox::warning(nullptr, "Внимание", text, QMessageBox::StandardButton::Close);
}

void showError(const QString &text)
{
    QMessageBox::critical(nullptr, "Ошибка", text, QMessageBox::StandardButton::Close);
}


}
