#include <iostream>
#include <thread>
#include <gtest/gtest.h>

#include "Utils.hpp"

class UtilsTest : public ::testing::Test
{
protected:
    UtilsTest() {}

    void SetUp() override
    {
    }

    void TearDown() override {}

    ~UtilsTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(UtilsTest, SplitStrTest)
{
    auto res = sd::splitStr("halo halo halo", ' ');

    EXPECT_EQ(res, (std::vector<std::string>{"halo", "halo", "halo"}));
}

TEST_F(UtilsTest, SplitStrSlashTest)
{
    auto res = sd::splitStr("halo/halo/halo/yes", '/');

    EXPECT_EQ(res, (std::vector<std::string>{"halo", "halo", "halo", "yes"}));
}

TEST_F(UtilsTest, GetOffsetTest)
{
    auto res = sd::getOffset(5);

    EXPECT_EQ(res, "\t\t\t\t\t");
}

TEST_F(UtilsTest, ToStringNodeTypeTest)
{
    auto ramp = sd::toString(sd::NodeType::RAMP);
    auto worker = sd::toString(sd::NodeType::WORKER);
    auto store = sd::toString(sd::NodeType::STORE);

    EXPECT_EQ(ramp, "ramp");
    EXPECT_EQ(worker, "worker");
    EXPECT_EQ(store, "store");
}

TEST_F(UtilsTest, ToStringWorkerTypeTest)
{
    auto fifo = sd::toString(sd::WorkerType::FIFO);
    auto lifo = sd::toString(sd::WorkerType::LIFO);

    EXPECT_EQ(fifo, "FIFO");
    EXPECT_EQ(lifo, "LIFO");
}
