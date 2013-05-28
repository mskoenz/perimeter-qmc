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
    addon::parameter.set("init1", 0);
    addon::parameter.set("f", 1);
    addon::parameter.set("g", 0);
    
    addon::parameter.set("term", 100);
    addon::parameter.set("sim", 1000);
    
    addon::parameter.set("H", 4);
    addon::parameter.set("L", 4);
    
    addon::parameter.read(argc, argv);
    
    sim_class sim(addon::parameter.get(), "../examples/swap_16x16.txt");
    
    //~ sim.grid().print_all({}, addon::parameter["-f"]);
    //~ sim.update();
    
    //~ sim.grid().init_loops();
    //~ auto start_loops = sim.grid().n_loops();
    //~ sim.grid().print_all({}, addon::parameter["-f"]);
    
    //~ sim.grid().set_shift_mode(qmc::ket_preswap);
    //~ sim.grid().init_loops();
    //~ auto preswap_loops = sim.grid().n_loops();
    //~ sim.grid().print_all({}, addon::parameter["-f"]);
    
    //~ sim.grid().set_shift_mode(qmc::ket_swap);
    //~ sim.grid().init_loops();
    //~ auto swap_loops = sim.grid().n_loops();
    //~ sim.grid().print_all({}, addon::parameter["-f"]);
    
    //~ DEBUG_VAR(start_loops)
    //~ DEBUG_VAR(preswap_loops)
    //~ DEBUG_VAR(swap_loops)
    
    sim.grid().print_all({0,1}, addon::parameter["-f"]);
    sim.run();
    sim.grid().set_shift_mode(qmc::ket_preswap);
    sim.grid().print_all({0,1}, addon::parameter["-f"]);
    sim.present_data();
    
    return 0;
}
