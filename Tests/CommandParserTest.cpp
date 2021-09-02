#include <iostream>
#include <thread>
#include <fstream>
#include <gtest/gtest.h>
#include <filesystem>

#include "CommandParser.hpp"
#include "Utils.hpp"

using variant = std::variant<size_t, std::vector<size_t>>;

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

TEST_F(CommandParserTest, FactoryFileArgTest)
{
    std::stringstream str;
    std::filesystem::path filename = "existingFile.txt";

    std::ofstream outfile(filename);
    outfile.close();

    EXPECT_TRUE(parse(std::format("Factory.exe -f {}", filename.string()), str, str));
    std::filesystem::remove(filename);

    EXPECT_TRUE(str.str().empty());

    auto config = parser.getResults();
    EXPECT_EQ(config.maxIterations, 100);
    EXPECT_EQ(config.raportFile, std::nullopt);
    EXPECT_EQ(config.stateRaportTimings, variant{size_t{20}});
    EXPECT_EQ(config.structureFile, filename);
}

TEST_F(CommandParserTest, FactoryFileWrongArgTest)
{
    std::stringstream str;
    std::filesystem::path filename = "nonExistingFile.txt";

    std::string expected = std::format("--file: File does not exist: {}\nRun with --help for more information.\n", filename.string());

    EXPECT_FALSE(parse(std::format("Factory.exe -f {}", filename.string()), str, str));
    EXPECT_EQ(expected, str.str());
}

TEST_F(CommandParserTest, MultipleOptionsTest)
{
    std::stringstream str;
    std::filesystem::path filename = "existingFile.txt";
    std::filesystem::path raportFile = "raportFile.txt";
    size_t maxIterations = 120;
    size_t raportInterval = 58;

    std::ofstream outfile(filename);
    outfile.close();

    EXPECT_TRUE(parse(std::format("Factory.exe -f {} -m {} -i {} -r {}", filename.string(), maxIterations, raportInterval, raportFile.string()), str, str));
    std::filesystem::remove(filename);

    EXPECT_TRUE(str.str().empty());

    auto config = parser.getResults();
    EXPECT_EQ(config.maxIterations, maxIterations);
    EXPECT_EQ(config.raportFile, raportFile);
    EXPECT_EQ(config.stateRaportTimings, variant{raportInterval});
    EXPECT_EQ(config.structureFile, filename);
}

TEST_F(CommandParserTest, OptionsExcludedTest)
{
    std::stringstream str;
    std::filesystem::path filename = "existingFile.txt";
    std::vector<size_t> raportTimes = {58, 129};
    size_t raportInterval = 58;

    std::string expected = "--raportTimings excludes --raportInterval\nRun with --help for more information.\n";

    std::ofstream outfile(filename);
    outfile.close();

    EXPECT_FALSE(parse(std::format("Factory.exe -f {} -i {} -t {} {}", filename.string(), raportInterval, raportTimes[0], raportTimes[1]), str, str));
    
    std::filesystem::remove(filename);

    EXPECT_EQ(str.str(), expected);
}
