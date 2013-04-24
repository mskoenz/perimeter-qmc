// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    18.04.2013 12:48:33 EDT
// File:    sim_class.hpp

#ifndef __SIM_CLASS_HEADER
#define __SIM_CLASS_HEADER

#include <map>
#include <algorithm>
#include <iostream>
#include <assert.h>

#include <grid_class.hpp>
#include <site_struct.hpp>
#include <random2_msk.hpp>
#include <timer2_msk.hpp>

//perimeter is documented in grid_class.hpp
namespace perimeter
{
    class sim_class {
        typedef grid_class<site_struct>::index_type index_type;
    public:
        sim_class(std::map<std::string, double> param): H_(param["-H"])
                                                        , L_(param["-L"])
                                                        , param_(param)
                                                        , grid_(H_, L_) {
            assert(H_%2==0);
            assert(L_%2==0);
            assert(H_>0);
            assert(L_>0);
            
            std::cout << "Parameter" << std::endl;
            for(auto in = param_.begin(); in != param_.end(); ++in)
                std::cout << in->first << " = " << in->second << std::endl;
            
            init_grid();
            //~ grid_.print_all();
        }
        
        void init_grid() {
            int state = 0;
            std::for_each(grid_.begin(), grid_.end(), 
                [&](site_struct & s) {
                    s.spin = (state + state / L_)%2;
                    if(param_["-init"] == 0) {
                        
                        if(qmc::n_bonds == 3) {
                            s.bond[qmc::bra] = qmc::hori;
                            s.bond[qmc::ket] = qmc::hori;
                        }
                        else {
                            s.bond[qmc::bra] = (state%2==0 ? qmc::right:qmc::left);
                            s.bond[qmc::ket] = (state%2==0 ? qmc::right:qmc::left);
                        }
                    }
                    else if(param_["-init"] == 1) {
                        s.bond[qmc::bra] = (state/L_%2==0 ? qmc::down:qmc::up);
                        s.bond[qmc::ket] = (state/L_%2==0 ? qmc::down:qmc::up);
                    }
                    else if(param_["-init"] == 2) {
                        s.bond[qmc::bra] = (state/L_%2==0 ? qmc::diag_down:qmc::diag_up);
                        s.bond[qmc::ket] = (state/L_%2==0 ? qmc::diag_down:qmc::diag_up);
                    }

                    s.loop = state;
                    ++state;
                }
            );
            //initialising the neighbor structure
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    grid_(i, j).neighbor[qmc::up] = &grid_((i+H_-1)%H_, j);
                    grid_(i, j).neighbor[qmc::down] = &grid_((i+1)%H_, j);
                    
                    if(qmc::n_bonds == 3) {
                        grid_(i, j).neighbor[qmc::hori] = &grid_(i, (j+L_ + 1 - 2*((i+j)%2) )%L_);
                    }
                    else {
                        grid_(i, j).neighbor[qmc::left] = &grid_(i, (j+L_-1)%L_);
                        grid_(i, j).neighbor[qmc::right] = &grid_(i, (j+1)%L_);
                    }
                    
                    if(qmc::n_bonds == 6) {
                        grid_(i, j).neighbor[qmc::diag_up] = &grid_((i+H_-1)%H_, (j+L_-1)%L_);
                        grid_(i, j).neighbor[qmc::diag_down] = &grid_((i+1)%H_, (j+1)%L_);
                    }
                }
            }
        }
        
        bool two_bond_update(index_type i, index_type j, uint state) {
            
            for(uint b = qmc::start; b < qmc::n_bonds; ++b) {
                if(grid_(i, j).bond[state] == grid_(i, j).neighbor[b]->bond[state]) {
                    uint dir = grid_(i, j).bond[state];
                    
                    grid_(i, j).bond[state] = b;
                    grid_(i, j).neighbor[dir]->bond[state] = b;
                    
                    grid_(i, j).neighbor[b]->bond[state] = qmc::invert - b;
                    grid_(i, j).neighbor[b]->neighbor[dir]->bond[state] = qmc::invert - b;
                    //~ grid_.print_all();
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
        void print() {
            grid_.print_all();
        }
    private:
        const uint H_;
        const uint L_;
        std::map<std::string, double> param_;
        grid_class<site_struct> grid_;
    };
}
#endif //__SIM_CLASS_HEADER
