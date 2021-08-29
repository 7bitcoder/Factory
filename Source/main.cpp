#include <iostream>

#include "Factory.hpp"

int main(int, char **)
{
    sd::Factory f;
    f.load("factory.txt");
    f.run();
}
