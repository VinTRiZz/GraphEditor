#include "savemaster.h"

#include "abstractsaveformat.h"

#include "gse_format.h"
#include "gsj_format.h"
#include "gsej_format.h"

#include <Common/Logging.h>

#include <CustomWidgets/PasswordInsertDialog.h>
#include <QMessageBox>

#include <QFileInfo>
#include <QFileDialog>
#include <QDir>

QString SaveMaster::formatToDefaultPath(const QString &iPath)
{
    auto fileSuffix = QFileInfo(iPath).completeSuffix();
    if (fileSuffix == "gsj") {
        return iPath;
    }
    return iPath + ".gsj";
}

QStringList SaveMaster::getAvailableFormats()
{
    QStringList res;
    res << "Файл графа (.gsj) (*.gsj)";
    res << "Файл зашифрованного графа (.gsje) (*.gsej)";
    res << "Старый формат графа (.gse) (*.gse)";
    return res;
}

QString SaveMaster::getSavePath()
{
    return QFileDialog::getSaveFileName(
                nullptr,
                "Файл для сохранения графа",
                QDir::homePath(),
                getAvailableFormats().join(";;"));
}

QString SaveMaster::getLoadPath()
{
    return QFileDialog::getOpenFileName(
                nullptr,
                "Файл для загрузки",
                QDir::homePath(),
                getAvailableFormats().join(";;"));
}

bool SaveMaster::save(const QString &oFilePath, Graph::PMaintainer iGraphMaintaner)
{
    auto fileSuffix = QFileInfo(oFilePath).completeSuffix();
    auto pFormat = getFormat(fileSuffix);
    if (!pFormat) {
        LOG_ERROR("Format did not recognized");
        return false;
    }

    pFormat->setGraphMaintaner(iGraphMaintaner);

    auto res = false;
    if (fileSuffix.isEmpty()) {
        res = pFormat->save(formatToDefaultPath(oFilePath));
    } else {
        res = pFormat->save(oFilePath);
    }

    if (res) {
        LOG_OK("Saved graph by path:", oFilePath);
    } else {
        LOG_OK("Graph not saved. Path:", oFilePath);
    }
    return res;
}

bool SaveMaster::load(const QString &iFilePath, Graph::PMaintainer oGraphMaintaner)
{
    auto fileSuffix = QFileInfo(iFilePath).completeSuffix();
    auto pFormat = getFormat(fileSuffix);
    if (!pFormat) {
        LOG_ERROR("Format did not recognized");
        return false;
    }

    pFormat->setGraphMaintaner(oGraphMaintaner);
    auto res = pFormat->load(iFilePath);

    if (res) {
        LOG_OK("Saved graph by path:", iFilePath);
    } else {
        LOG_OK("Graph not saved. Path:", iFilePath);
    }
    return res;
}

std::shared_ptr<Filework::AbstractSaveFormat> SaveMaster::getFormat(const QString &fileSuffix)
{
    std::shared_ptr<Filework::AbstractSaveFormat> pFormat;
    if (fileSuffix == "gse") {
        pFormat = std::make_shared<Filework::GSE_Format>();

    } else if (fileSuffix == "gsej") {
        PasswordInsertDialog passDialog;
        auto res = passDialog.exec();
        if (res != QDialog::Accepted) {
            QMessageBox::warning(nullptr, "Пароль", "Пароль не был введён");
            LOG_WARNING("Password input canceled");
            return pFormat;
        }

        pFormat = std::make_shared<Filework::GSEJ_Format>();
        std::dynamic_pointer_cast<Filework::GSEJ_Format>(pFormat)->setKey(passDialog.getPassword());

    } else if ((fileSuffix == "gsj") || fileSuffix.isEmpty()) {
        pFormat = std::make_shared<Filework::GSJ_Format>();

    } else {
        LOG_ERROR("Unknown format got:", fileSuffix);
    }
    return pFormat;
}
