#include "scenefielditem.h"
#include "constants.h"

#include <gtest/gtest.h>

TEST(SceneFieldItemTest, SetFieldRect) {
    auto fieldItem = std::shared_ptr<ObjectViewItems::SceneFieldItem>(new ObjectViewItems::SceneFieldItem);

    QRectF rect(10, 20, 300, 400);
    fieldItem->setFieldRect(rect);
    EXPECT_EQ(fieldItem->getFieldRect(), rect);
}

TEST(SceneFieldItemTest, RegisterAndRemoveItems) {
    auto fieldItem = std::shared_ptr<ObjectViewItems::SceneFieldItem>(new ObjectViewItems::SceneFieldItem);

    auto item1 = new ObjectViewItems::ItemBase;
    item1->setObjectId(1);
    item1->setType(ObjectViewConstants::ObjectType::OBJECTTYPE_NONE);
    fieldItem->registerItem(item1);

    auto item2 = new ObjectViewItems::ItemBase;
    item2->setObjectId(2);
    item2->setType(ObjectViewConstants::ObjectType::OBJECTTYPE_ARROWLINE);
    fieldItem->registerItem(item2);

    auto item3 = new ObjectViewItems::ItemBase;
    item3->setObjectId(3);
    item3->setType(ObjectViewConstants::ObjectType::OBJECTTYPE_PROPERTY_EDITOR);
    fieldItem->registerItem(item3);

    auto items = fieldItem->getRegisteredItems();
    ASSERT_EQ(items.size(), 3);

    EXPECT_EQ(fieldItem->getItem(1), item1);
    EXPECT_EQ(fieldItem->getItem(2, ObjectViewConstants::ObjectType::OBJECTTYPE_ARROWLINE), item2);
    EXPECT_EQ(fieldItem->getItem(3, ObjectViewConstants::ObjectType::OBJECTTYPE_PROPERTY_EDITOR), item3);

    auto ids = fieldItem->getRegisteredIds();
    ASSERT_EQ(ids.size(), 3);
    EXPECT_NE(std::find(ids.begin(), ids.end(), 1), ids.end());
    EXPECT_NE(std::find(ids.begin(), ids.end(), 2), ids.end());
    EXPECT_NE(std::find(ids.begin(), ids.end(), 3), ids.end());

    EXPECT_FALSE(fieldItem->isIdAvailable(1));
    EXPECT_TRUE(fieldItem->isIdAvailable(4));
}

TEST(SceneFieldItemTest, ItemRemoval) {
    auto fieldItem = std::shared_ptr<ObjectViewItems::SceneFieldItem>(new ObjectViewItems::SceneFieldItem);

    auto item1 = new ObjectViewItems::ItemBase;
    item1->setObjectId(1);
    item1->setType(ObjectViewConstants::ObjectType::OBJECTTYPE_ARROWLINE);
    fieldItem->registerItem(item1);

    auto item2 = new ObjectViewItems::ItemBase;
    item2->setObjectId(2);
    item2->setType(ObjectViewConstants::ObjectType::OBJECTTYPE_ARROWLINE);
    fieldItem->registerItem(item2);

    auto item3 = new ObjectViewItems::ItemBase;
    item3->setObjectId(3);
    item3->setType(ObjectViewConstants::ObjectType::OBJECTTYPE_ARROWLINE);
    fieldItem->registerItem(item3);

    fieldItem->removeRegisteredItem(item2);
    EXPECT_EQ(fieldItem->getRegisteredItems().size(), 2);
    EXPECT_EQ(fieldItem->getItem(2), nullptr);

    fieldItem->removeRegisteredItemById(1);
    EXPECT_EQ(fieldItem->getRegisteredItems().size(), 1);
    EXPECT_EQ(fieldItem->getItem(1), nullptr);

    fieldItem->removeRegisteredItems(ObjectViewConstants::ObjectType::OBJECTTYPE_ARROWLINE);
    EXPECT_TRUE(fieldItem->getRegisteredItems().empty());
}

TEST(SceneFieldItemTest, ClearAllItems) {
    auto fieldItem = std::shared_ptr<ObjectViewItems::SceneFieldItem>(new ObjectViewItems::SceneFieldItem);

    auto item1 = new ObjectViewItems::ItemBase;
    item1->setObjectId(1);
    item1->setType(ObjectViewConstants::ObjectType::OBJECTTYPE_NONE);

    auto item2 = new ObjectViewItems::ItemBase;
    item2->setObjectId(2);
    item2->setType(ObjectViewConstants::ObjectType::OBJECTTYPE_ARROWLINE);

    fieldItem->registerItem(item1);
    fieldItem->registerItem(item2);

    fieldItem->clearRegisteredItems();
    EXPECT_TRUE(fieldItem->getRegisteredItems().empty());
    EXPECT_TRUE(fieldItem->getRegisteredIds().empty());
}
