#include "filemanagementtoolbar.hpp"
#include "ui_filemanagementtoolbar.h"

#include <QMessageBox>
#include <QFileInfo>

#include <Filework/SaveMaster.h>

#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Common/Utils.h>

FileManagementToolbar::FileManagementToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileManagementToolbar)
{
    ui->setupUi(this);

    ui->saveAs_toolButton->setEnabled(false);
    connect(ui->saveAs_toolButton, &QToolButton::clicked,
            this, [this](){
        if (!isGraphSet()) { return; }
        auto savePath = SaveMaster::getSavePath();
        if (savePath.isEmpty()) {
            return;
        }
        auto saveRes = saveGraph(savePath);
        ui->save_toolButton->setEnabled(saveRes);
        ui->load_toolButton->setEnabled(saveRes);
    });

    connect(ui->loadAs_toolButton, &QToolButton::clicked,
            this, [this](){
        auto loadPath = SaveMaster::getLoadPath();
        if (loadPath.isEmpty()) {
            return;
        }

        auto pCurrentGraph = getGraph();
        setGraph(Graph::GraphObjectManager::createGraphInstance());
        auto loadRes = loadGraph(loadPath);

        // Обновляем свойства формы
        if (!loadRes) {
            setGraph(pCurrentGraph);
        } else {
            setGraph(getGraph());
        }
    });

    ui->save_toolButton->setEnabled(false);
    connect(ui->save_toolButton, &QToolButton::clicked,
            this, [this](){ saveGraph(); });

    ui->load_toolButton->setEnabled(false);
    connect(ui->load_toolButton, &QToolButton::clicked,
            this, [this](){ loadGraph(); });

    connect(ui->create_toolButton, &QToolButton::clicked,
            this, &FileManagementToolbar::createGraph);
}

FileManagementToolbar::~FileManagementToolbar()
{
    delete ui;
}

bool FileManagementToolbar::saveGraph(const QString &savePath)
{
    if (!isGraphSet()) { return false; }
    if (!savePath.isEmpty()) {
        getGraph()->getObject()->getMetaInfo()->setSavepath(savePath);
    } else if (!QFileInfo(savePath).dir().exists()) {
        LOG_ERROR("Can not save file with path:", savePath);
        return false;
    }
    getGraph()->getObject()->getMetaInfo()->setEditTime(QDateTime::currentDateTime());

    SaveMaster saveMaster;
    return saveMaster.save(getGraph()->getObject()->getMetaInfo()->getSavepath(), getGraph());
}

bool FileManagementToolbar::saveGraphAsTemporary()
{
    if (!isGraphSet()) { return false; }
    SaveMaster saveMaster;

    // Создаём временное название для файла
    auto graphFileName = Common::createRandomString(8);
    auto tmpSavePathBase = Common::DirectoryManager::getDirectoryStatic(Common::DirectoryManager::DirectoryType::Temporary).absolutePath();
    auto graphSavepath = saveMaster.formatToDefaultPath(tmpSavePathBase + QDir::separator() + graphFileName.c_str());
    while (QFileInfo(graphSavepath).exists()) {
        graphFileName = Common::createRandomString(8);
        graphSavepath = saveMaster.formatToDefaultPath(tmpSavePathBase + QDir::separator() + graphFileName.c_str());
    }
    getGraph()->getObject()->getMetaInfo()->setSavepath(graphSavepath);

    // Сохраняем
    return saveMaster.save(graphSavepath, getGraph());
}

bool FileManagementToolbar::loadGraph(const QString &savePath)
{
    if (!isGraphSet()) { // Подразумевается, что результат будет забираться из этого объекта
        setGraph(Graph::GraphObjectManager::createGraphInstance());
    }
    if (!savePath.isEmpty()) {
        auto targetFileInfo = QFileInfo(savePath);
        if (!targetFileInfo.exists() || !targetFileInfo.isReadable()) {
            LOG_ERROR("Can not loading file with path:", savePath);
            return false;
        }
        getGraph()->getObject()->getMetaInfo()->setSavepath(savePath);
    }

    auto graphPath = getGraph()->getObject()->getMetaInfo()->getSavepath();

    SaveMaster saveMaster;
    auto res = saveMaster.load(graphPath, getGraph());
    if (res) {
        emit loadedGraph(graphPath);
    }
    return res;
}

void FileManagementToolbar::processGraphChange()
{
    ui->saveAs_toolButton->setEnabled(isGraphSet());

    if (isGraphSet()) {
        auto savePath = getGraph()->getObject()->getMetaInfo()->getSavepath();
        auto isPathValid = !savePath.isEmpty() && QFileInfo(savePath).exists();
        ui->save_toolButton->setEnabled(isPathValid);
        ui->load_toolButton->setEnabled(isPathValid);
    } else {
        ui->save_toolButton->setEnabled(false);
        ui->load_toolButton->setEnabled(false);
    }
}
