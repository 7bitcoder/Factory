#include <iostream>

#include "CommandParser.hpp"
#include "Factory.hpp"

int main(int argc, char **argv)
{
    try
    {

        sd::CommandParser parser;
        if (parser.parse(argc, argv))
        {
            sd::Factory f{parser.getResults()};
            f.run();
        }
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }
    return 0;
}
