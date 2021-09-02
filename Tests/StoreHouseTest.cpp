#include <iostream>
#include <memory>
#include <format>
#include <gtest/gtest.h>

#include "StoreHouse.hpp"

class StoreHouseTest : public ::testing::Test
{
protected:
    StoreHouseTest() {}

    void SetUp() override
    {
    }

    void TearDown() override {}

    ~StoreHouseTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(StoreHouseTest, CreateTest)
{
    auto storeHouse = std::make_unique<sd::StoreHouse>(1);

    auto data = storeHouse->getStoreHouseData();

    EXPECT_EQ(data.id, 1);
}

TEST_F(StoreHouseTest, ToStringTest)
{
    auto storeHouse = std::make_unique<sd::StoreHouse>(1);

    EXPECT_EQ(storeHouse->toString(), "STOREHOUSE #1");
}

TEST_F(StoreHouseTest, StructureRaportTest)
{
    auto storeHouse = std::make_unique<sd::StoreHouse>(1);

    EXPECT_EQ(storeHouse->getStructureRaport(0), "STOREHOUSE #1");
}

TEST_F(StoreHouseTest, StateRaportTest)
{
    auto storeHouse = std::make_unique<sd::StoreHouse>(1);

    auto expected = "STOREHOUSE #1\n\tQueue: ";

    EXPECT_EQ(storeHouse->getStateRaport(0), expected);
}

TEST_F(StoreHouseTest, StateRaportWithFilledQueueTest)
{
    auto storeHouse = std::make_unique<sd::StoreHouse>(1);

    auto product1 = std::make_unique<sd::Product>();
    auto product2 = std::make_unique<sd::Product>();
    auto product3 = std::make_unique<sd::Product>();

    auto expectedProduct1Id = product1->getId();
    auto expectedProduct2Id = product2->getId();
    auto expectedProduct3Id = product3->getId();

    storeHouse->moveInProduct(std::move(product1));
    storeHouse->moveInProduct(std::move(product2));
    storeHouse->moveInProduct(std::move(product3));

    auto expected = std::format("STOREHOUSE #1\n\tQueue: #{}, #{}, #{}", expectedProduct1Id, expectedProduct2Id, expectedProduct3Id);

    EXPECT_EQ(storeHouse->getStateRaport(0), expected);
}

TEST_F(StoreHouseTest, NodeTypeTest)
{
    auto storeHouse = std::make_unique<sd::StoreHouse>(1);

    EXPECT_EQ(storeHouse->getNodeType(), sd::NodeType::STORE);
}

TEST_F(StoreHouseTest, MoveInTest)
{
    auto storeHouse = std::make_unique<sd::StoreHouse>(1);

    auto product = std::make_unique<sd::Product>();
    auto expectedProductId = product->getId();

    storeHouse->moveInProduct(std::move(product));

    EXPECT_TRUE(storeHouse->areProductsAvailable());
    EXPECT_EQ(storeHouse->getProduct()->getId(), expectedProductId);
}

TEST_F(StoreHouseTest, GetProductTest)
{
    auto storeHouse = std::make_unique<sd::StoreHouse>(1);

    auto product1 = std::make_unique<sd::Product>();
    auto product2 = std::make_unique<sd::Product>();
    auto product3 = std::make_unique<sd::Product>();
    
    auto expectedProduct1Id = product1->getId();
    auto expectedProduct2Id = product2->getId();
    auto expectedProduct3Id = product3->getId();

    storeHouse->moveInProduct(std::move(product1));
    storeHouse->moveInProduct(std::move(product2));
    storeHouse->moveInProduct(std::move(product3));

    EXPECT_TRUE(storeHouse->areProductsAvailable());

    EXPECT_EQ(storeHouse->getProduct(false)->getId(), expectedProduct3Id);
    EXPECT_EQ(storeHouse->getProduct(true)->getId(), expectedProduct1Id);
    EXPECT_EQ(storeHouse->getProduct(false)->getId(), expectedProduct2Id);
}
