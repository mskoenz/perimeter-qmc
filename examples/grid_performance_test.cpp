// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    25.04.2013 16:03:55 EDT
// File:    grid_performance_test.cpp

//~ #define NO_COLOR

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
    std::vector<int> H_vec = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100
                            , 110, 120, 130, 140, 150, 160, 170, 180, 190, 200  
                            , 210, 220, 230, 240, 250, 260, 270, 280, 290, 300  
                            , 310, 320, 330, 340, 350, 360, 370, 380, 390, 400  
                            , 410, 420, 430, 440, 450, 460, 470, 480, 490
                            ,  50, 100, 150, 200, 250, 300, 350, 400, 450, 500
                            , 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000
                            , 1050, 1100, 1150, 1200, 1250, 1300, 1350, 1400, 1450, 1500
                            , 1550, 1600, 1650, 1700, 1750, 1800, 1850, 1900, 1950, 2000
                            , 2050, 2100, 2150, 2200, 2250, 2300, 2350, 2400, 2450
                            };
    std::vector<int> L_vec = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10
                            , 10, 10, 10, 10, 10, 10, 10, 10, 10, 10
                            , 10, 10, 10, 10, 10, 10, 10, 10, 10, 10
                            , 10, 10, 10, 10, 10, 10, 10, 10, 10, 10
                            , 10, 10, 10, 10, 10, 10, 10, 10, 10
                            , 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
                            , 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
                            , 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
                            , 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
                            , 100, 100, 100, 100, 100, 100, 100, 100, 100
                            };
    assert(H_vec.size() == L_vec.size());
    version_info();
    std::vector<double> p_vec = {.8, .6, .4, .2, .1, 0};
    std::map<std::string, double> param;
    
    for(uint j = 0; j < p_vec.size(); ++j)
    {
    for(uint k = 0; k < L_vec.size(); ++k) {
        double H = H_vec[k];
        double L = L_vec[k];
        //~ double maxi = 100000000;
        double maxi = 1;
        double spin_mod = maxi;
        double accept = 0;
        param["-init"] = 0; //0 == horizontal bonds / 1 == vertical bonds
        param["-p"] = p_vec[j];
        param["-H"] = H;
        param["-L"] = L;
        
        sim_class s(param);
        
        addon::random_class<int, fibonacci> rngH(0, H);
        addon::random_class<int, fibonacci> rngL(0, L);
        
        timer_class<data> timer(maxi, "data_set_2.txt");
        timer.set_names("mc_updates[1/us]", "loop-time[us]", "spin-update-mod[-]", "H", "L", "size", "seedH", "reject[%]", "accept", "p");
        
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
        //~ s.grid().print_all();
        
        timer.print(1/timer.loop_time(), timer.loop_time(), spin_mod, H, L, H*L, rngH.seed(), 100 - 100 * accept / maxi, accept / maxi, p_vec[j]);
        timer.write(1/timer.loop_time(), timer.loop_time(), spin_mod, H, L, H*L, rngH.seed(), 100 - 100 * accept / maxi, accept / maxi, p_vec[j]);
        param.clear();
    }   
    }
    return 0;
}
