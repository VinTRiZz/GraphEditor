#include "containers/treeobject.hpp"
#include <gtest/gtest.h>

#include <Components/ExtraClasses/Containers/TreeObject.h>

using Node_t = ExtraClasses::TreeNode<std::string>;
using NodePtr_t = ExtraClasses::TreeNodePtr<std::string>;

TEST(Components_ExtraClasses, TreeObjectRegularOperations) {
    NodePtr_t pRoot = Node_t::create("");

    // Check data setting
    pRoot->setData("Test data");
    ASSERT_EQ("Test data", pRoot->getData());

    // Check node count
    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");
    ASSERT_EQ(3, pRoot->getNodeCount());
    ASSERT_EQ(pRoot->getNode(1), pRoot->findNode("2"));

    // Check nodes data
    ASSERT_TRUE(pRoot->getNode(0) != nullptr);
    ASSERT_EQ("1", pRoot->getNode(0)->getData());
    ASSERT_TRUE(pRoot->getNode(1) != nullptr);
    ASSERT_EQ("2", pRoot->getNode(1)->getData());
    ASSERT_TRUE(pRoot->getNode(2) != nullptr);
    ASSERT_EQ("3", pRoot->getNode(2)->getData());

    // Check out of bounds
    ASSERT_TRUE(pRoot->getNode(2000) == nullptr);

    // Check node clearing
    pRoot->clearNodes();
    ASSERT_EQ(0, pRoot->getNodeCount());
    ASSERT_TRUE(pRoot->getNode(0) == nullptr);
}



// ============================ BASIC OPERATIONS //
TEST(Components_ExtraClasses, TreeObjectAddingNodes) {
    NodePtr_t pRoot = Node_t::create("");

    // Add by value
    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");
    ASSERT_EQ("2", pRoot->getNode(1)->getData());

    // Add by index
    pRoot->clearNodes();
    pRoot->addNode(0); pRoot->getNode(0)->setData("1");
    pRoot->addNode(0); pRoot->getNode(0)->setData("2");
    pRoot->addNode(0); pRoot->getNode(0)->setData("3");
    ASSERT_EQ("1", pRoot->getNode(2)->getData());
    ASSERT_EQ("3", pRoot->getNode(0)->getData());

    // Add as object
    pRoot->clearNodes();
    auto pNode = Node_t::create("");
    pNode->setData("1");
    pRoot->addNode(pNode);
    pNode = Node_t::create("");
    pNode->setData("2");
    pRoot->addNode(pNode);
    pNode = Node_t::create("");
    pNode->setData("3");
    pRoot->addNode(pNode);
    ASSERT_EQ("1", pRoot->getNode(0)->getData());
    ASSERT_EQ("2", pRoot->getNode(1)->getData());
    ASSERT_EQ("3", pRoot->getNode(2)->getData());
}

TEST(Components_ExtraClasses, TreeObjectRemovingNodes) {
    NodePtr_t pRoot = Node_t::create("");

    // Remove by value
    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");
    ASSERT_TRUE(pRoot->removeNode("2"));
    ASSERT_EQ("3", pRoot->getNode(1)->getData());

    // Remove by index
    pRoot->clearNodes();
    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");
    ASSERT_TRUE(pRoot->removeNode(1));
    ASSERT_EQ("3", pRoot->getNode(1)->getData());

    // Remove as object
    pRoot->clearNodes();
    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");
    auto pSecondNode = pRoot->getNode(1);
    ASSERT_TRUE(pRoot->removeNode(pSecondNode));
    ASSERT_EQ("3", pRoot->getNode(1)->getData());
}



// ============================ RECURSION //
TEST(Components_ExtraClasses, TreeObjectBranchMoving) {
    ExtraClasses::TreeNodePtr<int> pRoot = ExtraClasses::TreeNode<int>::create(0);
    pRoot->setData(1);

    // Setup tree base
    int curNodeNo {1}; // First node is root
    pRoot->addNode(++curNodeNo);
    pRoot->addNode(++curNodeNo);
    pRoot->getNode(1)->addNode(++curNodeNo);
    pRoot->getNode(1)->getNode(0)->addNode(++curNodeNo);

    // Move branch
    ASSERT_EQ(2, pRoot->getNodeCount());
    auto pNode = pRoot->getNode(1)->takeNode(0);
    pNode->setParent(pRoot);
    ASSERT_EQ(3, pRoot->getNodeCount());
    ASSERT_EQ(pNode, pRoot->getNode(pRoot->getNodeCount() - 1));
    ASSERT_NE(pNode, pRoot->getNode(1)->getNode(0));

    // Setup other branch
    pRoot->addNode(++curNodeNo);
    pRoot->getNode(1)->addNode(++curNodeNo);

    // Check count of items
    int counter {0};
    pRoot->callRecursive([&counter](const auto& pNode) -> bool {
        ++counter;
        return false;
    });
    ASSERT_EQ(curNodeNo, counter);
}


TEST(Components_ExtraClasses, TreeObjectRecursiveSameLayer) {
    NodePtr_t pRoot = Node_t::create("");

    pRoot->setData("Test data");
    ASSERT_EQ("Test data", pRoot->getData());

    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");

    // Search in 0 depth
    std::string foundData;
    int callCount {0};
    pRoot->callRecursive([&foundData, &callCount](const auto& pNode) -> bool {
        ++callCount;
        if (pNode->getData() == "2") {
            foundData = "2";
            return true;
        }
        return false;
    });
    ASSERT_EQ("2", foundData);
    ASSERT_EQ(3, callCount);

    // Search for root node from root node
    foundData.clear();
    callCount = 0;
    pRoot->callRecursive([&foundData, &callCount](const auto& pNode) -> bool {
        ++callCount;
        if (pNode->getData() == "Test data") {
            foundData = "Test data";
            return true;
        }
        return false;
    });
    ASSERT_EQ("Test data", foundData);
    ASSERT_EQ(1, callCount);
}


TEST(Components_ExtraClasses, TreeObjectRecursiveDown) {
    ExtraClasses::TreeNodePtr<int> pRoot = ExtraClasses::TreeNode<int>::create(0);
    pRoot->setData(1);

    // Setup tree
    int curNodeNo {1}; // First node is root
    pRoot->addNode(++curNodeNo);
    pRoot->addNode(++curNodeNo);
    pRoot->getNode(1)->addNode(++curNodeNo);
    pRoot->getNode(1)->getNode(0)->addNode(++curNodeNo);
    pRoot->addNode(++curNodeNo);
    pRoot->getNode(1)->addNode(++curNodeNo);

    // Get total item count
    int counter {0};
    pRoot->callRecursive([&counter](const auto& pNode) -> bool {
        ++counter;
        return false;
    });
    ASSERT_EQ(curNodeNo, counter);

    // Warning here is normal (it's comparison between pointers inside).
    // Otherwise does not even compile
    ASSERT_EQ(std::size_t(curNodeNo - 1), pRoot->getTotalChildCount());
}


TEST(Components_ExtraClasses, TreeObjectRecursiveUp) {
    ExtraClasses::TreeNodePtr<int> pRoot = ExtraClasses::TreeNode<int>::create(0);
    pRoot->setData(1);

    int curNodeNo {1};
    pRoot->addNode(++curNodeNo);                         // L1
    pRoot->getNode(0)->addNode(++curNodeNo);             // L2
    pRoot->getNode(0)->getNode(0)->addNode(++curNodeNo); // L3

    auto pDepthNode = pRoot->getNode(0)->getNode(0)->getNode(0);
    pDepthNode->addNode(++curNodeNo);                    // L4
    pDepthNode = pDepthNode->getNode(0);

    int depth {0};
    pDepthNode->callBackwardRecursive([&depth](const auto& pNode) -> bool {
        ++depth;
        return false;
    });
    ASSERT_EQ(5, depth);
    ASSERT_EQ(0, pRoot->getDepth());
    ASSERT_EQ(4, pDepthNode->getDepth());
}
