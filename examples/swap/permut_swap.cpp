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
    std::string test = "";
    //~ std::string test = "../../";
    addon::parameter.set("init0", 0);
    addon::parameter.set("init1", 1);
    addon::parameter.set("p", 0.5);
    addon::parameter.set("f", 1);
    addon::parameter.set("g", 0);
    
    addon::parameter.set("H", 4);
    addon::parameter.set("L", 4);
    swap_region_class sw(test + "../examples/swap/swap_4x4.txt");
    
    sw.print(3);
    sw.write(test + "../examples/swap/swap_8x8_out.txt");
    addon::parameter.read(argc, argv);
    sim_class sim(addon::parameter.get());
    
    uint l0 = sim.grid().n_all_loops();
    std::cout << "l0: " << l0 << std::endl;
    sim.grid().set_swap_lvl(sw);
    sim.grid().print_all({qmc::bra, qmc::bra2, qmc::bra3}, uint(addon::parameter["-f"]));
    
    return 0;
}
