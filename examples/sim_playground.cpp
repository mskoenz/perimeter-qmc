// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    06.05.2013 12:07:06 EDT
// File:    sim_playground.cpp

#include <serialize.hpp>
#include <iostream>
#include <progress_save_msk.hpp>
#include <bash_parameter3_msk.hpp>
#include <sim_class.hpp>

using namespace std;
using namespace perimeter;

void test_loop_count(sim_class & sim) {
    sim.grid().init_loops();
    auto start_loops = sim.grid().n_loops();
    sim.grid().print_all({}, addon::parameter["f"]);

    sim.grid().set_shift_mode(qmc::ket_preswap);
    sim.grid().init_loops();
    auto preswap_loops = sim.grid().n_loops();
    sim.grid().print_all({}, addon::parameter["f"]);

    sim.grid().set_shift_mode(qmc::ket_swap);
    sim.grid().init_loops();
    auto swap_loops = sim.grid().n_loops();
    sim.grid().print_all({}, addon::parameter["f"]);

    DEBUG_VAR(start_loops)
    DEBUG_VAR(preswap_loops)
    DEBUG_VAR(swap_loops)
}

void test_sim(sim_class & sim) {
    sim.grid().print_all({}, addon::parameter["f"]);
    sim.run();
    //~ while(1) {
        //~ char a;
        //~ std::cin >> a;
        //~ sim.update();
        //~ sim.grid().print_all({0,1}, addon::parameter["f"]);
    //~ }

    sim.grid().set_shift_mode(qmc::ket_preswap);
    sim.grid().print_all({0,1}, addon::parameter["f"]);
    sim.present_data();

}

void test_spin_copy(sim_class & sim) {
    sim.grid().set_shift_mode(qmc::ket_preswap);
    sim.grid().init_loops();
    sim.grid().copy_to_ket();
    sim.grid().print_all({0,1,2}, addon::parameter["f"]);
}

void run_dual_sim() {

    addon::timer_class<addon::normal> timer(addon::parameter["L"]*2+2);


    for(uint i = 0; i <= addon::parameter["L"]; ++i) {
        timer.progress(i);
        DEBUG_MSG("pos")
        DEBUG_VAR(i)
        addon::parameter.set("g", i);
        sim_class sim(addon::parameter.get());
        sim.run();
    }

    for(uint i = 0; i <= addon::parameter["L"]; ++i) {
        timer.progress(addon::parameter["L"] + 1 + i);
        DEBUG_MSG("neg")
        DEBUG_VAR(i)
        addon::parameter.set("g", i);
        sim_class sim(addon::parameter.get());
        sim.run();
    }
}

void run_single_sim() {
    addon::timer_class<addon::normal> timer(addon::parameter["L"]+1);

    for(uint i = addon::checkpoint("i", i, 1); i <= addon::parameter["L"]; ++i) {
        timer.progress(i);
        addon::checkpoint.write();
        addon::parameter.set("g", i);
        sim_class sim(addon::parameter.get());
        std::ifstream ifs;
        ifs.open("arch.txt");
        if(ifs.is_open()) {
            sim.serialize(ifs);
            ifs.close();
            std::cout << "done reserialize" << std::endl;
        }
        sim.run();
        remove("arch.txt");
    }
}
int main(int argc, char* argv[])
{
    //~ addon::global_seed.set(0);

    addon::parameter.set("init0", 0);
    addon::parameter.set("init1", 0);
    addon::parameter.set("f", 1);
    addon::parameter.set("g", 0);

    addon::parameter.set("mult", 1);

    addon::parameter.set("H", 16);
    addon::parameter.set("L", 16);
    addon::parameter.set("s", 2);
    addon::parameter.set("S", 16);
    addon::parameter.set("shift_file", "16x16_shift.txt");
    addon::parameter.set("res_file", "results.txt");


    addon::parameter.read(argc, argv);
    
    addon::parameter.set("term", addon::parameter["mult"] * 100000);
    addon::parameter.set("sim", addon::parameter["mult"] * 1000000);
    
    sim_class sim(addon::parameter.get());
    sim.measure();
    
    std::string res_file = addon::parameter["res_file"];
    for(uint l = addon::checkpoint("l", l, addon::parameter["s"]); l < addon::parameter["S"]; l += 4) {
        addon::parameter.set("H", l);
        addon::parameter.set("L", l);
        std::stringstream ss;
        ss << l << res_file;
        addon::parameter.set("res_file", ss.str());
        run_single_sim();
    }
    
    
    
    
    return 0;
}
