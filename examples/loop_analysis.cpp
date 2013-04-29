// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    26.04.2013 15:24:07 EDT
// File:    loop_analysis.cpp

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
    version_info();
    
    std::vector<double> p_vec = {.9, .8, .7, .6, .5, .4, .3, .2, .1, 0};
    std::map<std::string, double> param;
    
    for(uint j = 0; j < p_vec.size(); ++j)
    {
        double H = 100;
        double L = H;
        //~ double maxi = 1000000000;
        double maxi = 1;
        double spin_mod = maxi;
        double accept = 0;
        param["-init"] = 0; //0 == horizontal bonds / 1 == vertical bonds
        param["-p"] = p_vec[j];
        param["-H"] = H;
        param["-L"] = L;
        
        sim_class s(param);
        
        addon::random_class<int, mersenne> rngH(0, H);
        addon::random_class<int, mersenne> rngL(0, L);
        
        timer_class<data> timer(maxi, "loop_analysis.txt");
        timer.set_names("mc_updates[1/us]", "H", "L", "size", "spin_mod", "loop_size", "loop_prob", "accept", "p");
        
        int spin_mod_i = spin_mod;
        
        std::map<int, double> acc;
        
        uint state = qmc::bra;
        for(uint i = 1; i <= maxi; ++i) {
            accept += s.two_bond_update(rngH(), rngL(), state);
            state = qmc::invert_state - state;
            if(i%spin_mod_i == 0)
                s.spin_update(qmc::bra);
            if(i%int(3*H*L) == 0) {
                std::map<int, double> l(s.grid().loop_analysis(qmc::bra));
                std::for_each(l.begin(), l.end(),
                    [&](std::pair<const int, double> & p) {
                        acc[p.first] += p.second;
                    }
                );
            }
                
            timer.progress(i);
        }
        //~ s.grid().print_all();
        
        std::for_each(acc.begin(), acc.end(),
            [&](std::pair<const int, double> & p) {
                std::cout << p.first << " "  << p.second/(maxi / (3*H*L)) << std::endl;
                timer.write(1/timer.loop_time(), H, L, H*L, spin_mod, p.first, p.second/(maxi / (3*H*L)), accept / maxi, p_vec[j]);
            }
        );
        
        timer.print(1/timer.loop_time(), H, L, H*L, spin_mod, 0, 0, accept / maxi, p_vec[j]);
        param.clear();
    }
    return 0;
}
