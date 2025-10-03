#include "savemaster.h"

#include <AppInfrastructure/ApplicationSettings.h>
#include <AppInfrastructure/DirectoryManager.h>
#include <Components/Logger/Logger.h>
#include <Components/CustomQt/PasswordInsertDialog.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "abstractsaveformat.h"
#include "encryptedsaveformat.h"
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
                                        DirectoryManager::getDocumentsPath(),
                                        getAvailableFormats().join(";;"));
}

QString SaveMaster::getLoadPath() {
    return QFileDialog::getOpenFileName(nullptr, "Файл для загрузки",
                                        DirectoryManager::getDocumentsPath(),
                                        getAvailableFormats().join(";;"));
}

bool SaveMaster::save(const QString& oFilePath,
                      Graph::PMaintainer iGraphMaintaner) {
    LOG_INFO("Saving graph data as:", oFilePath);
    auto fileSuffix = QFileInfo(oFilePath).completeSuffix();
    auto& formatFactory = Filework::FormatFactory::getInstance();
    if (fileSuffix.isEmpty()) {
        fileSuffix = formatFactory.getDefaultSaveExtension();
    }

    auto pFormat = formatFactory.getFormat(fileSuffix);
    if (!pFormat->isBackwardCompatible() && ApplicationSettings::getInstance()
                                                .getGeneralConfig()
                                                .m_needBackwardCompatibility) {
        auto userResponse = QMessageBox::question(
            nullptr, "Обратная совместимость",
            "Формат не является обратно совместимым.\nПри сохранении в этом "
            "формате, созданные\n кастомные объекты будут "
            "утеряны.\nПродолжить?",
            QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);

        if (userResponse != QMessageBox::StandardButton::Yes) {
            LOG_WARNING("Backward compatibility required, canceled saving");
            return false;
        }
    }

    auto pEncryptedFormat =
        std::dynamic_pointer_cast<Filework::EncryptedSaveFormat>(pFormat);
    if (pEncryptedFormat) {
        PasswordInsertDialog passDialog;
        auto res = passDialog.exec();
        if (res != QDialog::Accepted) {
            LOG_WARNING("Password input canceled");
            QMessageBox::warning(nullptr, "Пароль", "Пароль не был введён");
            return false;
        }
        pEncryptedFormat->setEncryptionKey(passDialog.getPassword());
    }

    if (!pFormat) {
        LOG_ERROR("Format did not recognized");
        QMessageBox::critical(nullptr, "Ошибка сохранения",
                              "Неизвестный формат");
        return false;
    }

    pFormat->setGraphMaintainer(iGraphMaintaner);

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
        QMessageBox::critical(nullptr, "Ошибка сохранения",
                              "Не удалось сохранить файл в данном "
                              "формате.\nПроверьте права доступа к директории");
    }
    return res;
}

bool SaveMaster::load(const QString& iFilePath,
                      Graph::PMaintainer oGraphMaintaner) {
    LOG_INFO("Loading graph data from:", iFilePath);
    auto fileSuffix = QFileInfo(iFilePath).completeSuffix();
    auto& formatFactory = Filework::FormatFactory::getInstance();
    auto pFormat = formatFactory.getFormat(fileSuffix);
    if (!pFormat) {
        LOG_ERROR("Format did not recognized");
        QMessageBox::critical(nullptr, "Ошибка загрузки", "Неизвестный формат");
        return false;
    }

    auto pEncryptedFormat =
        std::dynamic_pointer_cast<Filework::EncryptedSaveFormat>(pFormat);
    if (pEncryptedFormat) {
        PasswordInsertDialog passDialog;
        auto res = passDialog.exec();
        if (res != QDialog::Accepted) {
            LOG_WARNING("Password input canceled");
            QMessageBox::warning(nullptr, "Пароль", "Пароль не был введён");
            return false;
        }
        pEncryptedFormat->setEncryptionKey(passDialog.getPassword());
    }

    pFormat->setGraphMaintainer(oGraphMaintaner);
    auto res = pFormat->load(iFilePath);

    if (res) {
        LOG_OK("Loaded graph by path:", iFilePath);
    } else {
        LOG_WARNING("Graph not loaded. Path:", iFilePath);
        QMessageBox::critical(nullptr, "Ошибка загрузки",
                              "Ошибка загрузки данных графа.\nВозможно, формат "
                              "сохранений устаревший\nили файл был испорчен.");
    }
    return res;
}
