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
        
        ///  just for shorter notation
        typedef typename site_type::loop_type loop_type;
        ///  just for shorter notation
        typedef typename site_type::check_type check_type;
        ///  just for shorter notation
        typedef typename site_type::state_type state_type;
        ///  just for shorter notation
        typedef typename site_type::bond_type bond_type;
    public:
        ///  normally a uint
        typedef typename vector_type<site_type>::size_type index_type;
        ///  \brief standart constructor
        ///  
        ///  @param H height of the grid
        ///  @param L length of the grid
        ///  @param init says how the grid should be initialized
        ///  
        ///  nearly all asserts are located in this constructor
        inline grid_class(uint const H, uint const L, uint const init = 0): 
                H_(H)
              , L_(L)
              , grid_(boost::extents[H_][L_]) 
        {
            assert(H_%2 == 0);
            assert(L_%2 == 0);
            assert(qmc::n_states%2 == 0);
            assert(H_>0);
            assert(L_>0);
            assert(qmc::n_states > 0);
            assert(qmc::n_bonds == 3 or qmc::n_bonds == 4 or qmc::n_bonds == 6);
            
            init_grid(init);
            n_loops_[qmc::start] = 0;
            init_loops();
        }
        ///  \brief resets the flag, that a site has been visited
        ///  
        ///  all states use the same check-flag, since only one state is changed at a certain time.
        ///  this fct resets the check-flag for all sites
        void clear_check(){
            std::for_each(begin(), end(), 
                [&](site_type & s) {
                    --(s.check);
                }
            );
        }
        ///  \brief performs a two bond update with a split
        ///  
        ///  @param target is the input site
        ///  @param old_partner is the old partner of the target
        ///  @param b is the direction of the bond of target and old_partner after the update
        ///  @param state names the choosen state in which the flip should occure
        ///  @param bra is the corresponing bra of state. if state is a bra, then its equal to state. needed as index for the transition graph
        ///  
        ///  performs a two bond update where a longer loop is splitted into two smaller loops. for square grids
        ///  the fact that the partners where in the same loop is sufficient to predict a split. this doesn't hold
        ///  for triangular grids. so even though the fuction is called split, it doesn't split in certain circumstances
        ///  for triangular grids
        void two_bond_split(site_type * target, site_type * old_partner, bond_type const & b, state_type const & state, state_type const & bra) {
            two_bond_flip(target, old_partner, b, state);
            
            //rename after split
            if(qmc::n_bonds == 6) {
                loop_type tl = target->loop[bra];
                
                follow_loop_once(old_partner, available_[bra].back(), bra);
                if(tl == target->loop[bra]) {
                    available_[bra].pop_back();
                    ++n_loops_[bra];
                }
                else {
                    //~ std::cout << "\033[1;36m" << "---strange-loop-problem---" << "\033[0m" << std::endl;
                    
                    available_[bra].back() = tl;
                }
            }
            else {
                follow_loop_once(old_partner, available_[bra].back(), bra);
                available_[bra].pop_back();
                ++n_loops_[bra];
            }
        }
        ///  \brief performs a two bond update with a join
        ///  
        ///  @param target is the input site
        ///  @param old_partner is the old partner of the target
        ///  @param b is the direction of the bond of target and old_partner after the update
        ///  @param state names the choosen state in which the flip should occure
        ///  @param bra is the corresponing bra of state. if state is a bra, then its equal to state. needed as index for the transition graph
        ///  
        ///  joins the loop of target and new_partner. the fact that they didn't have the same loop label is 
        ///  sufficient to predict a join
        void two_bond_join(site_type * target, site_type * old_partner, bond_type const & b, state_type const & state, state_type const & bra) {
            //rename before join
            available_[bra].push_back(target->neighbor[b]->loop[bra]);
            --n_loops_[bra];
            follow_loop_once(target->neighbor[b], target->loop[bra], bra);
            
            two_bond_flip(target, old_partner, b, state);
        }
        ///  \brief returns the neighbor direction in which an update is possible (qmc::none if not possible)
        ///  
        ///  @param target is the input site
        ///  @param old_partner is the old partner of the target
        ///  @param b is the direction of the bond of target and old_partner after the update
        ///  @param state names the choosen state in which the flip should occure
        ///  @param bra is the corresponing bra of state. if state is a bra, then its equal to state. needed as index for the transition graph
        bond_type two_bond_update_site(site_type const & target, state_type const & state, state_type const & bra) const {
            for(bond_type b = qmc::start; b < qmc::n_bonds; ++b) {
                if(target.bond[state] == target.neighbor[b]->bond[state] and target.spin[bra] != target.neighbor[b]->spin[bra]) {
                    return b;
                }
            }
            return qmc::none;
        }
        ///  \brief follows a loop, changes the spins if flip is true and checks it as visited. global operation
        ///  
        ///  @param start can be any node in the target loop
        ///  @param flip decides, if all spins in the loop get flipped
        ///  @param bra describes the wanted transition graph. since the spins of bra and ket must
        ///  be identical, its enough to just specify bra
        ///  
        ///  while following the loop and eventually changing the spin for this transition-graph, the
        ///  visited sites are flaged as checked. therefore one need to uncheck the sites at the end of the
        ///  operation with clear_check()
        void follow_loop_spin(site_type * start, bool const & flip, state_type const & bra) {
            site_type * next = start;
            if(flip) {
                do {
                    ++(next->check);
                    next->spin[bra] = qmc::invert_spin - next->spin[bra];
                    next = next_in_loop(next, bra);
                } while(next != start);
            } else {
                do {
                    ++(next->check);
                    next = next_in_loop(next, bra);
                } while(next != start);
            }
        }
        ///  \brief returns a map with infos about the loopsize-distribution
        ///  
        ///  @param bra is the index for the transition graph
        ///  
        ///  returns a map with the loop lenght as key an the percentage of occurance as value
        std::map<int, double> loop_analysis(state_type const & bra) {
            std::map<int, int> l;
            std::for_each(begin(), end(), 
                [&](site_type & s) {
                    ++(l[s.loop[bra]]);
                }
            );
            
            std::map<int, double> res;
            std::for_each(l.begin(), l.end(),
                [&](std::pair<const int, int> & p) {
                    ++(res[p.second]);
                }
            );
            std::for_each(res.begin(), res.end(),
                [&](std::pair<const int, double> & p) {
                    p.second /= (H_ * L_ / 2);
                }
            );
            return res;
        }
        ///  returns a reference to the site (i, j)
        ///  @param i index for the height
        ///  @param j index for the lenght
        site_type & operator()(index_type const i, index_type const j) {
            return grid_[i][j];
        }
        ///  returns a const reference to the site (i, j)
        ///  @param i index for the height
        ///  @param j index for the lenght
        site_type const & operator()(index_type const i, index_type const j) const {
            return grid_[i][j];
        }
        ///  \brief print function
        ///  
        ///  @param os is the stream that is printed into. Default is the std::cout ostream
        ///  
        ///  Prints all transitions graphs. 
        void print(std::ostream & os = std::cout) const {
            for(state_type bra = qmc::start; bra != qmc::n_bra; ++bra) {
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
        ///  \brief print function
        ///  
        ///  @param os is the stream that is printed into. Default is the std::cout ostream
        void print_all(std::ostream & os = std::cout) const {
            for(state_type bra = qmc::start; bra != qmc::n_bra; ++bra) {
                os << "state nr: " << bra << std::endl;
                const uint kmax = site_type::print_site_height();
                
                array_type<std::string> s(boost::extents[kmax * H_][L_]);
                vector_type<std::string> in;
                for(index_type i = 0; i < H_; ++i) {
                    for(index_type j = 0; j < L_; ++j) {
                        in = grid_[i][j].string_print(L_, bra);
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
        }
        ///  \brief can be used to traverse the array
        site_type * begin() {
            return grid_.data();
        }
        ///  \brief can be used to traverse the array
        site_type * end() {
            return grid_.data() + grid_.num_elements();
        }
    //~ private:
        ///  \brief used by the constructor
        ///  
        ///@param init says how the grid should be initialized
        ///  
        ///  is only used once by the constructor at the start. initializes spin and bond structure
        void init_grid(uint const init) {
            int state = 0;
            std::for_each(begin(), end(), 
                [&](site_type & s) {
                    for(state_type bra = qmc::start; bra != qmc::n_bra; ++bra) {
                        state_type ket = qmc::invert_state - bra;
                        alternator_[bra] = bra;
                        s.spin[bra] = (state + state / L_)%2 == 0 ? qmc::beta : qmc::alpha;
                        if(init == 0) {
                            if(qmc::n_bonds == 3) {
                                s.bond[bra] = qmc::hori;
                                s.bond[ket] = qmc::hori;
                            }
                            else {
                                s.bond[bra] = (state%2==0 ? qmc::right:qmc::left);
                                s.bond[ket] = (state%2==0 ? qmc::right:qmc::left);
                            }
                        }
                        else if(init == 1) {
                            s.bond[bra] = (state/L_%2==0 ? qmc::down:qmc::up);
                            s.bond[ket] = (state/L_%2==0 ? qmc::down:qmc::up);
                        }
                        else if(init == 2) {
                            if(qmc::n_bonds == 3) {
                                s.bond[bra] = (state/L_%3==0 ? qmc::down: (state/L_%3==2 ? qmc::hori : qmc::up));
                                s.bond[ket] = (state/L_%3==0 ? qmc::down: (state/L_%3==2 ? qmc::hori : qmc::up));
                            }
                            else if(qmc::n_bonds == 6) {
                                s.bond[bra] = (state/L_%2==0 ? qmc::diag_down:qmc::diag_up);
                                s.bond[ket] = (state/L_%2==0 ? qmc::diag_down:qmc::diag_up);
                            }
                        }
                        s.loop[bra] = state;
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
        ///  \brief used by the constructor
        ///  
        ///  is also only used once by the constructor at the start. initializes the loop labels
        void init_loops() {
            for(state_type bra = qmc::start; bra != qmc::n_bra; ++bra) {
                available_[bra].clear();
                check_type const level = 1;
                n_loops_[bra] = 0;
                std::for_each(begin(), end(), 
                    [&](site_type & s) {
                        if(s.check != level)
                        {
                            follow_loop(&s, n_loops_[bra], bra);
                            ++n_loops_[bra];
                        }
                    }
                );
                clear_check();
            }
        }
        ///  \brief follows a loop, changes the loop labels and checks it as visited. global operation
        ///  
        ///  @param start can be any node in the target loop
        ///  @param loop_var is the value one wants to assign to this loop
        ///  @param bra describes the wanted transition graph. since there are only half as many transition graphs
        ///  as states, only the bra must be given here.
        ///  
        ///  while following the loop and changing the loop variable for this transition-graph, the
        ///  visited sites are flaged as checked. therefore one need to uncheck the sites at the end of the
        ///  operation with clear_check(). is only used to initialize the loops
        void follow_loop(site_type * start, loop_type const & loop_var, state_type const & bra) {
            site_type * next = start;
            do {
                ++(next->check);
                next->loop[bra] = loop_var;
                next = next_in_loop(next, bra);
            } while(next != start);
        }
        ///  \brief follows a loop, changes the loop label without checking them as visited. local operation
        ///  
        ///  @param start can be any node in the target loop
        ///  @param loop_var is the value one wants to assign to this loop
        ///  @param bra describes the wanted transition graph. since there are only half as many transition graphs
        ///  as states, only the bra must be given here.
        ///  
        ///  follows the loop and changes the loop variable without marking the visited sites. a clear_check()
        ///  is not neccessary
        void follow_loop_once(site_type * start, loop_type const & loop_var, state_type const & bra) {
            site_type * next = start;
            do {
                next->loop[bra] = loop_var;
                next = next_in_loop(next, bra);
            } while(next != start);
        }
        ///  \brief returns next site in the loop
        ///  
        ///  @param in is the input site
        ///  @param bra describes what transition graph is chosen
        site_type * next_in_loop(site_type * in, state_type const & bra) {
            alternator_[bra] = qmc::invert_state - alternator_[bra];
            return in->partner(alternator_[bra]);
        }
        ///  \brief performs the two_bond update for the bonds
        ///  
        ///  @param target is the input site
        ///  @param old_partner is the old partner of the target
        ///  @param b is the direction of the bond of target and old_partner after the update
        ///  @param state names the choosen state in which the flip should occure
        void two_bond_flip(site_type * target, site_type * old_partner, bond_type const & b, state_type const & state) {
            //target node shows in the dircetion of the neighbor with the same orientation
            target->bond[state] = b;
            //old partner does the same
            old_partner->bond[state] = b;
            //the new partner of the target bond shows in the targets direction
            target->neighbor[b]->bond[state] = qmc::invert_bond - b;
            //old partner of the new partner does the same
            old_partner->neighbor[b]->bond[state] = qmc::invert_bond - b;
        }
        
    
    //~ private:
        uint const H_; ///<height
        uint const L_; ///<length
        array_type<site_type> grid_; ///< the actual grid
        
        loop_type n_loops_[qmc::n_bra]; ///< keeps track of the amount of loops in a transition_graph
        std::vector<loop_type> available_[qmc::n_bra]; ///< if a loop is joined, the unused loop label is put on this stack for later usage
        state_type alternator_[qmc::n_bra]; ///< for finding the next_in_loop. one has always to alternate between bra and ket state
    };
}//end namespace perimeter
#endif //__GRID_CLASS_HEADER
