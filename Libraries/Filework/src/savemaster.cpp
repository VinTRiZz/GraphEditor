#include "savemaster.h"

#include <Common/Logging.h>
#include <CustomWidgets/PasswordInsertDialog.h>

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "abstractsaveformat.h"
#include "formatfactory.h"

QString SaveMaster::formatToDefaultPath(const QString& iPath) {
    auto defaultExtension =
        Filework::FormatFactory::getInstance().getDefaultSaveExtension();
    auto fileSuffix = QFileInfo(iPath).completeSuffix();
    if (fileSuffix == defaultExtension) {
        return iPath;
    }
    return iPath + "." + defaultExtension;
}

QStringList SaveMaster::getAvailableFormats() {
    return Filework::FormatFactory::getInstance().getAvailableFormats();
}

QString SaveMaster::getSavePath() {
    return QFileDialog::getSaveFileName(nullptr, "Файл для сохранения графа",
                                        QDir::homePath(),
                                        getAvailableFormats().join(";;"));
}

QString SaveMaster::getLoadPath() {
    return QFileDialog::getOpenFileName(nullptr, "Файл для загрузки",
                                        QDir::homePath(),
                                        getAvailableFormats().join(";;"));
}

bool SaveMaster::save(const QString& oFilePath,
                      Graph::PMaintainer iGraphMaintaner) {
    auto fileSuffix = QFileInfo(oFilePath).completeSuffix();
    auto& formatFactory = Filework::FormatFactory::getInstance();
    if (fileSuffix.isEmpty()) {
        fileSuffix = formatFactory.getDefaultSaveExtension();
    }

    auto pFormat = formatFactory.getFormat(fileSuffix);

    if (pFormat->getIsEncrypted()) {
        PasswordInsertDialog passDialog;
        auto res = passDialog.exec();
        if (res != QDialog::Accepted) {
            QMessageBox::warning(nullptr, "Пароль", "Пароль не был введён");
            LOG_WARNING("Password input canceled");
            return false;
        }
        pFormat->setEncryptionKey(passDialog.getPassword());
    }

    if (!pFormat) {
        LOG_ERROR("Format did not recognized");
        return false;
    }

    pFormat->setGraphMaintaner(iGraphMaintaner);

    auto res = false;
    if (QFileInfo(oFilePath).completeSuffix().isEmpty()) {
        res = pFormat->save(oFilePath + "." +
                            fileSuffix);  // Суффикс точно будет на этом моменте
    } else {
        res = pFormat->save(oFilePath);
    }

    if (res) {
        LOG_OK("Saved graph by path:", oFilePath);
    } else {
        LOG_WARNING("Graph not saved. Path:", oFilePath);
    }
    return res;
}

bool SaveMaster::load(const QString& iFilePath,
                      Graph::PMaintainer oGraphMaintaner) {
    auto fileSuffix = QFileInfo(iFilePath).completeSuffix();
    auto& formatFactory = Filework::FormatFactory::getInstance();
    auto pFormat = formatFactory.getFormat(fileSuffix);
    if (!pFormat) {
        LOG_ERROR("Format did not recognized");
        return false;
    }

    if (pFormat->getIsEncrypted()) {
        PasswordInsertDialog passDialog;
        auto res = passDialog.exec();
        if (res != QDialog::Accepted) {
            QMessageBox::warning(nullptr, "Пароль", "Пароль не был введён");
            LOG_WARNING("Password input canceled");
            return false;
        }
        pFormat->setEncryptionKey(passDialog.getPassword());
    }

    pFormat->setGraphMaintaner(oGraphMaintaner);
    auto res = pFormat->load(iFilePath);

    if (res) {
        LOG_OK("Loaded graph by path:", iFilePath);
    } else {
        LOG_WARNING("Graph not loaded. Path:", iFilePath);
    }
    return res;
}
