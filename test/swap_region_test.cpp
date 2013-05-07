// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    07.05.2013 14:22:22 EDT
// File:    swap_region_test.cpp

#include <iostream>
#include <sim_class.hpp>
#include <bash_parameter2_msk.hpp>

using namespace std;
using namespace perimeter;

int main(int argc, char* argv[])
{
    //~ std::string test = "";
    std::string test = "../";
    addon::parameter.set("g", 1); //grow
    addon::parameter.set("H", 4);
    addon::parameter.set("L", 4);
    addon::parameter.set("init0", 0);
    addon::parameter.set("init1", 1);
    addon::parameter.set("p", 0.5);
    addon::parameter.read(argc, argv);
    
    swap_region_class sw(test + "../examples/swap/swap_4x4.txt");
    sim_class sim(addon::parameter.get());
    
    uint l0 = sim.grid().n_all_loops();
    //~ std::cout << "l0: " << l0 << std::endl;
    assert(l0 == 16);
    
    sim.grid().swap_region(sw);
    uint l1 = sim.grid().n_swap_loops();
    //~ sim.grid().print_all();
    //~ std::cout << "l1 after swap: " << l1 << std::endl;
    assert(l1 == 12);
    
    sw.invert();
    sim.grid().swap_region(sw);
    uint l2 = sim.grid().n_swap_loops();
    //~ sim.grid().print_all();
    //~ std::cout << "l2 after invertswap: " << l2 << std::endl;
    assert(l2 == 4);
    
    swap_region_class sw2(test + "../examples/swap/swap_8x8.txt");
    //~ sw2.set_grow({qmc::right});
    //~ sw2.set_grow({qmc::up, qmc::hori});
    sw2.set_grow({qmc::right, qmc::left, qmc::up, qmc::down, qmc::diag_down, qmc::diag_up, qmc::hori});
    sw2.print(2);
    
    sw2.grow(addon::parameter.get()["-g"]);
    sw2.print(2);
    
    grid_class grid(8, 8);
    grid.swap_region(sw2);
    grid.print_all();
    
    sw2.write(test + "../examples/swap/swap_8x8_out.txt");
    
    return 0;
}
