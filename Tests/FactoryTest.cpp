#include <iostream>
#include <thread>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Factory.hpp"
#include "Random.hpp"
#include "TestHelpers.hpp"


class FactoryTest : public ::testing::Test
{
protected:
    FactoryTest()
    {
        sd::Random::get().updateRandomDevice(std::make_unique<RepetableRandomDevice>());
    }

    void SetUp() override
    {
    }

    void TearDown() override {}

    void tryPassProduct(sd::SourceNode &node)
    {
        if (node.isProductReady())
        {
            node.passProduct();
        }
    }

    ~FactoryTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(FactoryTest, SimpleSimulationTest)
{
    sd::LoadingRamp ramp{2};
    sd::Worker worker{3};
    sd::StoreHouse store1{2};
    sd::StoreHouse store2{1};
    sd::StoreHouse store3{1};
    auto link1 = std::make_shared<sd::Link>(1, 0.5, ramp, worker);
    auto link2 = std::make_shared<sd::Link>(1, 0.2, worker, store1);
    auto link3 = std::make_shared<sd::Link>(1, 0.5, worker, store2);
    auto link4 = std::make_shared<sd::Link>(1, 0.3, worker, store3);

    ramp.bindSourceLink(link1);
    worker.bindDestinationLink(link1);

    worker.bindSourceLink(link2);
    worker.bindSourceLink(link3);
    worker.bindSourceLink(link4);

    store1.bindDestinationLink(link2);
    store2.bindDestinationLink(link3);
    store3.bindDestinationLink(link4);

    size_t expectedLoops = 1000;
    for (size_t cnt = 0; cnt < expectedLoops; ++cnt)
    {
        ramp.process(cnt);
        tryPassProduct(ramp);
        tryPassProduct(worker);
        worker.process(cnt);
    }

    EXPECT_EQ(store1.getStoredProductsSize(), 193);
    EXPECT_EQ(store2.getStoredProductsSize(), 502);
    EXPECT_EQ(store3.getStoredProductsSize(), 304);
    EXPECT_EQ(
        (store1.getStoredProductsSize() +
         store2.getStoredProductsSize() +
         store3.getStoredProductsSize()),
        (expectedLoops - 1));
}

TEST_F(FactoryTest, SimpleSimulationDelayedTest)
{
    class OnlyZeroRandomDevice : public sd::IRandomDevice
    {
        double next() final { return 0; }
    };

    // it will force to move products by first link always
    sd::Random::get().updateRandomDevice(std::make_unique<OnlyZeroRandomDevice>());

    sd::LoadingRamp ramp{2};
    sd::Worker worker{3};
    sd::Worker worker2{4};
    sd::StoreHouse store1{2};
    auto link1 = std::make_shared<sd::Link>(1, 1, ramp, worker);
    auto link2 = std::make_shared<sd::Link>(2, 1, worker, worker2);
    auto link3 = std::make_shared<sd::Link>(3, 1, worker2, store1);

    ramp.bindSourceLink(link1);
    worker.bindDestinationLink(link1);

    worker.bindSourceLink(link2);
    worker2.bindDestinationLink(link2);

    worker2.bindSourceLink(link3);
    store1.bindDestinationLink(link3);

    size_t expectedLoops = 1000;
    for (size_t cnt = 0; cnt < expectedLoops; ++cnt)
    {
        ramp.process(cnt);
        tryPassProduct(ramp);
        tryPassProduct(worker);
        tryPassProduct(worker2);
        worker.process(cnt);
        worker2.process(cnt);
    }

    EXPECT_EQ(store1.getStoredProductsSize(), expectedLoops - 2);
}

TEST_F(FactoryTest, AddAndGetDataTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});
    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    factory.addLink({2, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({3, 0.5, {1, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({4, 0.3, {1, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});

    EXPECT_TRUE(cmp(factory.getLoadingRampsData(), {{1, 1}}));
    EXPECT_TRUE(cmp(factory.getWorkersData(), {{1, 1, sd::WorkerType::FIFO}}));
    EXPECT_TRUE(cmp(factory.getStorehousesData(), {{1}, {2}, {3}}));
    EXPECT_TRUE(cmp(factory.getLinksData(), {{1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}},
                                             {2, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}},
                                             {3, 0.5, {1, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}},
                                             {4, 0.3, {1, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}}}));
}

TEST_F(FactoryTest, RemoveLinkTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});
    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    factory.addLink({2, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({3, 0.5, {1, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({4, 0.3, {1, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});

    factory.removeLink(2);
    factory.removeLink(4);

    EXPECT_TRUE(cmp(factory.getLoadingRampsData(), {{1, 1}}));
    EXPECT_TRUE(cmp(factory.getWorkersData(), {{1, 1, sd::WorkerType::FIFO}}));
    EXPECT_TRUE(cmp(factory.getStorehousesData(), {{1}, {2}, {3}}));
    EXPECT_TRUE(cmp(factory.getLinksData(), {{1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}},
                                             {3, 0.5, {1, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}}}));
}

TEST_F(FactoryTest, RemoveLoadingRampTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addWorker({2, 1, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});
    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    factory.addLink({2, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({3, 0.5, {1, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({4, 0.3, {1, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});
    factory.addLink({5, 0.5, {1, sd::NodeType::RAMP}, {2, sd::NodeType::WORKER}});

    factory.removeLoadingRamp(1);

    EXPECT_TRUE(cmp(factory.getLoadingRampsData(), {}));
    EXPECT_TRUE(cmp(factory.getWorkersData(), {{1, 1, sd::WorkerType::FIFO}, {2, 1, sd::WorkerType::FIFO}}));
    EXPECT_TRUE(cmp(factory.getStorehousesData(), {{1}, {2}, {3}}));
    EXPECT_TRUE(cmp(factory.getLinksData(), {{2, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}},
                                             {3, 0.5, {1, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}},
                                             {4, 0.3, {1, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}}}));
}

TEST_F(FactoryTest, RemoveStoreHouseTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});
    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    factory.addLink({2, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({3, 0.5, {1, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({4, 0.3, {1, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});
    factory.addLink({5, 0.5, {1, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});

    factory.removeStorehouse(3);

    EXPECT_TRUE(cmp(factory.getLoadingRampsData(), {{1, 1}}));
    EXPECT_TRUE(cmp(factory.getWorkersData(), {{1, 1, sd::WorkerType::FIFO}}));
    EXPECT_TRUE(cmp(factory.getStorehousesData(), {{1}, {2}}));
    EXPECT_TRUE(cmp(factory.getLinksData(), {{1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}},
                                             {2, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}},
                                             {3, 0.5, {1, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}}}));
}

TEST_F(FactoryTest, RemoveWorkerTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addLoadingRamp({2, 1});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});
    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    factory.addLink({2, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({3, 0.5, {1, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({4, 0.3, {1, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});
    factory.addLink({5, 0.3, {2, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});

    factory.removeWorker(1);

    EXPECT_TRUE(cmp(factory.getLoadingRampsData(), {{1, 1}, {2, 1}}));
    EXPECT_TRUE(cmp(factory.getWorkersData(), {}));
    EXPECT_TRUE(cmp(factory.getStorehousesData(), {{1}, {2}, {3}}));
    EXPECT_TRUE(cmp(factory.getLinksData(), {}));
}

TEST_F(FactoryTest, DuplicateCreationTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    EXPECT_THROW(
        try
        {
            factory.addLoadingRamp({1, 1});
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Loading ramp of id 1 was already created.", e.what());
            throw;
        },
        std::runtime_error);

    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    EXPECT_THROW(
        try
        {
            factory.addWorker({1, 1, sd::WorkerType::FIFO});
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Worker of id 1 was already created.", e.what());
            throw;
        },
        std::runtime_error);

    factory.addStorehouse({1});
    EXPECT_THROW(
        try
        {
            factory.addStorehouse({1});
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Storehouse of id 1 was already created.", e.what());
            throw;
        },
        std::runtime_error);

    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    EXPECT_THROW(
        try
        {
            factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Link of id 1 was already created.", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(FactoryTest, CreateLinkFailTest)
{
    sd::Factory factory;

    EXPECT_THROW(
        try
        {
            factory.addLink({1, 0.5, {1, sd::NodeType::STORE}, {1, sd::NodeType::WORKER}});
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Storehouse cannot be used as link source.", e.what());
            throw;
        },
        std::runtime_error);

    EXPECT_THROW(
        try
        {
            factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Could not find LoadingRamp of id 1 to be link source.", e.what());
            throw;
        },
        std::runtime_error);

    EXPECT_THROW(
        try
        {
            factory.addLink({1, 0.5, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Could not find Worker of id 1 to be link source.", e.what());
            throw;
        },
        std::runtime_error);

    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    EXPECT_THROW(
        try
        {
            factory.addLink({1, 0.5, {1, sd::NodeType::WORKER}, {1, sd::NodeType::RAMP}});
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("LoadingRamp cannot be used as link destination.", e.what());
            throw;
        },
        std::runtime_error);

    EXPECT_THROW(
        try
        {
            factory.addLink({1, 0.5, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Could not find Storehouse of id 1 to be link destination.", e.what());
            throw;
        },
        std::runtime_error);

    EXPECT_THROW(
        try
        {
            factory.addLink({1, 0.5, {1, sd::NodeType::WORKER}, {2, sd::NodeType::WORKER}});
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Could not find Worker of id 2 to be link destination.", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(FactoryTest, InitializedTest)
{
    sd::Factory factory;

    EXPECT_FALSE(factory.initialized());

    factory.addStorehouse({1});

    EXPECT_TRUE(factory.initialized());
}

TEST_F(FactoryTest, ValidateTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addLoadingRamp({2, 2});
    factory.addLoadingRamp({3, 6});
    factory.addLoadingRamp({4, 11});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addWorker({2, 2, sd::WorkerType::FIFO});
    factory.addWorker({3, 1, sd::WorkerType::LIFO});
    factory.addWorker({4, 6, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});

    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    factory.addLink({2, 0.5, {2, sd::NodeType::RAMP}, {2, sd::NodeType::WORKER}});
    factory.addLink({3, 0.5, {2, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({4, 0.5, {3, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({5, 0.5, {4, sd::NodeType::RAMP}, {4, sd::NodeType::WORKER}});
    factory.addLink({6, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({7, 0.5, {2, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({8, 0.3, {3, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});
    factory.addLink({9, 0.3, {4, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});

    EXPECT_NO_THROW(factory.validate());
}

TEST_F(FactoryTest, ValidateFailWorkerSourceTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addLoadingRamp({2, 2});
    factory.addLoadingRamp({3, 6});
    factory.addLoadingRamp({4, 11});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addWorker({2, 2, sd::WorkerType::FIFO});
    factory.addWorker({3, 1, sd::WorkerType::LIFO});
    factory.addWorker({4, 6, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});

    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    factory.addLink({2, 0.5, {2, sd::NodeType::RAMP}, {2, sd::NodeType::WORKER}});
    factory.addLink({3, 0.5, {2, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({4, 0.5, {3, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({5, 0.5, {4, sd::NodeType::RAMP}, {4, sd::NodeType::WORKER}});
    factory.addLink({6, 0.2, {2, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({7, 0.5, {2, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({8, 0.3, {3, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});
    factory.addLink({9, 0.3, {4, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});

    EXPECT_THROW(
        try
        {
            factory.validate();
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Worker of id 1 is not connected as source.", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(FactoryTest, ValidateFailRampDestinationTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addLoadingRamp({2, 2});
    factory.addLoadingRamp({3, 6});
    factory.addLoadingRamp({4, 11});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addWorker({2, 2, sd::WorkerType::FIFO});
    factory.addWorker({3, 1, sd::WorkerType::LIFO});
    factory.addWorker({4, 6, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});

    factory.addLink({1, 0.5, {2, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    factory.addLink({2, 0.5, {2, sd::NodeType::RAMP}, {2, sd::NodeType::WORKER}});
    factory.addLink({3, 0.5, {2, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({4, 0.5, {3, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({5, 0.5, {4, sd::NodeType::RAMP}, {4, sd::NodeType::WORKER}});
    factory.addLink({6, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({7, 0.5, {2, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({8, 0.3, {3, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});
    factory.addLink({9, 0.3, {4, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});

    EXPECT_THROW(
        try
        {
            factory.validate();
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Ramp of id 1 is not connected as source.", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(FactoryTest, ValidateFailWorkerDestinationTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addLoadingRamp({2, 2});
    factory.addLoadingRamp({3, 6});
    factory.addLoadingRamp({4, 11});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addWorker({2, 2, sd::WorkerType::FIFO});
    factory.addWorker({3, 1, sd::WorkerType::LIFO});
    factory.addWorker({4, 6, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});

    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {2, sd::NodeType::WORKER}});
    factory.addLink({2, 0.5, {2, sd::NodeType::RAMP}, {2, sd::NodeType::WORKER}});
    factory.addLink({3, 0.5, {2, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({4, 0.5, {3, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({5, 0.5, {4, sd::NodeType::RAMP}, {4, sd::NodeType::WORKER}});
    factory.addLink({6, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({7, 0.5, {2, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({8, 0.3, {3, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});
    factory.addLink({9, 0.3, {4, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});

    EXPECT_THROW(
        try
        {
            factory.validate();
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Worker of id 1 is not connected as destination.", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(FactoryTest, ValidateFailStoreDestinationTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addLoadingRamp({2, 2});
    factory.addLoadingRamp({3, 6});
    factory.addLoadingRamp({4, 11});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addWorker({2, 2, sd::WorkerType::FIFO});
    factory.addWorker({3, 1, sd::WorkerType::LIFO});
    factory.addWorker({4, 6, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});

    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    factory.addLink({2, 0.5, {2, sd::NodeType::RAMP}, {2, sd::NodeType::WORKER}});
    factory.addLink({3, 0.5, {2, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({4, 0.5, {3, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({5, 0.5, {4, sd::NodeType::RAMP}, {4, sd::NodeType::WORKER}});
    factory.addLink({6, 0.2, {1, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({7, 0.5, {2, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({8, 0.3, {3, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});
    factory.addLink({9, 0.3, {4, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});

    EXPECT_THROW(
        try
        {
            factory.validate();
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("StoreHouse of id 1 is not connected as destination.", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(FactoryTest, StructureRaportTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addLoadingRamp({2, 2});
    factory.addLoadingRamp({3, 6});
    factory.addLoadingRamp({4, 11});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addWorker({2, 2, sd::WorkerType::FIFO});
    factory.addWorker({3, 1, sd::WorkerType::LIFO});
    factory.addWorker({4, 6, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});
    factory.addStorehouse({4});

    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    factory.addLink({2, 0.5, {2, sd::NodeType::RAMP}, {2, sd::NodeType::WORKER}});
    factory.addLink({3, 0.5, {2, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({4, 0.5, {3, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({5, 0.5, {4, sd::NodeType::RAMP}, {4, sd::NodeType::WORKER}});
    factory.addLink({6, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({7, 0.5, {2, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({8, 0.3, {3, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});
    factory.addLink({9, 0.3, {4, sd::NodeType::WORKER}, {4, sd::NodeType::STORE}});
    factory.addLink({10, 0.3, {4, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});

    std::string expectedRaport = "== LOADING RAMPS ==\n\nLOADING_RAMP #1\n\tDelivery interval: 1\n\tReceivers:\n\t\tWORKER #1 (p = 1.00)\n\nLOADING_RAMP #2\n\tDelivery interval: 2\n\tReceivers:\n\t\tWORKER #2 (p = 0.50)\n\t\tWORKER #3 (p = 0.50)\n\nLOADING_RAMP #3\n\tDelivery interval: 6\n\tReceivers:\n\t\tWORKER #3 (p = 1.00)\n\nLOADING_RAMP #4\n\tDelivery interval: 11\n\tReceivers:\n\t\tWORKER #4 (p = 1.00)\n\n== WORKERS ==\n\nWORKER #1\n\tProcessing time: 1\n\tQueue type: FIFO\n\tReceivers:\n\t\tSTOREHOUSE #1 (p = 1.00)\n\nWORKER #2\n\tProcessing time: 2\n\tQueue type: FIFO\n\tReceivers:\n\t\tSTOREHOUSE #2 (p = 1.00)\n\nWORKER #3\n\tProcessing time: 1\n\tQueue type: LIFO\n\tReceivers:\n\t\tSTOREHOUSE #3 (p = 1.00)\n\nWORKER #4\n\tProcessing time: 6\n\tQueue type: FIFO\n\tReceivers:\n\t\tSTOREHOUSE #4 (p = 0.50)\n\t\tSTOREHOUSE #1 (p = 0.50)\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\nSTOREHOUSE #2\n\nSTOREHOUSE #3\n\nSTOREHOUSE #4\n\n";

    EXPECT_EQ(factory.generateStructureRaport(), expectedRaport);
}

TEST_F(FactoryTest, RunBasicSimulationTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addLoadingRamp({2, 2});
    factory.addLoadingRamp({3, 6});
    factory.addLoadingRamp({4, 11});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addWorker({2, 2, sd::WorkerType::FIFO});
    factory.addWorker({3, 1, sd::WorkerType::LIFO});
    factory.addWorker({4, 6, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});
    factory.addStorehouse({4});

    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    factory.addLink({2, 0.5, {2, sd::NodeType::RAMP}, {2, sd::NodeType::WORKER}});
    factory.addLink({3, 0.5, {2, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({4, 0.5, {3, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({5, 0.5, {4, sd::NodeType::RAMP}, {4, sd::NodeType::WORKER}});
    factory.addLink({6, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({7, 0.5, {2, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({8, 0.3, {3, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});
    factory.addLink({9, 0.3, {4, sd::NodeType::WORKER}, {4, sd::NodeType::STORE}});
    factory.addLink({10, 0.3, {4, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});

    std::stringstream out;

    sd::Factory::RaportGuard guard{size_t{1}};
    factory.run(20, out, guard);

    std::string expectedOut = "========= Factory Structure ========\n== LOADING RAMPS ==\n\nLOADING_RAMP #1\n\tDelivery interval: 1\n\tReceivers:\n\t\tWORKER #1 (p = 1.00)\n\nLOADING_RAMP #2\n\tDelivery interval: 2\n\tReceivers:\n\t\tWORKER #2 (p = 0.50)\n\t\tWORKER #3 (p = 0.50)\n\nLOADING_RAMP #3\n\tDelivery interval: 6\n\tReceivers:\n\t\tWORKER #3 (p = 1.00)\n\nLOADING_RAMP #4\n\tDelivery interval: 11\n\tReceivers:\n\t\tWORKER #4 (p = 1.00)\n\n== WORKERS ==\n\nWORKER #1\n\tProcessing time: 1\n\tQueue type: FIFO\n\tReceivers:\n\t\tSTOREHOUSE #1 (p = 1.00)\n\nWORKER #2\n\tProcessing time: 2\n\tQueue type: FIFO\n\tReceivers:\n\t\tSTOREHOUSE #2 (p = 1.00)\n\nWORKER #3\n\tProcessing time: 1\n\tQueue type: LIFO\n\tReceivers:\n\t\tSTOREHOUSE #3 (p = 1.00)\n\nWORKER #4\n\tProcessing time: 6\n\tQueue type: FIFO\n\tReceivers:\n\t\tSTOREHOUSE #4 (p = 0.50)\n\t\tSTOREHOUSE #1 (p = 0.50)\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\nSTOREHOUSE #2\n\nSTOREHOUSE #3\n\nSTOREHOUSE #4\n\n\n========= Simulation Start =========\n========= Iteration: 0 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: \n\nSTOREHOUSE #2\n\tQueue: \n\nSTOREHOUSE #3\n\tQueue: \n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 1 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0\n\nSTOREHOUSE #2\n\tQueue: \n\nSTOREHOUSE #3\n\tQueue: \n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 2 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1\n\nSTOREHOUSE #2\n\tQueue: \n\nSTOREHOUSE #3\n\tQueue: #2\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 3 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3\n\nSTOREHOUSE #2\n\tQueue: \n\nSTOREHOUSE #3\n\tQueue: #2\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 4 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4\n\nSTOREHOUSE #2\n\tQueue: \n\nSTOREHOUSE #3\n\tQueue: #2, #5\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 5 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: #8 (pt = 1), \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6\n\nSTOREHOUSE #2\n\tQueue: \n\nSTOREHOUSE #3\n\tQueue: #2, #5\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 6 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7\n\nSTOREHOUSE #2\n\tQueue: \n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 7 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: #12 (pt = 1), \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10\n\nSTOREHOUSE #2\n\tQueue: #8\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 8 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10, #11\n\nSTOREHOUSE #2\n\tQueue: #8\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 9 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: #15 (pt = 1), \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10, #11, #13\n\nSTOREHOUSE #2\n\tQueue: #8, #12\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 10 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: #17 (pt = 1), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10, #11, #13, #14\n\nSTOREHOUSE #2\n\tQueue: #8, #12\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 11 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: #19 (pt = 0), \n\nWORKER #4\n\tQueue: #17 (pt = 2), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10, #11, #13, #14, #16\n\nSTOREHOUSE #2\n\tQueue: #8, #12, #15\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 12 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: #17 (pt = 3), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10, #11, #13, #14, #16, #18\n\nSTOREHOUSE #2\n\tQueue: #8, #12, #15\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9, #20\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 13 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: #23 (pt = 1), \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: #17 (pt = 4), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10, #11, #13, #14, #16, #18, #21\n\nSTOREHOUSE #2\n\tQueue: #8, #12, #15\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9, #20, #19\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 14 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: #17 (pt = 5), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10, #11, #13, #14, #16, #18, #21, #22\n\nSTOREHOUSE #2\n\tQueue: #8, #12, #15\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9, #20, #19\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 15 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10, #11, #13, #14, #16, #18, #21, #22, #24\n\nSTOREHOUSE #2\n\tQueue: #8, #12, #15, #23\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9, #20, #19\n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 16 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10, #11, #13, #14, #16, #18, #21, #22, #24, #25\n\nSTOREHOUSE #2\n\tQueue: #8, #12, #15, #23\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9, #20, #19, #26\n\nSTOREHOUSE #4\n\tQueue: #17\n\n========= Iteration: 17 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: #29 (pt = 0), \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10, #11, #13, #14, #16, #18, #21, #22, #24, #25, #27\n\nSTOREHOUSE #2\n\tQueue: #8, #12, #15, #23\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9, #20, #19, #26\n\nSTOREHOUSE #4\n\tQueue: #17\n\n========= Iteration: 18 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10, #11, #13, #14, #16, #18, #21, #22, #24, #25, #27, #28\n\nSTOREHOUSE #2\n\tQueue: #8, #12, #15, #23\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9, #20, #19, #26, #30\n\nSTOREHOUSE #4\n\tQueue: #17\n\n========= Iteration: 19 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: #33 (pt = 1), \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #1, #3, #4, #6, #7, #10, #11, #13, #14, #16, #18, #21, #22, #24, #25, #27, #28, #31\n\nSTOREHOUSE #2\n\tQueue: #8, #12, #15, #23\n\nSTOREHOUSE #3\n\tQueue: #2, #5, #9, #20, #19, #26, #30, #29\n\nSTOREHOUSE #4\n\tQueue: #17\n\n";
    EXPECT_EQ(out.str(), expectedOut);
}

TEST_F(FactoryTest, RunComplexSimulationTest)
{
    sd::Factory factory;

    factory.addLoadingRamp({1, 1});
    factory.addLoadingRamp({2, 2});
    factory.addLoadingRamp({3, 3});
    factory.addLoadingRamp({4, 2});
    factory.addWorker({1, 1, sd::WorkerType::FIFO});
    factory.addWorker({2, 2, sd::WorkerType::FIFO});
    factory.addWorker({3, 1, sd::WorkerType::LIFO});
    factory.addWorker({4, 2, sd::WorkerType::FIFO});
    factory.addStorehouse({1});
    factory.addStorehouse({2});
    factory.addStorehouse({3});
    factory.addStorehouse({4});

    factory.addLink({1, 0.5, {1, sd::NodeType::RAMP}, {1, sd::NodeType::WORKER}});
    factory.addLink({2, 0.5, {2, sd::NodeType::RAMP}, {2, sd::NodeType::WORKER}});
    factory.addLink({3, 0.5, {2, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({4, 0.5, {3, sd::NodeType::RAMP}, {3, sd::NodeType::WORKER}});
    factory.addLink({5, 0.5, {4, sd::NodeType::RAMP}, {4, sd::NodeType::WORKER}});
    factory.addLink({6, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({7, 0.2, {1, sd::NodeType::WORKER}, {1, sd::NodeType::WORKER}});
    factory.addLink({8, 0.2, {1, sd::NodeType::WORKER}, {2, sd::NodeType::WORKER}});
    factory.addLink({9, 0.5, {2, sd::NodeType::WORKER}, {2, sd::NodeType::WORKER}});
    factory.addLink({10, 0.5, {2, sd::NodeType::WORKER}, {2, sd::NodeType::WORKER}});
    factory.addLink({11, 0.5, {2, sd::NodeType::WORKER}, {3, sd::NodeType::WORKER}});
    factory.addLink({12, 0.3, {3, sd::NodeType::WORKER}, {3, sd::NodeType::WORKER}});
    factory.addLink({13, 0.3, {3, sd::NodeType::WORKER}, {4, sd::NodeType::WORKER}});
    factory.addLink({14, 0.3, {3, sd::NodeType::WORKER}, {3, sd::NodeType::STORE}});
    factory.addLink({15, 0.3, {3, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});
    factory.addLink({16, 0.3, {3, sd::NodeType::WORKER}, {2, sd::NodeType::STORE}});
    factory.addLink({17, 0.3, {4, sd::NodeType::WORKER}, {4, sd::NodeType::STORE}});
    factory.addLink({18, 0.3, {4, sd::NodeType::WORKER}, {1, sd::NodeType::STORE}});

    std::stringstream out;

    sd::Factory::RaportGuard guard{size_t{1}};
    factory.run(20, out, guard);

    std::string expectedOut = "========= Factory Structure ========\n== LOADING RAMPS ==\n\nLOADING_RAMP #1\n\tDelivery interval: 1\n\tReceivers:\n\t\tWORKER #1 (p = 1.00)\n\nLOADING_RAMP #2\n\tDelivery interval: 2\n\tReceivers:\n\t\tWORKER #2 (p = 0.50)\n\t\tWORKER #3 (p = 0.50)\n\nLOADING_RAMP #3\n\tDelivery interval: 3\n\tReceivers:\n\t\tWORKER #3 (p = 1.00)\n\nLOADING_RAMP #4\n\tDelivery interval: 2\n\tReceivers:\n\t\tWORKER #4 (p = 1.00)\n\n== WORKERS ==\n\nWORKER #1\n\tProcessing time: 1\n\tQueue type: FIFO\n\tReceivers:\n\t\tSTOREHOUSE #1 (p = 0.33)\n\t\tWORKER #1 (p = 0.33)\n\t\tWORKER #2 (p = 0.33)\n\nWORKER #2\n\tProcessing time: 2\n\tQueue type: FIFO\n\tReceivers:\n\t\tWORKER #2 (p = 0.33)\n\t\tWORKER #2 (p = 0.33)\n\t\tWORKER #3 (p = 0.33)\n\nWORKER #3\n\tProcessing time: 1\n\tQueue type: LIFO\n\tReceivers:\n\t\tWORKER #3 (p = 0.20)\n\t\tWORKER #4 (p = 0.20)\n\t\tSTOREHOUSE #3 (p = 0.20)\n\t\tSTOREHOUSE #1 (p = 0.20)\n\t\tSTOREHOUSE #2 (p = 0.20)\n\nWORKER #4\n\tProcessing time: 2\n\tQueue type: FIFO\n\tReceivers:\n\t\tSTOREHOUSE #4 (p = 0.50)\n\t\tSTOREHOUSE #1 (p = 0.50)\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\nSTOREHOUSE #2\n\nSTOREHOUSE #3\n\nSTOREHOUSE #4\n\n\n========= Simulation Start =========\n========= Iteration: 0 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: \n\nSTOREHOUSE #2\n\tQueue: \n\nSTOREHOUSE #3\n\tQueue: \n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 1 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: #3 (pt = 1), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0\n\nSTOREHOUSE #2\n\tQueue: \n\nSTOREHOUSE #3\n\tQueue: \n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 2 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: #1 (pt = 1), \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0\n\nSTOREHOUSE #2\n\tQueue: #2\n\nSTOREHOUSE #3\n\tQueue: \n\nSTOREHOUSE #4\n\tQueue: \n\n========= Iteration: 3 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: #8 (pt = 1), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5\n\nSTOREHOUSE #2\n\tQueue: #2\n\nSTOREHOUSE #3\n\tQueue: \n\nSTOREHOUSE #4\n\tQueue: #3\n\n========= Iteration: 4 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: #1 (pt = 1), \n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6\n\nSTOREHOUSE #2\n\tQueue: #2, #7\n\nSTOREHOUSE #3\n\tQueue: \n\nSTOREHOUSE #4\n\tQueue: #3\n\n========= Iteration: 5 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: #9 (pt = 0), \n\nWORKER #3\n\tQueue: #11 (pt = 0), \n\nWORKER #4\n\tQueue: #13 (pt = 1), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6\n\nSTOREHOUSE #2\n\tQueue: #2, #7\n\nSTOREHOUSE #3\n\tQueue: \n\nSTOREHOUSE #4\n\tQueue: #3, #8\n\n========= Iteration: 6 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #10 (pt = 0), \n\nWORKER #2\n\tQueue: #9 (pt = 1), #1\n\nWORKER #3\n\tQueue: #12 (pt = 0), \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6\n\nSTOREHOUSE #2\n\tQueue: #2, #7\n\nSTOREHOUSE #3\n\tQueue: \n\nSTOREHOUSE #4\n\tQueue: #3, #8\n\n========= Iteration: 7 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #15 (pt = 0), \n\nWORKER #2\n\tQueue: #1 (pt = 0), #14\n\nWORKER #3\n\tQueue: #11 (pt = 0), #16\n\nWORKER #4\n\tQueue: #17 (pt = 1), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13\n\nSTOREHOUSE #2\n\tQueue: #2, #7\n\nSTOREHOUSE #3\n\tQueue: \n\nSTOREHOUSE #4\n\tQueue: #3, #8\n\n========= Iteration: 8 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #18 (pt = 0), \n\nWORKER #2\n\tQueue: #1 (pt = 1), #14, #10, #9\n\nWORKER #3\n\tQueue: #19 (pt = 0), #16\n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13, #12\n\nSTOREHOUSE #2\n\tQueue: #2, #7\n\nSTOREHOUSE #3\n\tQueue: \n\nSTOREHOUSE #4\n\tQueue: #3, #8\n\n========= Iteration: 9 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #20 (pt = 0), \n\nWORKER #2\n\tQueue: #14 (pt = 0), #10, #9, #21\n\nWORKER #3\n\tQueue: #16 (pt = 0), \n\nWORKER #4\n\tQueue: #22 (pt = 1), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13, #12, #15\n\nSTOREHOUSE #2\n\tQueue: #2, #7\n\nSTOREHOUSE #3\n\tQueue: #11\n\nSTOREHOUSE #4\n\tQueue: #3, #8, #17\n\n========= Iteration: 10 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #23 (pt = 0), \n\nWORKER #2\n\tQueue: #14 (pt = 1), #10, #9, #21, #18\n\nWORKER #3\n\tQueue: #1 (pt = 0), \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13, #12, #15\n\nSTOREHOUSE #2\n\tQueue: #2, #7, #19\n\nSTOREHOUSE #3\n\tQueue: #11\n\nSTOREHOUSE #4\n\tQueue: #3, #8, #17\n\n========= Iteration: 11 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #24 (pt = 0), #20\n\nWORKER #2\n\tQueue: #10 (pt = 0), #9, #21, #18, #25\n\nWORKER #3\n\tQueue: #26 (pt = 0), \n\nWORKER #4\n\tQueue: #27 (pt = 1), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13, #12, #15, #16, #22\n\nSTOREHOUSE #2\n\tQueue: #2, #7, #19\n\nSTOREHOUSE #3\n\tQueue: #11\n\nSTOREHOUSE #4\n\tQueue: #3, #8, #17\n\n========= Iteration: 12 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #20 (pt = 0), #28\n\nWORKER #2\n\tQueue: #10 (pt = 1), #9, #21, #18, #25, #23, #14\n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13, #12, #15, #16, #22\n\nSTOREHOUSE #2\n\tQueue: #2, #7, #19, #1\n\nSTOREHOUSE #3\n\tQueue: #11\n\nSTOREHOUSE #4\n\tQueue: #3, #8, #17\n\n========= Iteration: 13 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #28 (pt = 0), #29, #24\n\nWORKER #2\n\tQueue: #9 (pt = 0), #21, #18, #25, #23, #14\n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: #31 (pt = 1), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13, #12, #15, #16, #22\n\nSTOREHOUSE #2\n\tQueue: #2, #7, #19, #1\n\nSTOREHOUSE #3\n\tQueue: #11, #26\n\nSTOREHOUSE #4\n\tQueue: #3, #8, #17, #27\n\n========= Iteration: 14 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #29 (pt = 0), #24, #32\n\nWORKER #2\n\tQueue: #9 (pt = 1), #21, #18, #25, #23, #14, #20\n\nWORKER #3\n\tQueue: #33 (pt = 0), \n\nWORKER #4\n\tQueue: #30 (pt = 0), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13, #12, #15, #16, #22\n\nSTOREHOUSE #2\n\tQueue: #2, #7, #19, #1\n\nSTOREHOUSE #3\n\tQueue: #11, #26\n\nSTOREHOUSE #4\n\tQueue: #3, #8, #17, #27\n\n========= Iteration: 15 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #24 (pt = 0), #32, #34\n\nWORKER #2\n\tQueue: #21 (pt = 0), #18, #25, #23, #14, #20, #35, #28\n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: #30 (pt = 1), #36\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13, #12, #15, #16, #22, #31\n\nSTOREHOUSE #2\n\tQueue: #2, #7, #19, #1, #10\n\nSTOREHOUSE #3\n\tQueue: #11, #26\n\nSTOREHOUSE #4\n\tQueue: #3, #8, #17, #27\n\n========= Iteration: 16 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #32 (pt = 0), #34, #37\n\nWORKER #2\n\tQueue: #21 (pt = 1), #18, #25, #23, #14, #20, #35, #28, #29, #9\n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: #36 (pt = 0), \n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13, #12, #15, #16, #22, #31, #33\n\nSTOREHOUSE #2\n\tQueue: #2, #7, #19, #1, #10\n\nSTOREHOUSE #3\n\tQueue: #11, #26\n\nSTOREHOUSE #4\n\tQueue: #3, #8, #17, #27\n\n========= Iteration: 17 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #34 (pt = 0), #37, #38, #24\n\nWORKER #2\n\tQueue: #18 (pt = 0), #25, #23, #14, #20, #35, #28, #29, #9\n\nWORKER #3\n\tQueue: #39 (pt = 0), \n\nWORKER #4\n\tQueue: #36 (pt = 1), #41\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13, #12, #15, #16, #22, #31, #33, #30\n\nSTOREHOUSE #2\n\tQueue: #2, #7, #19, #1, #10\n\nSTOREHOUSE #3\n\tQueue: #11, #26\n\nSTOREHOUSE #4\n\tQueue: #3, #8, #17, #27\n\n========= Iteration: 18 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #37 (pt = 0), #38, #24, #42, #32\n\nWORKER #2\n\tQueue: #18 (pt = 1), #25, #23, #14, #20, #35, #28, #29, #9, #21\n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: #41 (pt = 0), #40\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13, #12, #15, #16, #22, #31, #33, #30\n\nSTOREHOUSE #2\n\tQueue: #2, #7, #19, #1, #10\n\nSTOREHOUSE #3\n\tQueue: #11, #26\n\nSTOREHOUSE #4\n\tQueue: #3, #8, #17, #27\n\n========= Iteration: 19 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #38 (pt = 0), #24, #42, #32, #43\n\nWORKER #2\n\tQueue: #25 (pt = 0), #23, #14, #20, #35, #28, #29, #9, #21, #44\n\nWORKER #3\n\tQueue: \n\nWORKER #4\n\tQueue: #41 (pt = 1), #40, #45, #39\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #0, #4, #5, #6, #13, #12, #15, #16, #22, #31, #33, #30, #34\n\nSTOREHOUSE #2\n\tQueue: #2, #7, #19, #1, #10\n\nSTOREHOUSE #3\n\tQueue: #11, #26\n\nSTOREHOUSE #4\n\tQueue: #3, #8, #17, #27, #36\n\n";
    EXPECT_EQ(out.str(), expectedOut);
}
