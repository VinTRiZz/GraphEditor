#include "objectpropertyeditorform.h"

#include <Components/Common/CommonFunctions.h>
#include <Components/Common/DirectoryManager.h>
#include <AppInfrastructure/ImageManager.h>
#include <Components/Logger/Logger.h>
#include <GraphObject/Components.h>

#include <QBuffer>
#include <QFileDialog>
#include <QVariant>

#include <GraphItems/VertexConnectionItem.h>
#include <GraphItems/VertexObjectItem.h>

#include "ui_objectpropertyeditorform.h"

const auto PROPEDITORFORM_PROPERTY_IMAGEHASH{"imagehash"};

using namespace CommonFunctions;

ObjectPropertyEditorForm::ObjectPropertyEditorForm(QWidget* parent)
    : QWidget(parent), ui(new Ui::ObjectPropertyEditorForm) {
    ui->setupUi(this);

    initHistoryGalery();
    initIcons();

    connect(ui->accept_pushButton, &QPushButton::clicked, this,
            &ObjectPropertyEditorForm::acceptChanges);

    connect(ui->cancel_pushButton, &QPushButton::clicked, this,
            &ObjectPropertyEditorForm::cancelChanges);

    connectColorDialog(ui->selectMainColor_pushButton, ui->mainColor_label);
    connectColorDialog(ui->selectBgrColor_pushButton, ui->bgrColor_label);
    connectColorDialog(ui->selectSelectionColor_pushButton,
                       ui->selectedColor_label);

    ui->shortName_lineEdit->setMaxLength(Graph::GRAPH_MAX_SHORTNAME_SIZE);
    ui->property_tabWidget->setCurrentIndex(0);
}

ObjectPropertyEditorForm::~ObjectPropertyEditorForm() {
    delete ui;
}

void ObjectPropertyEditorForm::setTargetItem(
    ObjectViewItems::ItemBase* pTargetItem) {
    m_pTargetItem = pTargetItem;
    ui->name_lineEdit->setText(m_pTargetItem->toolTip());
    ui->shortName_lineEdit->setText(m_pTargetItem->getDisplayName());
    ui->description_plainTextEdit->setPlainText(
        m_pTargetItem->getDescription());

    setColor(ui->mainColor_label, pTargetItem->getBorderColor());
    setColor(ui->bgrColor_label, pTargetItem->getBackgroundColor());
    setColor(ui->selectedColor_label, pTargetItem->getSelectionColor());

    auto pVertex = dynamic_cast<Graph::VertexObject*>(m_pTargetItem);

    if (nullptr != pVertex) {
        selectImage(pVertex->getImageHash());
    }
    ui->property_tabWidget->setTabEnabled(1, nullptr != pVertex);

    auto isConnectionEditing =
        pTargetItem->getType() ==
        Graph::OBJECTTYPE_CONNECTION;
    ui->name_lineEdit->setEnabled(!isConnectionEditing);
    ui->description_plainTextEdit->setEnabled(!isConnectionEditing);
}

void ObjectPropertyEditorForm::acceptChanges() {
    LOG_INFO("Changing data of object");
    m_pTargetItem->setDisplayName(ui->shortName_lineEdit->text());
    m_pTargetItem->setToolTip(ui->name_lineEdit->text());
    m_pTargetItem->setDescription(ui->description_plainTextEdit->toPlainText());

    m_pTargetItem->setBorderColor(getColor(ui->mainColor_label));
    m_pTargetItem->setBackgroundColor(getColor(ui->bgrColor_label));
    m_pTargetItem->setSelectionColor(getColor(ui->selectedColor_label));

    if (auto pVertex =
            dynamic_cast<Graph::VertexObject*>(m_pTargetItem);
        nullptr != pVertex) {
        if (nullptr != m_selectedIconLabel) {
            auto& imgManager = ImageManager::getInstance();
            auto imgHash =
                m_selectedIconLabel->property(PROPEDITORFORM_PROPERTY_IMAGEHASH)
                    .toString();
            pVertex->setImageByHash(imgHash);
        } else {
            pVertex->setImageByHash({});
        }
    }

    if (auto pConnection =
            dynamic_cast<Graph::VertexConnectionLine*>(m_pTargetItem);
        nullptr != pConnection) {
        // TODO: Do something with styles
        //        pConnection
    }

    emit changedItemData();
}

void ObjectPropertyEditorForm::cancelChanges() {
    LOG_INFO("Canceled change data of object");
    setTargetItem(m_pTargetItem);  // ez
    emit editCanceled();
}

void ObjectPropertyEditorForm::initHistoryGalery() {
    ui->imageHistoryGalery->init();
    ui->imageHistoryGalery->setColumnCount(3);
    ui->imageHistoryGalery->setSelectionColor(QColor(161, 209, 207));

    connect(ui->openImage_pushButton, &QPushButton::clicked, this, [this]() {
        auto targetPath =
            QFileDialog::getOpenFileName(nullptr, "Выберите изображение",
                                         QDir::homePath());

        addHistoryImage(targetPath);
    });

    connect(ui->imageHistoryGalery, &WidgetGalery::selectionChanged, this,
            [this](QWidget* pWidget) {
                if (nullptr != pWidget) {
                    ui->iconGalery->clearSelection();
                }
                m_selectedIconLabel = pWidget;
            });

    auto history = ImageManager::getImageHistoryPaths();
    for (auto& img : history) {
        addHistoryImage(img);
    }
}

void ObjectPropertyEditorForm::initIcons() {
    ui->iconGalery->init();
    ui->iconGalery->setColumnCount(3);
    ui->iconGalery->setSelectionColor(QColor(161, 209, 207));

    connect(ui->iconGalery, &WidgetGalery::selectionChanged, this,
            [this](QWidget* pWidget) {
                if (nullptr != pWidget) {
                    ui->imageHistoryGalery->clearSelection();
                }
                m_selectedIconLabel = pWidget;
            });

    // Добавляем элементы из ресурсов
    auto addIcon = [this](const QString& iconPath, const QString& iconName) {
        const QString iconBasepath = ":/common/images/vertexicons/%0";
        auto& imgmanager = ImageManager::getInstance();

        auto img = imgmanager.getImageFromPath(iconBasepath.arg(iconPath));
        auto labelFixedSize = QSize(80, 80);

        auto pxmap = QPixmap::fromImage(img.second.scaled(labelFixedSize));
        auto pLabel = new QLabel(this);
        pLabel->setPixmap(pxmap);
        pLabel->setProperty(PROPEDITORFORM_PROPERTY_IMAGEHASH, img.first);
        ui->iconGalery->addWidget(pLabel, iconName);
    };

    addIcon("person/green.svg", "Человек (З)");
    addIcon("person/red.svg", "Человек (К)");
}

void ObjectPropertyEditorForm::addHistoryImage(const QString& targetPath) {
    auto img = ImageManager::getInstance().getImageFromPath(targetPath);

    auto imgName = QFileInfo(targetPath).baseName();
    auto existWidget = ui->imageHistoryGalery->getWidget([&img](
                                                             QWidget* pLabel) {
        return (img.first ==
                pLabel->property(PROPEDITORFORM_PROPERTY_IMAGEHASH).toString());
    });
    if (nullptr != existWidget) {
        ui->imageHistoryGalery->setLabel(existWidget, imgName);
        LOG_INFO("Image exist, nothing to add into galery");
        return;
    }

    auto& imgManager = ImageManager::getInstance();
    auto labelFixedSize = QSize(80, 80);
    auto pxmap = QPixmap::fromImage(
        img.second.scaled(labelFixedSize.width(), labelFixedSize.height()));

    auto pLabel = new QLabel(this);
    pLabel->setPixmap(pxmap);
    pLabel->setProperty(PROPEDITORFORM_PROPERTY_IMAGEHASH, img.first);
    ui->imageHistoryGalery->addWidget(pLabel, imgName);

    auto imageHistoryDir = Common::DirectoryManager::getDirectoryStatic(
        Common::DirectoryManager::DirectoryType::Temporary).absolutePath() + QDir::separator();
    QFile::copy(targetPath, imageHistoryDir + QFileInfo(targetPath).fileName());
}

void ObjectPropertyEditorForm::selectImage(const QString& imageHash) {
    auto selectResult = ui->imageHistoryGalery->selectWidget([&](auto* pWidget)
                                                                 -> bool {
        auto res =
            (imageHash ==
             pWidget->property(PROPEDITORFORM_PROPERTY_IMAGEHASH).toString());
        if (res) {
            m_selectedIconLabel = pWidget;
        }
        return res;
    });
    if (selectResult) {
        return;
    }

    ui->iconGalery->selectWidget([&](auto* pWidget) -> bool {
        auto res =
            (imageHash ==
             pWidget->property(PROPEDITORFORM_PROPERTY_IMAGEHASH).toString());
        if (res) {
            m_selectedIconLabel = pWidget;
        }
        return res;
    });
}
