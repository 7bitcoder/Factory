#include <iostream>
#include <thread>
#include <gtest/gtest.h>

#include "Factory.hpp"

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

    for(size_t cnt = 0; cnt < 100000; ++cnt) {
        ramp.process(cnt);
        worker.process(cnt);
    }
    EXPECT_TRUE(true);
}
