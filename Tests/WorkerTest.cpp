#include <iostream>
#include <memory>
#include <format>
#include <gtest/gtest.h>

#include "Worker.hpp"
#include "StoreHouse.hpp"
#include "Link.hpp"

class WorkerTest : public ::testing::Test
{
protected:
    WorkerTest() {}

    void SetUp() override
    {
    }

    void TearDown() override {}

    ~WorkerTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(WorkerTest, CreateTest)
{
    auto worker = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);

    auto data = worker->getWorkerData();

    EXPECT_EQ(data.id, 1);
    EXPECT_EQ(data.type, sd::WorkerType::FIFO);
    EXPECT_EQ(data.processingTime, 3);
}

TEST_F(WorkerTest, ToStringTest)
{
    auto worker = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);

    EXPECT_EQ(worker->toString(), "WORKER #1");
}

TEST_F(WorkerTest, StructureRaportTest)
{
    auto worker = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);

    auto expected = "WORKER #1\n\tProcessing time: 3\n\tQueue type: FIFO\n\tReceivers:\n";

    EXPECT_EQ(worker->getStructureRaport(0), expected);
}

TEST_F(WorkerTest, StructureRaportWithLinksTest)
{

    auto worker1 = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);
    auto worker2 = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);
    auto worker3 = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);
    auto storeHouse = std::make_unique<sd::StoreHouse>(1);

    auto link1 = std::make_shared<sd::Link>(1, 0.5, *worker1, *worker2);
    auto link2 = std::make_shared<sd::Link>(1, 0.5, *worker1, *worker3);
    auto link3 = std::make_shared<sd::Link>(1, 0.5, *worker1, *storeHouse);

    worker1->bindSourceLink(link1);
    worker1->bindSourceLink(link2);
    worker1->bindSourceLink(link3);

    worker2->bindDestinationLink(link1);
    worker3->bindDestinationLink(link2);
    storeHouse->bindDestinationLink(link3);

    auto expected = "WORKER #1\n\tProcessing time: 3\n\tQueue type: FIFO\n\tReceivers:\n\t\tWORKER #1 (p = 0.33)\n\t\tWORKER #1 (p = 0.33)\n\t\tSTOREHOUSE #1 (p = 0.33)";

    EXPECT_EQ(worker1->getStructureRaport(0), expected);
}

TEST_F(WorkerTest, StateRaportTest)
{
    auto worker = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);

    auto expected = "WORKER #1\n\tQueue: ";

    EXPECT_EQ(worker->getStateRaport(0), expected);
}

TEST_F(WorkerTest, StateRaportWithFilledQueueTest)
{
    auto worker = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);

    auto product1 = std::make_unique<sd::Product>();
    auto product2 = std::make_unique<sd::Product>();
    auto product3 = std::make_unique<sd::Product>();

    auto expectedProduct1Id = product1->getId();
    auto expectedProduct2Id = product2->getId();
    auto expectedProduct3Id = product3->getId();

    worker->addProductToStore(std::move(product1));
    worker->addProductToStore(std::move(product2));
    worker->addProductToStore(std::move(product3));

    worker->process(0);

    auto expected = std::format("WORKER #1\n\tQueue: #{} (pt = 1), #{}, #{}", expectedProduct1Id, expectedProduct2Id, expectedProduct3Id);

    EXPECT_EQ(worker->getStateRaport(0), expected);
}

TEST_F(WorkerTest, NodeTypeTest)
{
    auto worker = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);

    EXPECT_EQ(worker->getNodeType(), sd::NodeType::WORKER);
}

TEST_F(WorkerTest, MoveInProcessTest)
{
    auto worker = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);

    auto product = std::make_unique<sd::Product>();
    auto expectedProductId = product->getId();

    worker->addProductToStore(std::move(product));

    worker->process(0);

    EXPECT_FALSE(worker->areProductsAvailable());
    EXPECT_TRUE(worker->isProcessingProduct());
}

TEST_F(WorkerTest, GetProductTest)
{
    auto worker = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);

    auto product1 = std::make_unique<sd::Product>();
    auto product2 = std::make_unique<sd::Product>();
    auto product3 = std::make_unique<sd::Product>();

    auto expectedProduct1Id = product1->getId();
    auto expectedProduct2Id = product2->getId();
    auto expectedProduct3Id = product3->getId();

    worker->addProductToStore(std::move(product1));
    worker->addProductToStore(std::move(product2));
    worker->addProductToStore(std::move(product3));

    worker->process(0);

    EXPECT_TRUE(worker->areProductsAvailable());
    EXPECT_TRUE(worker->isProcessingProduct());

    EXPECT_EQ(worker->getStoredProduct(false)->getId(), expectedProduct3Id);
    EXPECT_EQ(worker->getStoredProduct(true)->getId(), expectedProduct2Id);
}

TEST_F(WorkerTest, ProcessTest)
{
    auto worker = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);

    auto product = std::make_unique<sd::Product>();

    worker->addProductToStore(std::move(product));
    worker->process(0);
    worker->process(1);
    worker->process(2);
    EXPECT_THROW(
        try
        {
            worker->passProduct();
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("No links available", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(WorkerTest, ProcessFailTest)
{
    auto worker = std::make_unique<sd::Worker>(1, sd::WorkerType::FIFO, 3);

    auto product = std::make_unique<sd::Product>();
    auto product2 = std::make_unique<sd::Product>();

    worker->addProductToStore(std::move(product));
    worker->addProductToStore(std::move(product2));
    
    worker->process(0);
    worker->process(1);
    worker->process(2);
    worker->process(3);
    worker->process(4);
    EXPECT_THROW(
        try
        {
            worker->process(5);
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Simulation Error", e.what());
            throw;
        },
        std::runtime_error);
}