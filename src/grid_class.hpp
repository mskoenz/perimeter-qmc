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
#include <memory>
#include <algorithm>
#include <typeinfo>

///  \brief where all the physics happens
///  
///  
namespace perimeter {
    ///  \brief grid class with site_struct as sites
    ///
    ///
    class grid_class { //grid is shorter than lattice ; 
    public:
        ///  the type of the sites
        typedef site_struct site_type;
    private:
        ///  the array where the sites are stored in
        template<typename U> 
        using array_type = boost::multi_array<U,2>;
        
        template<typename U> 
        using vector_type = std::vector<U>;
        
        typedef typename site_type::loop_type loop_type;
        typedef typename site_type::check_type check_type;
        typedef typename site_type::state_type state_type;
        typedef typename site_type::bond_type bond_type;
    public:
        typedef typename vector_type<site_type>::size_type index_type;
        ///standart constructor
        ///@param H height of the grid
        ///@param L length of the grid
        inline grid_class(uint const H, uint const L, uint const init = 0): 
                H_(H)
              , L_(L)
              , grid_(boost::extents[H_][L_]) 
              , n_loops_(loop_type()) {
            assert(H_%2==0);
            assert(L_%2==0);
            assert(H_>0);
            assert(L_>0);
            init_grid(init);
            init_loops();
        }
        
        void init_grid(uint const init) {
            assert(H_%2==0);
            assert(L_%2==0);
            assert(H_>0);
            assert(L_>0);
            int state = 0;
            std::for_each(begin(), end(), 
                [&](site_type & s) {
                    s.spin = (state + state / L_)%2 == 0 ? qmc::beta : qmc::alpha;
                    
                    for(state_type s1 = qmc::start; s1 < qmc::n_states/2; ++s1) {
                        state_type s2 = qmc::invert_state - s1;
                        if(init == 0) {
                            if(qmc::n_bonds == 3) {
                                s.bond[s1] = qmc::hori;
                                s.bond[s2] = qmc::hori;
                            }
                            else {
                                s.bond[s1] = (state%2==0 ? qmc::right:qmc::left);
                                s.bond[s2] = (state%2==0 ? qmc::right:qmc::left);
                            }
                        }
                        else if(init == 1) {
                            s.bond[s1] = (state/L_%2==0 ? qmc::down:qmc::up);
                            s.bond[s2] = (state/L_%2==0 ? qmc::down:qmc::up);
                        }
                        else if(init == 2) {
                            if(qmc::n_bonds == 3) {
                                s.bond[s1] = (state/L_%3==0 ? qmc::down: (state/L_%3==2 ? qmc::hori : qmc::up));
                                s.bond[s2] = (state/L_%3==0 ? qmc::down: (state/L_%3==2 ? qmc::hori : qmc::up));
                            }
                            else if(qmc::n_bonds == 6) {
                                s.bond[s1] = (state/L_%2==0 ? qmc::diag_down:qmc::diag_up);
                                s.bond[s2] = (state/L_%2==0 ? qmc::diag_down:qmc::diag_up);
                            }
                        }

                        s.loop = state;
                    }
                    ++state;
                }
            );
            //initialising the neighbor structure
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    grid_[i][j].neighbor[qmc::up] = &grid_[(i+H_-1)%H_][j];
                    grid_[i][j].neighbor[qmc::down] = &grid_[(i+1)%H_][j];
                    
                    if(qmc::n_bonds == 3) {
                        grid_[i][j].neighbor[qmc::hori] = &grid_[i][(j+L_ + 1 - 2*((i+j)%2) )%L_];
                    }
                    else {
                        grid_[i][j].neighbor[qmc::left] = &grid_[i][(j+L_-1)%L_];
                        grid_[i][j].neighbor[qmc::right] = &grid_[i][(j+1)%L_];
                    }
                    
                    if(qmc::n_bonds == 6) {
                        grid_[i][j].neighbor[qmc::diag_up] = &grid_[(i+H_-1)%H_][(j+L_-1)%L_];
                        grid_[i][j].neighbor[qmc::diag_down] = &grid_[(i+1)%H_][(j+1)%L_];
                    }
                }
            }
        }
        
        void init_loops() {
            if(n_loops_ != 0) {//can only be called once at the start
                throw std::runtime_error("init_loops() can only be called once");
                return;
            }
            
            check_type const level = 1;
            loop_type loop_var = 0;
            
            std::for_each(begin(), end(), 
                [&](site_type & s) {
                    if(s.check != level)
                    {
                        follow_loop(&s, loop_var);
                        ++loop_var;
                    }
                }
            );
            clear_check();
        }
        
        void clear_check(){
            std::for_each(begin(), end(), 
                [&](site_type & s) {
                    --(s.check);
                }
            );
        }
        
        void follow_loop(site_type * start, loop_type const & loop_var) {
            site_type * next = start;
            do {
                ++(next->check);
                next->loop = loop_var;
                next = next_in_loop(next);
            } while(next != start);
        }
        
        void follow_loop_spin(site_type * start, bool const & flip) {
            site_type * next = start;
            if(flip) {
                do {
                    ++(next->check);
                    next->spin = qmc::invert_spin - next->spin;
                    next = next_in_loop(next);
                } while(next != start);
            } else {
                do {
                    ++(next->check);
                    next = next_in_loop(next);
                } while(next != start);
            }
        }
        
        void follow_loop_once(site_type * start, loop_type const & loop_var) {
            site_type * next = start;
            do {
                next->loop = loop_var;
                next = next_in_loop(next);
            } while(next != start);
        }
        
        site_type * next_in_loop(site_type * in) const  {
            static state_type alternator = qmc::bra;
            alternator = qmc::invert_state - alternator;
            return in->partner(alternator);
        }
        
        void two_bond_flip(site_type * target, site_type * old_partner, bond_type b, state_type state) {
            //target node shows in the dircetion of the neighbor with the same orientation
            target->bond[state] = b;
            //old partner does the same
            old_partner->bond[state] = b;
            //the new partner of the target bond shows in the targets direction
            target->neighbor[b]->bond[state] = qmc::invert_bond - b;
            //old partner of the new partner does the same
            old_partner->neighbor[b]->bond[state] = qmc::invert_bond - b;
        }
        
        void two_bond_split(site_type * target, site_type * old_partner, bond_type b, state_type state) {
            two_bond_flip(target, old_partner, b, state);
            //rename after split
            if(qmc::n_bonds == 6) {
                loop_type tl = target->loop;
                
                follow_loop_once(old_partner, available_.back());
                if(tl == target->loop) {
                    available_.pop_back();
                    ++n_loops_;
                }
                else {
                    //~ std::cout << "\033[1;36m" << "---strange-loop-problem---" << "\033[0m" << std::endl;
                    
                    available_.back() = tl;
                }
            }
            else {
                follow_loop_once(old_partner, available_.back());
                available_.pop_back();
                ++n_loops_;
            }
        }
        
        void two_bond_join(site_type * target, site_type * old_partner, bond_type b, state_type state) {
            //rename before join
            available_.push_back(target->neighbor[b]->loop);
            --n_loops_;
            follow_loop_once(target->neighbor[b], target->loop);
            
            two_bond_flip(target, old_partner, b, state);
        }
        
        bond_type two_bond_update_site(site_type const & target, state_type state) const {
            for(bond_type b = qmc::start; b < qmc::n_bonds; ++b) {
                if(target.bond[state] == target.neighbor[b]->bond[state] and target.spin != target.neighbor[b]->spin) {
                    return b;
                }
            }
            return qmc::none;
        }
        
        ///  returns a reference to the site (i, j)
        ///  @param i index for the height
        ///  @param j index for the lenght
        site_type & operator()(index_type const i, index_type const j) {
            return grid_[i][j];
        }
        site_type const & operator()(index_type const i, index_type const j) const {
            return grid_[i][j];
        }
        
        ///  \brief print function
        ///  @param os is the stream that is printed into. Default is the std::cout ostream
        void print(std::ostream & os = std::cout) const {
            for(index_type i = 0; i < H_; ++i)
            {
                for(index_type j = 0; j < L_; ++j)
                {
                    os << std::setw(3) << grid_[i][j] << " ";
                }
                os << std::endl;
            }
        }
        
        void print_all(state_type const & state = qmc::bra) const {
            std::ostream & os = std::cout;
            const uint kmax = site_type::print_site_height();
            
            array_type<std::string> s(boost::extents[kmax * H_][L_]);
            vector_type<std::string> in;
            for(index_type i = 0; i < H_; ++i) {
                for(index_type j = 0; j < L_; ++j) {
                    in = grid_[i][j].string_print(L_, state);
                    for(index_type k = 0; k < kmax; ++k) {
                        s[i * kmax + k][j] = in[k];
                    }
                }
            }
            for(index_type i = 0; i < H_ * kmax; ++i) {
                if(qmc::n_bonds == 6)
                    for(index_type j = 0; j < H_*kmax - i; ++j)
                        os << " ";
                    
                for(index_type j = 0; j < L_; ++j)
                    os << s[i][j];
                os << std::endl;
            }
        }
        
        ///  \brief can be used to traverse the array
        site_type * begin() {
            return grid_.data();
        }
        
        ///  \brief can be used to traverse the array
        site_type * end() {
            return grid_.data() + grid_.num_elements();
        }
    private:
        uint const H_; ///<height
        uint const L_; ///<length
        array_type<site_type> grid_; ///< the actual grid
        
        loop_type n_loops_;
        std::vector<loop_type> available_;
    };
}//end namespace perimeter
#endif //__GRID_CLASS_HEADER
