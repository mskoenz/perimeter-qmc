// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    18.04.2013 12:48:33 EDT
// File:    sim_class.hpp

#ifndef __SIM_CLASS_HEADER
#define __SIM_CLASS_HEADER

#include <grid_class.hpp>
#include <swap_region_class.hpp>

#include <random2_msk.hpp>

#include <map>
#include <algorithm>
#include <iostream>
#include <assert.h>

//perimeter is documented in grid_class.hpp
namespace perimeter
{
    class sim_class {
        typedef typename grid_class::index_type index_type;
        typedef typename grid_class::site_type site_type;
    public:
        sim_class(std::map<std::string, double> param):   H_(param["-H"])
                                                        , L_(param["-L"])
                                                        , param_(param)
                                                        , grid_(H_, L_, {uint(param["-init0"]), uint(param["-init1"]), uint(param["-init2"]), uint(param["-init3"]), uint(param["-init4"])}) 
                                                        , rngS_() 
                                                        , p(param_["-p"]){
            std::cout << "Parameter" << std::endl;
            for(auto in = param_.begin(); in != param_.end(); ++in)
                std::cout << in->first << " = " << in->second << std::endl;
        }
        
        bool two_bond_update(index_type i, index_type j, state_type state) {
            site_type & target = grid_(i, j);
            
            state_type bra = state;
            if(bra >= qmc::n_bra) //it's a ket
                bra = qmc::invert_state - state; //now it's a bra
            
            bond_type const b = grid_.two_bond_update_site(target, state, bra);
            if(b != qmc::none) {
                
                bond_type & dir = target.bond[state];
                if(target.loop[bra] == target.neighbor[b]->loop[bra]) {
                    grid_.two_bond_split(&target, target.neighbor[dir], b, state, bra);
                }
                else {
                    if(rngS_() > p)
                        grid_.two_bond_join(&target, target.neighbor[dir], b, state, bra);
                }
                return true;
            }
            return false;
        }
        
        void spin_update(state_type const & bra) {
            
            std::for_each(grid_.begin(), grid_.end(), 
                [&](site_type & s) {
                    if(s.check[bra] == false)
                    {
                        if(rngS_() > .5) {
                            grid_.follow_loop_tpl(&s, bra, 
                                [&](site_type * next) {
                                    next->check[bra] = true;
                                }
                            );
                        }
                        else {
                            grid_.follow_loop_tpl(&s, bra, 
                                [&](site_type * next) {
                                    next->check[bra] = true;
                                    next->spin[bra] = qmc::invert_spin - next->spin[bra];
                                    next->spin[qmc::invert_state - bra] = qmc::invert_spin - next->spin[qmc::invert_state - bra];
                                }
                            );
                        }
                    }
                }
            );
            grid_.clear_check();
        }
        
        void update() {
            
        }
        
        void measure() {
            
        }
        
        void run() {
            
        }
        
        grid_class & grid() {
            return grid_;
        }
    private:
        const uint H_;
        const uint L_;
        std::map<std::string, double> param_;
        grid_class grid_;
        addon::random_class<double, addon::mersenne> rngS_;
        double const p;
    };
}
#endif //__SIM_CLASS_HEADER
