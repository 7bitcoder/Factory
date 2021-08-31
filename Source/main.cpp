#include <iostream>

#include "CommandParser.hpp"
#include "Controller.hpp"

int main(int argc, char **argv)
{
    try
    {

        sd::CommandParser parser;
        if (parser.parse(argc, argv))
        {
            sd::Controller c{parser.getResults()};
            c.run();
        }
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }
    return 0;
}
