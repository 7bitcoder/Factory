#include <iostream>
#include <thread>
#include <fstream>
#include <gtest/gtest.h>
#include <filesystem>

#include "CommandParser.hpp"
#include "Utils.hpp"

class CommandParserTest : public ::testing::Test
{
protected:
    CommandParserTest() {}

    void SetUp() override
    {
    }

    void TearDown() override {}

    bool parse(const std::string &line, std::ostream &out, std::ostream &err)
    {
        auto args = sd::splitStr(line, ' ');
        std::vector<char *> cArgs;
        cArgs.reserve(args.size());

        for (size_t i = 0; i < args.size(); ++i)
        {
            cArgs.push_back(const_cast<char *>(args[i].c_str()));
        }

        if (!cArgs.empty())
        {
            return parser.parse(int(cArgs.size()), &cArgs[0], out, err);
        }
        return false;
    }

    sd::CommandParser parser;

    ~CommandParserTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(CommandParserTest, NormalSuccess)
{
    std::stringstream str;
    std::filesystem::path filename = "existingFile.txt";

    std::ofstream outfile(filename);
    outfile << "my text here!" << std::endl;
    outfile.close();

    EXPECT_TRUE(parse(std::format("Factory.exe -f {}", filename.string()), str, str));
    std::filesystem::remove(filename);

    EXPECT_TRUE(str.str().empty());
}

TEST_F(CommandParserTest, Fail)
{
    std::filesystem::path filename = "nonExistingFile.txt";

    std::string expected = std::format("--file: File does not exist: {}\nRun with --help for more information.\n", filename.string());
    std::stringstream str;
    EXPECT_FALSE(parse(std::format("Factory.exe -f {}", filename.string()), str, str));
    EXPECT_EQ(expected, str.str());
}

TEST_F(CommandParserTest, Help)
{
    std::string expected =

    std::stringstream str;
    EXPECT_FALSE(parse("Factory.exe -h", str, str));
    auto g = str.str();
    EXPECT_EQ(expected, str.str());
}