#include "filemanagementtoolbar.hpp"
#include "ui_filemanagementtoolbar.h"

#include <QMessageBox>

#include <Filework/SaveMaster.h>

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
        if (savePath.isNull()) {
            return;
        }
        auto saveRes = saveGraph(savePath);
        ui->save_toolButton->setEnabled(saveRes);
        ui->load_toolButton->setEnabled(saveRes);
    });

    connect(ui->loadAs_toolButton, &QToolButton::clicked,
            this, [this](){
        auto loadPath = SaveMaster::getLoadPath();
        if (loadPath.isNull()) {
            return;
        }

        auto loadRes = loadGraph(loadPath);
        if (loadRes) {
            emit loadedGraph(loadPath);
        }
        ui->save_toolButton->setEnabled(loadRes);
        ui->load_toolButton->setEnabled(loadRes);
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
    if (savePath.isNull()) {
        getGraph()->setSavefile(savePath);
    } else if (!QFileInfo(savePath).dir().exists()) {
        LOG_ERROR("Can not save file with path:", savePath);
        return false;
    }
    getGraph()->setEditTime(QDateTime::currentDateTime());

    SaveMaster saveMaster;
    return saveMaster.save(getGraph()->getSavefile(), getGraph());
}

bool FileManagementToolbar::loadGraph(const QString &savePath)
{
    if (!isGraphSet()) { // Подразумевается, что результат будет забираться из этого объекта
        setGraph(Graph::GraphMaintainer::createInstance());
    }
    if (!savePath.isNull()) {
        auto targetFileInfo = QFileInfo(savePath);
        if (!targetFileInfo.exists() || !targetFileInfo.isReadable()) {
            LOG_ERROR("Can not loading file with path:", savePath);
            return false;
        }
        getGraph()->setSavefile(savePath);
    }

    SaveMaster saveMaster;
    return saveMaster.load(getGraph()->getSavefile(), getGraph());
}

void FileManagementToolbar::processGraphChange()
{
    auto pMaintainer = getGraph();

    auto isMaintainerValid = (pMaintainer != nullptr);
    ui->saveAs_toolButton->setEnabled(isMaintainerValid);

    if (isMaintainerValid) {
        auto isPathValid = !pMaintainer->getSavefile().isNull();
        ui->save_toolButton->setEnabled(isPathValid);
        ui->load_toolButton->setEnabled(isPathValid);
    } else {
        ui->save_toolButton->setEnabled(false);
        ui->load_toolButton->setEnabled(false);
    }
}
