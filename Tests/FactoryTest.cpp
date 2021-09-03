#include <iostream>
#include <thread>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Factory.hpp"
#include "Random.hpp"

bool operator==(const sd::LoadingRampData &lhs, const sd::LoadingRampData &rhs)
{
    return lhs.id == rhs.id && lhs.deliveryInterval == rhs.deliveryInterval;
}

bool operator==(const sd::WorkerData &lhs, const sd::WorkerData &rhs)
{
    return lhs.id == rhs.id && lhs.processingTime == rhs.processingTime && lhs.type == rhs.type;
}

bool operator==(const sd::StoreHouseData &lhs, const sd::StoreHouseData &rhs)
{
    return lhs.id == rhs.id;
}

bool operator==(const sd::LinkData &lhs, const sd::LinkData &rhs)
{
    return lhs.id == rhs.id && lhs.probability == rhs.probability &&
           lhs.source.id == rhs.source.id && lhs.source.type == rhs.source.type &&
           lhs.destination.id == rhs.destination.id && lhs.destination.type == rhs.destination.type;
}

class FactoryTest : public ::testing::Test
{
protected:
    FactoryTest() {}

    void SetUp() override
    {
    }

    void TearDown() override {}

    ~FactoryTest() {}

    static void TearDownTestSuite() {}
};

template <class T>
bool cmp(const std::vector<T> &left, const std::vector<T> &right)
{
    if (left.size() != right.size())
    {
        return false;
    }
    for (int i = 0; i < left.size(); ++i)
    {
        if (!(left[i] == right[i]))
        {
            return false;
        }
    }
    return true;
}

TEST_F(FactoryTest, SimpleSimulationTest)
{
    class OnlyZeroRandomDevice : public sd::IRandomDevice
    {
        double next() final { return 0; }
    };

    // it will force to move products by first link always
    sd::Random::get().updateRandomDevice(std::make_unique<OnlyZeroRandomDevice>());

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

    size_t expectedLoops = 10000;
    for (size_t cnt = 0; cnt < expectedLoops; ++cnt)
    {
        ramp.process(cnt);
        worker.process(cnt);
    }

    EXPECT_EQ(store1.getStoredProducts(), expectedLoops);
    EXPECT_EQ(store2.getStoredProducts(), 0);
    EXPECT_EQ(store3.getStoredProducts(), 0);
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
