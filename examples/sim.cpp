// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    06.05.2013 12:07:06 EDT
// File:    sim.cpp

#include <serialize.hpp>
#include <iostream>
#include <progress_save_msk.hpp>
#include <bash_parameter3_msk.hpp>
#include <sim_class.hpp>

using namespace std;
using namespace perimeter;

int main(int argc, char* argv[])
{
    //~ addon::global_seed.set(0);
    auto & p = addon::parameter;
    p["init0"] = 0;
    p["init1"] = 0;
    p["f"] = 1;
    p["g"] = 0;

    p["mult"] = 1;

    p["H"] = 16;
    p["L"] = 16;
    p["shift"] = "shift.txt";
    p["spaceing"] = 1;
    p["res"] = "results.txt";
    p["timer_dest"] = 1;

    p.read(argc, argv);
    
    p["term"] = p["mult"] * 100000;
    p["sim"] = p["mult"] * 1000000;
    p["H"] = p["L"];
    
    std::string prog_dir = p["prog_dir"];
    p["shift"] = prog_dir + std::string(p["shift"]);
    p["res"] = prog_dir + std::string(p["res"]);
    
    addon::checkpoint.set_path(p["prog_dir"]);
    
    sim_class sim(p.get());
    
    sim.run();
    
    std::ofstream ofs;
    ofs.open(std::string(p["prog_dir"]) + "/last_config.txt");
    sim.serialize(ofs);
    ofs.close();
    
    return 0;
}
