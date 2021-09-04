#include <iostream>
#include <memory>
#include <gtest/gtest.h>

#include "LoadingRamp.hpp"
#include "Worker.hpp"

class LoadingRampTest : public ::testing::Test
{
protected:
    LoadingRampTest() {}

    void SetUp() override
    {
    }

    void TearDown() override {}

    ~LoadingRampTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(LoadingRampTest, CreateTest)
{
    auto loadingRamp = std::make_unique<sd::LoadingRamp>(1, 2);

    auto data = loadingRamp->getLoadingRampData();

    EXPECT_EQ(data.id, 1);
    EXPECT_EQ(data.deliveryInterval, 2);
}

TEST_F(LoadingRampTest, ToStringTest)
{
    auto loadingRamp = std::make_unique<sd::LoadingRamp>(1, 2);

    EXPECT_EQ(loadingRamp->toString(), "LOADING_RAMP #1");
}

TEST_F(LoadingRampTest, StructureRaportTest)
{
    auto loadingRamp = std::make_unique<sd::LoadingRamp>(1, 2);

    std::string expected = "LOADING_RAMP #1\n\tDelivery interval: 2\n\tReceivers:\n";

    EXPECT_EQ(loadingRamp->getStructureRaport(0), expected);
}

TEST_F(LoadingRampTest, StructureRaportWithLinksTest)
{

    auto loadingRamp = std::make_unique<sd::LoadingRamp>(1, 2);
    auto worker1 = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);
    auto worker2 = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);
    auto worker3 = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);

    auto link1 = std::make_shared<sd::Link>(1, 0.5, *loadingRamp, *worker1);
    auto link2 = std::make_shared<sd::Link>(1, 0.5, *loadingRamp, *worker2);
    auto link3 = std::make_shared<sd::Link>(1, 0.5, *loadingRamp, *worker3);

    loadingRamp->bindSourceLink(link1);
    loadingRamp->bindSourceLink(link2);
    loadingRamp->bindSourceLink(link3);

    worker1->bindDestinationLink(link1);
    worker2->bindDestinationLink(link2);
    worker3->bindDestinationLink(link3);

    auto expected = "LOADING_RAMP #1\n\tDelivery interval: 2\n\tReceivers:\n\t\tWORKER #1 (p = 0.33)\n\t\tWORKER #1 (p = 0.33)\n\t\tWORKER #1 (p = 0.33)";

    EXPECT_EQ(loadingRamp->getStructureRaport(0), expected);
}

TEST_F(LoadingRampTest, NodeTypeTest)
{
    auto loadingRamp = std::make_unique<sd::LoadingRamp>(1, 2);

    EXPECT_EQ(loadingRamp->getNodeType(), sd::NodeType::RAMP);
}

TEST_F(LoadingRampTest, ProcessTest)
{
    auto loadingRamp = std::make_unique<sd::LoadingRamp>(1, 2);

    loadingRamp->process(0);
    loadingRamp->process(1);
    EXPECT_THROW(
        try
        {
            loadingRamp->passProduct();
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("No links available", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(LoadingRampTest, ProcessFailTest)
{
    auto loadingRamp = std::make_unique<sd::LoadingRamp>(1, 2);

    loadingRamp->process(0);
    loadingRamp->process(1);
    loadingRamp->process(2);
    EXPECT_THROW(
        try
        {
        loadingRamp->process(3);
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Simulation Error", e.what());
            throw;
        },
        std::runtime_error);
}