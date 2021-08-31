#include <iostream>

#include "CommandParser.hpp"
#include "Controller.hpp"

int main(int argc, char **argv)
{
    auto &in = std::cin;
    auto &out = std::cout;
    auto &err = std::cerr;
    try
    {
        sd::CommandParser parser;
        if (parser.parse(argc, argv, out, err))
        {
            sd::Controller c{parser.getResults(), out, err, in};
            c.run();
        }
    }
    catch (std::exception &e)
    {
        out << e.what();
    }
    catch (...)
    {
        out << "Unexpected error occured";
    }
    return 0;
}
