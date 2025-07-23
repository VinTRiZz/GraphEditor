#include <gtest/gtest.h>

#include <GraphObject/Object.h>

TEST(GraphObject, VertexWork) {

    Graph::GraphObject testObject;

    Graph::GVertex testVertex;
    testVertex.id = 1000;
    EXPECT_EQ(testObject.addVertex(testVertex), true);
}
