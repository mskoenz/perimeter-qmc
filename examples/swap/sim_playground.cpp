// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    06.05.2013 12:07:06 EDT
// File:    sim_playground.cpp

#include <iostream>
#include <sim_class.hpp>
#include <bash_parameter2_msk.hpp>

using namespace std;
using namespace perimeter;

int main(int argc, char* argv[])
{
    addon::parameter.set("H", 4);
    addon::parameter.set("L", 4);
    addon::parameter.set("init0", 0);
    addon::parameter.set("init1", 1);
    addon::parameter.set("p", 0.5);
    addon::parameter.read(argc, argv);
    
    swap_region_class sw("../examples/swap/swap_4x4.txt");
    sim_class sim(addon::parameter.get());
    
    return 0;
}
