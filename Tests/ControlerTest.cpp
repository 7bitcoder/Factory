#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>


#include "Controler.hpp"
#include "Random.hpp"
#include "TestHelpers.hpp"

class ControlerTest : public ::testing::Test
{
  protected:
    ControlerTest()
    {
        sd::Random::get().updateRandomDevice(std::make_unique<RepetableRandomDevice>());
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    void tryPassProduct(sd::SourceNode &node)
    {
        if (node.isProductReady())
        {
            node.passProduct();
        }
    }

    ~ControlerTest()
    {
    }

    static void TearDownTestSuite()
    {
    }
};

TEST_F(ControlerTest, RunBasicSimulationTest)
{
    std::filesystem::path filename = "existingFile.txt";

    std::ofstream outfile(filename);
    outfile
        << "; == LOADING RAMPS ==\n\nLOADING_RAMP id=1 delivery-interval=3\n\nLOADING_RAMP id=2 "
           "delivery-interval=2\n\n; == WORKERS ==\n\nWORKER id=1 processing-time=2 queue-type=FIFO\n\nWORKER id=2 "
           "processing-time=1 queue-type=FIFO\n\nWORKER id=22 processing-time=10 queue-type=FIFO\n\n; == STOREHOUSES "
           "==\n\nSTOREHOUSE id=1\n\n; == LINKS ==\n\nLINK id=1 src=ramp-1 dest=worker-1 p=1\n\nLINK id=2 src=ramp-2 "
           "dest=worker-1 p=0.3\n\nLINK id=3 src=ramp-2 dest=worker-2 p=0.7\n\nLINK id=4 src=worker-1 dest=worker-1 "
           "p=0.5\n\nLINK id=5 src=worker-1 dest=worker-2 p=0.5\n\nLINK id=6 src=worker-2 dest=store-1 p=1\n\nLINK "
           "id=7 src=worker-22 dest=store-1 p=1\n\nLINK id=8 src=ramp-1 dest=worker-22 p=1\n\n";
    outfile.close();

    std::stringstream in;
    std::stringstream out;

    in << "run\n";

    sd::Configuration conf;
    conf.structureFile = filename.string();
    conf.maxIterations = 300;
    conf.stateRaportTimings = size_t{50};

    sd::Controler con{conf, out, out, in};

    con.run();

    std::string expectedOut =
        " ============================ FACTORY SIMULATOR v 0.1 ============================ \n>>  "
        "============================== STARTING SIMULATION ============================== \n========= Factory "
        "Structure ========\n== LOADING RAMPS ==\n\nLOADING_RAMP #1\n\tDelivery interval: 3\n\tReceivers:\n\t\tWORKER "
        "#1 (p = 0.50)\n\t\tWORKER #22 (p = 0.50)\n\nLOADING_RAMP #2\n\tDelivery interval: 2\n\tReceivers:\n\t\tWORKER "
        "#1 (p = 0.30)\n\t\tWORKER #2 (p = 0.70)\n\n== WORKERS ==\n\nWORKER #1\n\tProcessing time: 2\n\tQueue type: "
        "FIFO\n\tReceivers:\n\t\tWORKER #1 (p = 0.50)\n\t\tWORKER #2 (p = 0.50)\n\nWORKER #2\n\tProcessing time: "
        "1\n\tQueue type: FIFO\n\tReceivers:\n\t\tSTOREHOUSE #1 (p = 1.00)\n\nWORKER #22\n\tProcessing time: "
        "10\n\tQueue type: FIFO\n\tReceivers:\n\t\tSTOREHOUSE #1 (p = 1.00)\n\n== STOREHOUSES ==\n\nSTOREHOUSE "
        "#1\n\n\n========= Simulation Start =========\n========= Iteration: 0 =========\n== WORKERS ==\n\nWORKER "
        "#1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #22\n\tQueue: \n\n== STOREHOUSES ==\n\nSTOREHOUSE "
        "#1\n\tQueue: \n\n========= Iteration: 50 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #33 (pt = 0), #27, "
        "#39, #32\n\nWORKER #2\n\tQueue: \n\nWORKER #22\n\tQueue: #26 (pt = 9), #31, #36, #38, #41\n\n== STOREHOUSES "
        "==\n\nSTOREHOUSE #1\n\tQueue: #2, #0, #5, #4, #7, #9, #1, #10, #12, #8, #15, #13, #17, #3, #22, #14, #24, "
        "#25, #6, #19, #11, #29, #20, #30, #34, #23, #18, #35, #37, #28, #16, #40\n\n========= Iteration: 100 "
        "=========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #62 (pt = 0), #76, #58, #70, #81, #65\n\nWORKER #2\n\tQueue: "
        "\n\nWORKER #22\n\tQueue: #43 (pt = 9), #46, #48, #51, #53, #56, #63, #66, #71, #73, #78\n\n== STOREHOUSES "
        "==\n\nSTOREHOUSE #1\n\tQueue: #2, #0, #5, #4, #7, #9, #1, #10, #12, #8, #15, #13, #17, #3, #22, #14, #24, "
        "#25, #6, #19, #11, #29, #20, #30, #34, #23, #18, #35, #37, #28, #16, #40, #42, #26, #44, #33, #47, #39, #49, "
        "#32, #50, #31, #45, #54, #27, #55, #21, #52, #59, #36, #57, #60, #64, #61, #38, #72, #67, #75, #41, #68, #77, "
        "#69, #79, #80, #82\n\n========= Iteration: 150 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #58 (pt = 0), "
        "#105, #106, #107, #91, #108, #81, #110, #112, #113, #114, #96, #62, #117, #98, #118, #101, #121, "
        "#122\n\nWORKER #2\n\tQueue: \n\nWORKER #22\n\tQueue: #56 (pt = 9), #63, #66, #71, #73, #78, #86, #111, #116, "
        "#123\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #2, #0, #5, #4, #7, #9, #1, #10, #12, #8, #15, #13, #17, "
        "#3, #22, #14, #24, #25, #6, #19, #11, #29, #20, #30, #34, #23, #18, #35, #37, #28, #16, #40, #42, #26, #44, "
        "#33, #47, #39, #49, #32, #50, #31, #45, #54, #27, #55, #21, #52, #59, #36, #57, #60, #64, #61, #38, #72, #67, "
        "#75, #41, #68, #77, #69, #79, #80, #82, #84, #43, #74, #85, #89, #70, #92, #46, #65, #95, #83, #97, #99, "
        "#100, #48, #76, #102, #90, #109, #51, #93, #94, #115, #53, #119, #87, #120, #88, #124\n\n========= Iteration: "
        "200 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #134 (pt = 0), #135, #138, #139, #140, #113, #143, #114, "
        "#96, #148, #98, #151, #153, #154, #101, #156, #157, #122, #58, #106, #132, #166\n\nWORKER #2\n\tQueue: "
        "\n\nWORKER #22\n\tQueue: #78 (pt = 9), #86, #111, #116, #123, #128, #131, #133, #136, #141, #146, #158, #161, "
        "#163\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #2, #0, #5, #4, #7, #9, #1, #10, #12, #8, #15, #13, #17, "
        "#3, #22, #14, #24, #25, #6, #19, #11, #29, #20, #30, #34, #23, #18, #35, #37, #28, #16, #40, #42, #26, #44, "
        "#33, #47, #39, #49, #32, #50, #31, #45, #54, #27, #55, #21, #52, #59, #36, #57, #60, #64, #61, #38, #72, #67, "
        "#75, #41, #68, #77, #69, #79, #80, #82, #84, #43, #74, #85, #89, #70, #92, #46, #65, #95, #83, #97, #99, "
        "#100, #48, #76, #102, #90, #109, #51, #93, #94, #115, #53, #119, #87, #120, #88, #124, #103, #125, #56, #104, "
        "#127, #129, #105, #91, #63, #108, #137, #81, #110, #112, #142, #66, #144, #145, #147, #62, #149, #117, #150, "
        "#71, #152, #118, #155, #121, #159, #73, #126, #160, #162, #130, #164, #165\n\n========= Iteration: 250 "
        "=========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #113 (pt = 0), #178, #143, #180, #96, #183, #148, #98, #190, "
        "#154, #191, #192, #101, #193, #156, #157, #198, #132, #203, #204, #166, #207, #168\n\nWORKER #2\n\tQueue: "
        "\n\nWORKER #22\n\tQueue: #128 (pt = 9), #131, #133, #136, #141, #146, #158, #161, #163, #171, #173, #176, "
        "#181, #186, #188, #196, #201, #206\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #2, #0, #5, #4, #7, #9, "
        "#1, #10, #12, #8, #15, #13, #17, #3, #22, #14, #24, #25, #6, #19, #11, #29, #20, #30, #34, #23, #18, #35, "
        "#37, #28, #16, #40, #42, #26, #44, #33, #47, #39, #49, #32, #50, #31, #45, #54, #27, #55, #21, #52, #59, #36, "
        "#57, #60, #64, #61, #38, #72, #67, #75, #41, #68, #77, #69, #79, #80, #82, #84, #43, #74, #85, #89, #70, #92, "
        "#46, #65, #95, #83, #97, #99, #100, #48, #76, #102, #90, #109, #51, #93, #94, #115, #53, #119, #87, #120, "
        "#88, #124, #103, #125, #56, #104, #127, #129, #105, #91, #63, #108, #137, #81, #110, #112, #142, #66, #144, "
        "#145, #147, #62, #149, #117, #150, #71, #152, #118, #155, #121, #159, #73, #126, #160, #162, #130, #164, "
        "#165, #167, #78, #169, #170, #135, #172, #138, #174, #139, #175, #86, #140, #177, #179, #114, #182, #184, "
        "#111, #185, #187, #151, #189, #153, #116, #194, #195, #197, #122, #199, #58, #200, #123, #106, #202, #205, "
        "#107\n\n ================================ SIMULATION ENDED =============================== \n";
    EXPECT_EQ(out.str(), expectedOut);

    std::filesystem::remove(filename);
}

TEST_F(ControlerTest, SaveRaportInFileSimulationTest)
{
    std::filesystem::path filename = "existingFile.txt";
    std::filesystem::path raportFilename = "reportFile.txt";

    std::ofstream outfile(filename);
    outfile
        << "; == LOADING RAMPS ==\n\nLOADING_RAMP id=1 delivery-interval=3\n\nLOADING_RAMP id=2 "
           "delivery-interval=2\n\n; == WORKERS ==\n\nWORKER id=1 processing-time=2 queue-type=FIFO\n\nWORKER id=2 "
           "processing-time=1 queue-type=FIFO\n\nWORKER id=22 processing-time=10 queue-type=FIFO\n\n; == STOREHOUSES "
           "==\n\nSTOREHOUSE id=1\n\n; == LINKS ==\n\nLINK id=1 src=ramp-1 dest=worker-1 p=1\n\nLINK id=2 src=ramp-2 "
           "dest=worker-1 p=0.3\n\nLINK id=3 src=ramp-2 dest=worker-2 p=0.7\n\nLINK id=4 src=worker-1 dest=worker-1 "
           "p=0.5\n\nLINK id=5 src=worker-1 dest=worker-2 p=0.5\n\nLINK id=6 src=worker-2 dest=store-1 p=1\n\nLINK "
           "id=7 src=worker-22 dest=store-1 p=1\n\nLINK id=8 src=ramp-1 dest=worker-22 p=1\n\n";
    outfile.close();

    std::stringstream in;
    std::stringstream out;

    in << "run\n";

    sd::Configuration conf;
    conf.structureFile = filename.string();
    conf.maxIterations = 300;
    conf.stateRaportTimings = size_t{50};
    conf.raportFile = raportFilename.string();

    sd::Controler con{conf, out, out, in};

    con.run();

    std::string expectedOut =
        "========= Factory Structure ========\n== LOADING RAMPS ==\n\nLOADING_RAMP #1\n\tDelivery interval: "
        "3\n\tReceivers:\n\t\tWORKER #1 (p = 0.50)\n\t\tWORKER #22 (p = 0.50)\n\nLOADING_RAMP #2\n\tDelivery interval: "
        "2\n\tReceivers:\n\t\tWORKER #1 (p = 0.30)\n\t\tWORKER #2 (p = 0.70)\n\n== WORKERS ==\n\nWORKER "
        "#1\n\tProcessing time: 2\n\tQueue type: FIFO\n\tReceivers:\n\t\tWORKER #1 (p = 0.50)\n\t\tWORKER #2 (p = "
        "0.50)\n\nWORKER #2\n\tProcessing time: 1\n\tQueue type: FIFO\n\tReceivers:\n\t\tSTOREHOUSE #1 (p = "
        "1.00)\n\nWORKER #22\n\tProcessing time: 10\n\tQueue type: FIFO\n\tReceivers:\n\t\tSTOREHOUSE #1 (p = "
        "1.00)\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\n\n========= Simulation Start =========\n========= Iteration: 0 "
        "=========\n== WORKERS ==\n\nWORKER #1\n\tQueue: \n\nWORKER #2\n\tQueue: \n\nWORKER #22\n\tQueue: \n\n== "
        "STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: \n\n========= Iteration: 50 =========\n== WORKERS ==\n\nWORKER "
        "#1\n\tQueue: #33 (pt = 0), #27, #39, #32\n\nWORKER #2\n\tQueue: \n\nWORKER #22\n\tQueue: #26 (pt = 9), #31, "
        "#36, #38, #41\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #2, #0, #5, #4, #7, #9, #1, #10, #12, #8, #15, "
        "#13, #17, #3, #22, #14, #24, #25, #6, #19, #11, #29, #20, #30, #34, #23, #18, #35, #37, #28, #16, "
        "#40\n\n========= Iteration: 100 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #62 (pt = 0), #76, #58, #70, "
        "#81, #65\n\nWORKER #2\n\tQueue: \n\nWORKER #22\n\tQueue: #43 (pt = 9), #46, #48, #51, #53, #56, #63, #66, "
        "#71, #73, #78\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #2, #0, #5, #4, #7, #9, #1, #10, #12, #8, #15, "
        "#13, #17, #3, #22, #14, #24, #25, #6, #19, #11, #29, #20, #30, #34, #23, #18, #35, #37, #28, #16, #40, #42, "
        "#26, #44, #33, #47, #39, #49, #32, #50, #31, #45, #54, #27, #55, #21, #52, #59, #36, #57, #60, #64, #61, #38, "
        "#72, #67, #75, #41, #68, #77, #69, #79, #80, #82\n\n========= Iteration: 150 =========\n== WORKERS "
        "==\n\nWORKER #1\n\tQueue: #58 (pt = 0), #105, #106, #107, #91, #108, #81, #110, #112, #113, #114, #96, #62, "
        "#117, #98, #118, #101, #121, #122\n\nWORKER #2\n\tQueue: \n\nWORKER #22\n\tQueue: #56 (pt = 9), #63, #66, "
        "#71, #73, #78, #86, #111, #116, #123\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #2, #0, #5, #4, #7, #9, "
        "#1, #10, #12, #8, #15, #13, #17, #3, #22, #14, #24, #25, #6, #19, #11, #29, #20, #30, #34, #23, #18, #35, "
        "#37, #28, #16, #40, #42, #26, #44, #33, #47, #39, #49, #32, #50, #31, #45, #54, #27, #55, #21, #52, #59, #36, "
        "#57, #60, #64, #61, #38, #72, #67, #75, #41, #68, #77, #69, #79, #80, #82, #84, #43, #74, #85, #89, #70, #92, "
        "#46, #65, #95, #83, #97, #99, #100, #48, #76, #102, #90, #109, #51, #93, #94, #115, #53, #119, #87, #120, "
        "#88, #124\n\n========= Iteration: 200 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #134 (pt = 0), #135, "
        "#138, #139, #140, #113, #143, #114, #96, #148, #98, #151, #153, #154, #101, #156, #157, #122, #58, #106, "
        "#132, #166\n\nWORKER #2\n\tQueue: \n\nWORKER #22\n\tQueue: #78 (pt = 9), #86, #111, #116, #123, #128, #131, "
        "#133, #136, #141, #146, #158, #161, #163\n\n== STOREHOUSES ==\n\nSTOREHOUSE #1\n\tQueue: #2, #0, #5, #4, #7, "
        "#9, #1, #10, #12, #8, #15, #13, #17, #3, #22, #14, #24, #25, #6, #19, #11, #29, #20, #30, #34, #23, #18, #35, "
        "#37, #28, #16, #40, #42, #26, #44, #33, #47, #39, #49, #32, #50, #31, #45, #54, #27, #55, #21, #52, #59, #36, "
        "#57, #60, #64, #61, #38, #72, #67, #75, #41, #68, #77, #69, #79, #80, #82, #84, #43, #74, #85, #89, #70, #92, "
        "#46, #65, #95, #83, #97, #99, #100, #48, #76, #102, #90, #109, #51, #93, #94, #115, #53, #119, #87, #120, "
        "#88, #124, #103, #125, #56, #104, #127, #129, #105, #91, #63, #108, #137, #81, #110, #112, #142, #66, #144, "
        "#145, #147, #62, #149, #117, #150, #71, #152, #118, #155, #121, #159, #73, #126, #160, #162, #130, #164, "
        "#165\n\n========= Iteration: 250 =========\n== WORKERS ==\n\nWORKER #1\n\tQueue: #113 (pt = 0), #178, #143, "
        "#180, #96, #183, #148, #98, #190, #154, #191, #192, #101, #193, #156, #157, #198, #132, #203, #204, #166, "
        "#207, #168\n\nWORKER #2\n\tQueue: \n\nWORKER #22\n\tQueue: #128 (pt = 9), #131, #133, #136, #141, #146, #158, "
        "#161, #163, #171, #173, #176, #181, #186, #188, #196, #201, #206\n\n== STOREHOUSES ==\n\nSTOREHOUSE "
        "#1\n\tQueue: #2, #0, #5, #4, #7, #9, #1, #10, #12, #8, #15, #13, #17, #3, #22, #14, #24, #25, #6, #19, #11, "
        "#29, #20, #30, #34, #23, #18, #35, #37, #28, #16, #40, #42, #26, #44, #33, #47, #39, #49, #32, #50, #31, #45, "
        "#54, #27, #55, #21, #52, #59, #36, #57, #60, #64, #61, #38, #72, #67, #75, #41, #68, #77, #69, #79, #80, #82, "
        "#84, #43, #74, #85, #89, #70, #92, #46, #65, #95, #83, #97, #99, #100, #48, #76, #102, #90, #109, #51, #93, "
        "#94, #115, #53, #119, #87, #120, #88, #124, #103, #125, #56, #104, #127, #129, #105, #91, #63, #108, #137, "
        "#81, #110, #112, #142, #66, #144, #145, #147, #62, #149, #117, #150, #71, #152, #118, #155, #121, #159, #73, "
        "#126, #160, #162, #130, #164, #165, #167, #78, #169, #170, #135, #172, #138, #174, #139, #175, #86, #140, "
        "#177, #179, #114, #182, #184, #111, #185, #187, #151, #189, #153, #116, #194, #195, #197, #122, #199, #58, "
        "#200, #123, #106, #202, #205, #107\n\n";

    std::ifstream raportFile(raportFilename);

    std::string fileContent((std::istreambuf_iterator<char>(raportFile)), std::istreambuf_iterator<char>());

    raportFile.close();

    EXPECT_EQ(fileContent, expectedOut);

    std::filesystem::remove(filename);
    std::filesystem::remove(raportFilename);
}
