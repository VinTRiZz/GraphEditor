#include "objectscene.h"
#include "ui_objectscene.h"
#include "objectsinternalscene.h"

#include "logging.h"

ObjectScene::ObjectScene(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::ObjectScene)
{
    ui->setupUi(this);
}

ObjectScene::~ObjectScene()
{
    delete ui;
}

void ObjectScene::setIdGenerator(const std::function<uint ()> fGen)
{
    m_pScene->setIdGenerator(fGen);
}

std::function<uint ()> ObjectScene::getIdGenerator() const
{
    return m_pScene->getIdGenerator();
}

void ObjectScene::init()
{
    m_pScene = new ObjectsInternalScene(this);
    m_pScene->init();
    setScene(m_pScene);
}

uint ObjectScene::addObject(QGraphicsItem *pItem)
{
    return m_pScene->addObject(pItem);
}

QList<uint> ObjectScene::getAlObjectIds() const
{
    return m_pScene->getAlObjectIds();
}

void ObjectScene::removeObject(uint itemId)
{
    return m_pScene->removeObject(itemId);
}
