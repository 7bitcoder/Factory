#include <iostream>
#include <thread>
#include <gtest/gtest.h>

#include "Link.hpp"
#include "LoadingRamp.hpp"
#include "Worker.hpp"
#include "StoreHouse.hpp"

class LinkTest : public ::testing::Test
{
protected:
    LinkTest() {}

    void SetUp() override
    {
    }

    void TearDown() override {}

    ~LinkTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(LinkTest, CreateTest)
{
    sd::Worker worker{3};
    sd::StoreHouse store{2};
    auto link = std::make_unique<sd::Link>(1, 0.5, worker, store);

    auto data = link->getLinkData();

    EXPECT_EQ(data.id, 1);
    EXPECT_EQ(data.probability, 0.5);
    EXPECT_EQ(data.source.id, 3);
    EXPECT_EQ(data.source.type, sd::NodeType::WORKER);
    EXPECT_EQ(data.destination.id, 2);
    EXPECT_EQ(data.destination.type, sd::NodeType::STORE);
}

TEST_F(LinkTest, CreateProbabilityClampTest)
{
    sd::Worker worker{3};
    sd::StoreHouse store{2};
    auto link = std::make_unique<sd::Link>(1, 20, worker, store);

    auto data = link->getLinkData();

    EXPECT_EQ(data.id, 1);
    EXPECT_EQ(data.probability, 1);
    EXPECT_EQ(data.source.id, 3);
    EXPECT_EQ(data.source.type, sd::NodeType::WORKER);
    EXPECT_EQ(data.destination.id, 2);
    EXPECT_EQ(data.destination.type, sd::NodeType::STORE);
}

TEST_F(LinkTest, BadCreateTest)
{
    EXPECT_THROW(
        try
        {
            sd::LoadingRamp ramp{3};
            sd::StoreHouse store{2};
            std::make_unique<sd::Link>(1, 0.5, ramp, store);
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Cannot bind Ramp and Store.", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(LinkTest, BadCreateZeroProbabilityTest)
{
    EXPECT_THROW(
        try
        {
            sd::Worker worker{3};
            sd::StoreHouse store{2};
            std::make_unique<sd::Link>(1, 0, worker, store);
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Probability Cannot be set to 0.", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(LinkTest, StructureRaportTest)
{
    sd::Worker worker{3};
    sd::StoreHouse store{2};
    auto link = std::make_unique<sd::Link>(1, 0.5, worker, store);

    EXPECT_EQ(link->getStructureRaport(0), "STOREHOUSE #2 (p = 0.50)");
}

TEST_F(LinkTest, UnbindTest)
{
    sd::Worker worker{3};
    sd::StoreHouse store{2};
    auto link = std::make_shared<sd::Link>(1, 0.5, worker, store);

    worker.bindSourceLink(link);
    store.bindDestinationLink(link);

    EXPECT_TRUE(worker.connectedSources());
    EXPECT_FALSE(worker.connectedDestinations());
    EXPECT_TRUE(store.connectedDestinations());

    link->unBindDestination();
    EXPECT_FALSE(store.connectedDestinations());

    link->unBindSource();
    EXPECT_FALSE(worker.connectedSources());
}
