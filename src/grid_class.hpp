// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.04.2013 10:45:13 EDT
// File:    grid_class.hpp

#ifndef __GRID_CLASS_HEADER
#define __GRID_CLASS_HEADER

#include <site_struct.hpp>

#include <boost/integer.hpp>
#include <boost/multi_array.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <typeinfo>

///  \brief where all the physics happens
///  
///  
namespace perimeter {
    ///  \brief grid class with site_struct as sites
    ///
    ///  via the enums in site_struct.hpp can be specified, what grid-type is chosen 
    ///  and how many states pairs of states the grid has
    class grid_class { //grid is shorter than lattice ; 
    public:
        ///  the type of the sites
        typedef site_struct site_type;
    private:
        ///  the array where the sites are stored in
        template<typename U> 
        using array_type = boost::multi_array<U,2>;
        
        ///  the used vector_type
        template<typename U> 
        using vector_type = std::vector<U>;
    public:
        ///  normally a uint
        typedef typename vector_type<site_type>::size_type index_type;
        
        inline grid_class(uint const H, uint const L, std::vector<uint> const & init = std::vector<uint>(qmc::n_bra, 0)): 
                H_(H)
              , L_(L)
              , grid_(boost::extents[H_][L_])
              , n_loops_(0)
              , alternator_(qmc::start_state)
              , shift_mode_(qmc::no_shift) {
            assert(H_%2 == 0);
            assert(L_%2 == 0);
            assert(qmc::n_states%2 == 0);
            assert(H_>0);
            assert(L_>0);
            assert(qmc::n_states > 0);
            assert(qmc::n_bonds == qmc::tri or qmc::n_bonds == qmc::sqr or qmc::n_bonds == qmc::hex);
            
            init_grid(init);
            init_loops();
        }
        void clear_check(){
            std::for_each(begin(), end(), 
                [&](site_type & s) {
                    s.check = check_type();
                }
            );
        }
        template<typename F>
        void follow_loop_tpl(site_type * const start, state_type & bra, F fct) {
            state_type old_bra = bra;
            site_type * next = start;
            do {
                fct(next);
                next = next_in_loop(next, bra);
            } while(next != start or bra != old_bra);
        }
        void two_bond_flip(site_type * const target, site_type * const old_partner, bond_type const & b, state_type const & state) {
            //target node shows in the dircetion of the neighbor with the same orientation
            target->bond[state] = b;
            //old partner does the same
            old_partner->bond[state] = b;
            //the new partner of the target bond shows in the targets direction
            target->neighbor[b]->bond[state] = qmc::invert_bond - b;
            //old partner of the new partner does the same
            old_partner->neighbor[b]->bond[state] = qmc::invert_bond - b;
        }
        bond_type two_bond_update_site(site_type const & target, state_type const & state, state_type const & bra) const {
            for(bond_type b = qmc::start_bond; b < qmc::n_bonds; ++b) {
                if(target.bond[state] == target.neighbor[b]->bond[state] and target.spin[state] != target.neighbor[b]->spin[state]) {
                    return b;
                }
            }
            return qmc::none;
        }
        
        void set_shift_mode(shift_type const & new_mode) {
            site_type::shift_mode_print = new_mode;
            
            shift_mode_ = new_mode;
        }
        template<typename T> //T must suport an operator()(uint, uint, uint)
        void set_shift_region(T const & region) {
            for(shift_type shift_mode = qmc::start_shift; shift_mode != qmc::n_shifts; ++shift_mode)
                for(index_type i = 0; i < H_; ++i)
                    for(index_type j = 0; j < L_; ++j)
                        grid_[i][j].shift_region[shift_mode] = region(shift_mode, i, j);
        }
        
        
        site_type & operator()(index_type const i, index_type const j) {
            return grid_[i][j];
        }
        site_type const & operator()(index_type const i, index_type const j) const {
            return grid_[i][j];
        }

        void init_loops() {
            n_loops_ = 0;
            for(state_type bra = qmc::start_state; bra != qmc::n_bra; ++bra)
                std::for_each(begin(), end(), 
                    [&](site_type & s) {
                        if(s.check[bra] == false) {
                            alternator_ = bra;
                            auto old_bra = bra;
                            follow_loop_tpl(&s, bra, 
                                [&](site_type * next){
                                    next->check[bra] = true;
                                    next->loop[bra] = n_loops_;
                                }
                            );
                            //~ DEBUG_VAR(old_bra)
                            //~ DEBUG_VAR(bra)
                            assert(old_bra == bra);
                            //~ std::cout << "loop " << n_loops_ << " done" << std::endl;
                            ++n_loops_;
                        }
                    }
                );
            clear_check();
        }
        loop_type n_loops() {
            return n_loops_;
        }
        //=================== print and iterate ===================
        void print(std::vector<state_type> state = std::vector<state_type>(), std::ostream & os = std::cout) const {
            for(index_type i = 0; i < state.size(); ++i) {
                state_type bra = state[i];
                os << "state nr: " << bra << std::endl;
                for(index_type i = 0; i < H_; ++i) {
                    for(index_type j = 0; j < L_; ++j) {
                        os << std::setw(3);
                        grid_[i][j].print(bra, os);
                        os << " ";
                    }
                    os << std::endl;
                }
            }
        }
        void print_all(std::vector<state_type> state = std::vector<state_type>(), uint flags = 1, std::ostream & os = std::cout) const {
            if(state.size() == 0) {
                for(state_type bra = qmc::start_state; bra != qmc::n_bra; ++bra) {
                    state.push_back(bra);
                }
            }
            for(index_type i = 0; i < state.size(); ++i) {
                state_type bra = state[i];
                os << "state nr: " << bra << std::endl;
                const uint kmax = site_type::print_site_height();
                
                array_type<std::string> s(boost::extents[kmax * H_][5]);
                vector_type<std::string> in;
                for(index_type i = 0; i < H_; ++i) {
                    
                    if((flags & 1) == 1) { //print tragsition graph
                        for(index_type j = 0; j < L_; ++j) {
                            in = grid_[i][j].string_print(L_, bra, 0);
                            for(index_type k = 0; k < kmax; ++k) {
                                s[i * kmax + k][0] += in[k];
                                s[i * kmax + k][1] = "      ";
                            }
                        }
                    }
                    if((flags & 2) == 2) { //print bra
                        for(index_type j = 0; j < L_; ++j) {
                            in = grid_[i][j].string_print(L_, bra, 1);
                            for(index_type k = 0; k < kmax; ++k) {
                                s[i * kmax + k][2] += in[k];
                                s[i * kmax + k][3] = "      ";
                            }
                        }
                    }
                    if((flags & 4) == 4) { //print ket
                        for(index_type j = 0; j < L_; ++j) {
                            in = grid_[i][j].string_print(L_, bra, 2);
                            for(index_type k = 0; k < kmax; ++k) {
                                s[i * kmax + k][4] += in[k];
                            }
                        }
                    }
                }
                for(index_type i = 0; i < H_ * kmax; ++i) {
                    if(qmc::n_bonds == qmc::tri)
                        for(index_type j = 0; j < H_*kmax - i; ++j)
                            os << " ";
                        
                    for(index_type j = 0; j < 5; ++j)
                        os << s[i][j];
                    os << std::endl;
                }
            }
        }
        site_type * begin() {
            return grid_.data();
        }
        site_type * end() {
            return grid_.data() + grid_.num_elements();
        }
    private:
        void init_grid(std::vector<uint> const init) {
            int state = 0;
            
            for(state_type bra = qmc::start_state; bra != qmc::n_bra; ++bra) {
                std::for_each(begin(), end(), 
                    [&](site_type & s) {
                        state_type ket = qmc::invert_state - bra;
                        s.spin[bra] = (state + state / L_)%2 == 0 ? qmc::beta : qmc::alpha;
                        s.spin[qmc::invert_state - bra] = (state + state / L_)%2 == 0 ? qmc::beta : qmc::alpha;
                        
                        if(init[bra] == 0) {
                            if(qmc::n_bonds == qmc::hex) {
                                s.bond[bra] = qmc::hori;
                                s.bond[ket] = qmc::hori;
                            }
                            else {
                                s.bond[bra] = (state%2==0 ? qmc::right:qmc::left);
                                s.bond[ket] = (state%2==0 ? qmc::right:qmc::left);
                            }
                        }
                        else if(init[bra] == 1) {
                            s.bond[bra] = (state/L_%2==0 ? qmc::down:qmc::up);
                            s.bond[ket] = (state/L_%2==0 ? qmc::down:qmc::up);
                        }
                        else if(init[bra] == 2) {
                            if(qmc::n_bonds == qmc::hex) {
                                s.bond[bra] = (state/L_%3==0 ? qmc::down: (state/L_%3==2 ? qmc::hori : qmc::up));
                                s.bond[ket] = (state/L_%3==0 ? qmc::down: (state/L_%3==2 ? qmc::hori : qmc::up));
                            }
                            else if(qmc::n_bonds == qmc::tri) {
                                s.bond[bra] = (state/L_%2==0 ? qmc::diag_down:qmc::diag_up);
                                s.bond[ket] = (state/L_%2==0 ? qmc::diag_down:qmc::diag_up);
                            }
                        }
                        s.loop[bra] = state;
                    ++state;
                    }
                );
            }
            //initialising the neighbor structure
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    grid_[i][j].neighbor[qmc::up] = &grid_[(i+H_-1)%H_][j];
                    grid_[i][j].neighbor[qmc::down] = &grid_[(i+1)%H_][j];
                    grid_[i][j].neighbor[qmc::me] = &grid_[i][j];
                    
                    if(qmc::n_bonds == qmc::hex) {
                        grid_[i][j].neighbor[qmc::hori] = &grid_[i][(j+L_ + 1 - 2*((i+j)%2) )%L_];
                    }
                    else {
                        grid_[i][j].neighbor[qmc::left] = &grid_[i][(j+L_-1)%L_];
                        grid_[i][j].neighbor[qmc::right] = &grid_[i][(j+1)%L_];
                    }
                    
                    if(qmc::n_bonds == qmc::tri) {
                        grid_[i][j].neighbor[qmc::diag_up] = &grid_[(i+H_-1)%H_][(j+L_-1)%L_];
                        grid_[i][j].neighbor[qmc::diag_down] = &grid_[(i+1)%H_][(j+1)%L_];
                    }
                }
            }
        }
        site_type * next_in_loop(site_type * const in, state_type & bra) {
            //~ DEBUG_VAR(bra)
            //~ DEBUG_VAR(alternator_)
            //~ static state_type bra_now = z;
            //~ if(z != bra_now) {
                //~ bra_now = z;
                //~ if(alternator_ < qmc::n_bra) { //it's a bra
                    //~ alternator_ = qmc::invert_state - bra_now;
                //~ }
                //~ else
                    //~ alternator_ = bra_now;
            //~ }
            //~ else
            alternator_ = qmc::invert_state - alternator_;
            return in->loop_partner(alternator_, bra, shift_mode_); //alternator can be changed, as well as bra
        }
        
    private:
        uint const H_; ///<height
        uint const L_; ///<length
        array_type<site_type> grid_; ///< the actual grid
        
        loop_type n_loops_;
        state_type alternator_;
        shift_type shift_mode_;
    };
}//end namespace perimeter
#endif //__GRID_CLASS_HEADER
