// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    18.04.2013 12:48:33 EDT
// File:    sim_class.hpp

#ifndef __SIM_CLASS_HEADER
#define __SIM_CLASS_HEADER

#include <grid_class.hpp>
#include <random2_msk.hpp>
#include <timer2_msk.hpp>

#include <map>
#include <algorithm>
#include <iostream>
#include <assert.h>

//perimeter is documented in grid_class.hpp
namespace perimeter
{
    class sim_class {
        typedef grid_class::index_type index_type;
    public:
        sim_class(std::map<std::string, double> param):   H_(param["-H"])
                                                        , L_(param["-L"])
                                                        , param_(param)
                                                        , grid_(H_, L_, param["-init"]) {
            std::cout << "Parameter" << std::endl;
            for(auto in = param_.begin(); in != param_.end(); ++in)
                std::cout << in->first << " = " << in->second << std::endl;
        }
        
        bool two_bond_update(index_type i, index_type j, uint state) {
            
            for(uint b = qmc::start; b < qmc::n_bonds; ++b) {
                if(grid_(i, j).bond[state] == grid_(i, j).neighbor[b]->bond[state]) {
                    uint dir = grid_(i, j).bond[state];
                    
                    grid_(i, j).bond[state] = b;
                    grid_(i, j).neighbor[dir]->bond[state] = b;
                    
                    grid_(i, j).neighbor[b]->bond[state] = qmc::invert - b;
                    grid_(i, j).neighbor[b]->neighbor[dir]->bond[state] = qmc::invert - b;
                    
                    return true;
                }
            }
            return false;
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
    };
}
#endif //__SIM_CLASS_HEADER
