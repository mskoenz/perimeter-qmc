// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    25.04.2013 16:03:55 EDT
// File:    grid_performance_test.cpp

#define NO_COLOR

#include <sim_class.hpp>
#include <bash_parameter2_msk.hpp>
#include <timer2_msk.hpp>
#include <random2_msk.hpp>

#include <version/version_info.hpp>

#include <iostream>

using namespace std;
using namespace perimeter;
using namespace addon;

int main(int argc, char* argv[])
{
    double H = 6;
    double L = 10;
    version_info();
    double maxi = 10000000;
    double spin_mod = 100;
    double accept = 0;
    
    parameter.set(maxi, "i");
    parameter.set(spin_mod, "mod");
    parameter.set(H, "H");
    parameter.set(L, "L");
    parameter.set("init", 1); //0 == horizontal bonds / 1 == vertical bonds
    parameter.read(argc, argv);
    
    sim_class s(parameter.get());
    
    addon::random_class<int, mersenne> rngH(0, H);
    addon::random_class<int, mersenne> rngL(0, L);
    
    timer_class<normal> timer(maxi, "grid_perf.txt");
    timer.set_names("mc_updates[1/us]", "loop-time[us]", "spin-update-mod[-]", "H", "L", "size", "seedH", "reject[%]");
    
    int spin_mod_i = spin_mod;
    
    uint state = qmc::bra;
    for(uint i = 1; i <= maxi; ++i)
    {
        accept += s.two_bond_update(rngH(), rngL(), state);
        state = qmc::invert_state - state;
        if(i%spin_mod_i == 0)
            s.spin_update();
        timer.progress(i);
    }
    s.grid().print_all();
    
    timer.print(1/timer.loop_time(), timer.loop_time(), spin_mod, H, L, H*L, rngH.seed(), 100 - 100 * accept / maxi);
    timer.write(1/timer.loop_time(), timer.loop_time(), spin_mod, H, L, H*L, rngH.seed(), 100 - 100 * accept / maxi);
    
    return 0;
}
