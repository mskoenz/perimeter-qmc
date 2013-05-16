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
    //~ std::string test = "";
    std::string test = "../../";
    addon::parameter.set("init0", 0);
    addon::parameter.set("init1", 1);
    addon::parameter.set("p", 0.5);
    addon::parameter.set("f", 1);
    addon::parameter.set("g", 0);
    
    //~ addon::parameter.set("H", 4);
    //~ addon::parameter.set("L", 4);
    //~ swap_region_class sw(test + "../examples/swap/swap_4x4.txt");
    
    addon::parameter.set("H", 8);
    addon::parameter.set("L", 8);
    swap_region_class sw(test + "../examples/swap/swap_8x8.txt");
    
    addon::parameter.read(argc, argv);
    sim_class sim(addon::parameter.get());
    
    uint l0 = sim.grid().n_all_loops();
    std::cout << "l0: " << l0 << std::endl;
    sim.grid().swap_region(sw);
    
    uint l1 = sim.grid().n_swap_loops();
    sim.grid().print_all({qmc::swap_bra1});
    std::cout << "l1 after swap: " << l1 << std::endl;
    
    sw.set_grow({qmc::up, qmc::down, qmc::right, qmc::left});
    sw.grow(addon::parameter["-g"]);
    
    sim.grid().swap_region(sw);
    uint l2 = sim.grid().n_swap_loops();
    sim.grid().print_all({qmc::bra, qmc::swap_bra1}, uint(addon::parameter["-f"]));
    std::cout << "l2 after invertswap: " << l2 << std::endl;
    
    return 0;
}
