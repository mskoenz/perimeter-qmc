// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    18.04.2013 12:48:33 EDT
// File:    sim_class.hpp

#ifndef __SIM_CLASS_HEADER
#define __SIM_CLASS_HEADER

#include <grid_class.hpp>
#include <shift_region_class.hpp>

#include <random2_msk.hpp>
#include <timer2_msk.hpp>
#include <bash_parameter3_msk.hpp>
#include <accum_double.hpp>
#include <accum_simple.hpp>

#include <map>
#include <algorithm>
#include <iostream>
#include <assert.h>
#include <cmath>

//perimeter is documented in grid_class.hpp
namespace perimeter {
    class sim_class {
        typedef typename grid_class::index_type index_type;
        typedef typename grid_class::site_type site_type;
        typedef addon::bash_parameter_class::map_type map_type;
    public:
        sim_class(map_type const & param):    param_(param)
                                            , H_(param_["H"])
                                            , L_(param_["L"])
                                            , grid_(H_, L_, std::vector<uint>(2, 0))
                                            , rngS_()
                                            , rngH_(H_)
                                            , rngL_(L_)
                                            {
            shift_region_class sr_(param_["shift"]);
            sr_.set_grow(std::vector<bond_type>(1, qmc::right));
            sr_.grow(param_["g"]);
            grid_.set_shift_region(sr_);
        }
        
        void two_bond_update(index_type i, index_type j, state_type state) {
            if(qmc::n_bonds == qmc::tri)
                accept_ << grid_.two_bond_update_intern_2(i, j, state, int(rngS_() * 3));
            else
                accept_ << grid_.two_bond_update_intern_2(i, j, state, 0);
                //~ accept_ << grid_.two_bond_update_intern(i, j, state);
            
        }
        
        void spin_update() {
            //~ DEBUG_MSG("init loops")
            grid_.init_loops();
            //~ DEBUG_MSG("start spin")
            for(state_type bra = qmc::start_state; bra < qmc::n_bra; ++bra) {
                grid_.alternator_ = bra;
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
                                //~ std::cout << "follow change loop: " << s.loop[bra] << "   >";
                                grid_.follow_loop_tpl(&s, bra, 
                                    [&](site_type * next) {
                                        next->check[bra] = true;
                                        //~ std::cout << "."; std::cout.flush();
                                        next->spin[bra] = qmc::invert_spin - next->spin[bra];
                                    }
                                );
                                //~ std::cout << "done" << std::endl;
                            }
                        }
                    }
                );
            }
            grid_.clear_check();
        }
        
        void update() {
            grid_.copy_to_ket();
            grid_.set_shift_mode(qmc::no_shift);
            
            grid_.clear_tile_spin();
            
            for(state_type state = qmc::start_state; state < qmc::n_states; ++state)
                for(uint i = 0; i < H_ * L_; ++i) {
                    two_bond_update(rngH_(), rngL_(), state);
                }
            
            grid_.set_shift_mode(qmc::ket_preswap);
            spin_update();
        }
        
        void measure() {
            double loops = grid_.n_loops();
            data_["loops"] << loops;
            data_["overlap"] << pow(2.0, loops - 2*H_*L_* .5 );
            grid_.set_shift_mode(qmc::ket_swap);
            grid_.init_loops();
            data_["swap_loops"] << grid_.n_loops();
            data_["swap_overlap"] << pow(2.0, int(grid_.n_loops()) - loops);
            data_["mean_for_error"] << pow(2.0, int(grid_.n_loops()) - loops);
            grid_.set_shift_mode(qmc::ket_preswap);
        }
        void run() {
            //------------------- init state -------------------
            grid_.set_shift_mode(qmc::ket_preswap);
            //------------------- init timer -------------------
            addon::timer_class<addon::data> timer(param_["term"] + param_["sim"], param_["res"]);
            timer.set_names("seed", "H", "L", "sim", "term", "grow", "S2", "accept", "loop_time");
            timer.set_comment("test");
            
            std::string mean_file = std::string(param_["prog_dir"]) + "/mean.txt";
            remove(mean_file.c_str());
            
            for(uint i = 0; i < param_["term"]; ++i) {
                update();
                timer.progress(i, param_["timer_dest"]);
            }
            std::ofstream ofs;
            
            for(uint i = 0; i < param_["sim"]; ++i) {
                update();
                measure();
                timer.progress(param_["term"] + i, param_["timer_dest"]);
                if((i & 1023) == 0) {
                    ofs.open(mean_file, std::ios::app);
                    ofs << data_["mean_for_error"].mean() << std::endl;
                    data_["mean_for_error"] = accumulator_double();
                    ofs.close();
                }
            }
            timer.write_state(param_["term"] + param_["sim"]);
            
            timer.write(addon::global_seed.get()
                        , H_
                        , L_
                        , param_["sim"]
                        , param_["term"]
                        , param_["g"]
                        , -std::log(data_["swap_overlap"].mean())
                        , accept_.mean()
                        , timer.loop_time()
                        );
        }
        
        void present_data() {
            std::for_each(data_.begin(), data_.end(), 
                [&](std::pair<std::string const, accumulator_double> & p) {
                    std::cout << p.first << ": " << p.second << std::endl;
                }
            );
            std::cout << "S2 = " << -std::log(data_["swap_overlap"].mean()) << std::endl;
            std::cout << "accept " << int(accept_.mean() * 100) << "%" << std::endl;
        }
        
        grid_class & grid() {
            return grid_;
        }
        #ifdef __SERIALIZE_HEADER
        template<typename S>
        void serialize(S & io) {
            grid_.serialize(io);
            std::for_each(data_.begin(), data_.end(), 
                [&](std::pair<std::string const, accumulator_double> & p) {
                    p.second.serialize(io);
                }
            );
            accept_.serialize(io);
        }
        #endif
    private:
        map_type param_;
        const uint H_;
        const uint L_;
        grid_class grid_;
        addon::random_class<double, addon::mersenne> rngS_;
        addon::random_class<int, addon::mersenne> rngH_;
        addon::random_class<int, addon::mersenne> rngL_;
        std::map<std::string, accumulator_double> data_;
        accumulator_simple accept_;
    };
}
#endif //__SIM_CLASS_HEADER
