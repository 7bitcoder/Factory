#include <iostream>
#include <memory>
#include <gtest/gtest.h>
#include <format>

#include "Product.hpp"

class ProductTest : public ::testing::Test
{
protected:
    ProductTest() {}

    void SetUp() override
    {
    }

    void TearDown() override {}

    ~ProductTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(ProductTest, CreateTest)
{
    auto p1 = std::make_unique<sd::Product>();
    auto p2 = std::make_unique<sd::Product>();
    auto p3 = std::make_unique<sd::Product>();

    EXPECT_EQ(p1->getId() + 1, p2->getId());
    EXPECT_EQ(p2->getId() + 1, p3->getId());
}

TEST_F(ProductTest, ToStringTest)
{
    auto p1 = std::make_unique<sd::Product>();

    EXPECT_EQ(p1->toString(), std::format("#{}", p1->getId()));
}
