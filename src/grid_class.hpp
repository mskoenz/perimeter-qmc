// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.04.2013 10:45:13 EDT
// File:    grid_class.hpp

#ifndef __GRID_CLASS_HEADER
#define __GRID_CLASS_HEADER

#include <site_struct.hpp>
#include <loop_realisator.hpp>

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
        /*///  \brief standart constructor
        ///  
        ///  @param H height of the grid
        ///  @param L length of the grid
        ///  @param init says how the grids should be initialized
        ///  
        ///  nearly all asserts are located in this constructor*/
        inline grid_class(uint const H, uint const L, std::vector<uint> const & init = std::vector<uint>(qmc::n_bra, 0)): 
                H_(H)
              , L_(L)
              , grid_(boost::extents[H_][L_]) {
            assert(H_%2 == 0);
            assert(L_%2 == 0);
            assert(qmc::n_states%2 == 0);
            assert(H_>0);
            assert(L_>0);
            assert(qmc::n_states > 0);
            assert(qmc::n_bonds == qmc::tri or qmc::n_bonds == qmc::sqr or qmc::n_bonds == qmc::hex);
            
            init_grid(init);
            n_loops_[qmc::start_state] = 0;
            init_loops();
        }
        void read_in(state_type const & bra, loop_real_class const & input) {
            assert(H_ == input.H());
            assert(L_ == input.L());
            assert(input.conform());
            
            if(qmc::n_bonds != qmc::sqr) {
                std::cout << "not supported for anything else than square" << std::endl;
                return;
            }
            std::for_each(begin(), end(),
                [&](site_struct & s){
                    s.bond[bra] = qmc::none;
                    s.bond[qmc::invert_state - bra] = qmc::none;
                }
            );
            //=================== copy parse to loop ===================
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    grid_[i][j].loop[bra] = input(i, j).to_ulong();
                }
            }
            //=================== construct bond-structure ===================
            std::for_each(begin(), end(), 
                [&](site_struct & s) {
                    if(s.check[bra] == false) {
                        follow_loop_build(&s, bra);
                    }
                }
            );
            clear_check();
            init_loops();
        }
        
        /*///  \brief resets the flag, that a site has been visited
        ///  
        ///  all states use the same check-flag, since only one state is changed at a certain time.
        ///  this fct resets the check-flag for all sites*/
        void clear_check(){
            std::for_each(begin(), end(), 
                [&](site_type & s) {
                    s.check = check_type();
                }
            );
        }
        
        template<typename F>
        void follow_loop_tpl(site_type * const start, state_type const & bra, F fct) {
            site_type * next = start;
            do {
                fct(next);
                next = next_in_loop(next, bra);
            } while(next != start);
        }
        
          /*///  \brief performs the two_bond update for the bonds
        ///  
        ///  @param target is the input site
        ///  @param old_partner is the old partner of the target
        ///  @param b is the direction of the bond of target and old_partner after the update
        ///  @param state names the choosen state in which the flip should occure*/
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
        
        /*///  \brief performs a two bond update with a split
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
        */
        void two_bond_split(site_type * target, site_type * old_partner, bond_type const & b, state_type const & state, state_type const & bra) {
            two_bond_flip(target, old_partner, b, state);
            
            //rename after split
            if(qmc::n_bonds == qmc::tri) {
                loop_type tl = target->loop[bra];
                
                follow_loop_tpl(old_partner, bra, 
                    [&](site_type * next) {
                        next->loop[bra] = available_[bra].back();
                    }
                );
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
                follow_loop_tpl(old_partner, bra, 
                    [&](site_type * next) {
                        next->loop[bra] = available_[bra].back();
                    }
                );
                available_[bra].pop_back();
                ++n_loops_[bra];
            }
        }
        
        /*///  \brief performs a two bond update with a join
        ///  
        ///  @param target is the input site
        ///  @param old_partner is the old partner of the target
        ///  @param b is the direction of the bond of target and old_partner after the update
        ///  @param state names the choosen state in which the flip should occure
        ///  @param bra is the corresponing bra of state. if state is a bra, then its equal to state. needed as index for the transition graph
        ///  
        ///  joins the loop of target and new_partner. the fact that they didn't have the same loop label is 
        ///  sufficient to predict a join*/
        void two_bond_join(site_type * target, site_type * old_partner, bond_type const & b, state_type const & state, state_type const & bra) {
            //rename before join
            available_[bra].push_back(target->neighbor[b]->loop[bra]);
            --n_loops_[bra];
            
            follow_loop_tpl(target->neighbor[b], bra, 
                    [&](site_type * next) {
                        next->loop[bra] = target->loop[bra];
                    }
                );
            
            
            two_bond_flip(target, old_partner, b, state);
        }
        
        /*///  \brief returns the neighbor direction in which an update is possible (qmc::none if not possible)
        ///  
        ///  @param target is the input site
        ///  @param old_partner is the old partner of the target
        ///  @param b is the direction of the bond of target and old_partner after the update
        ///  @param state names the choosen state in which the flip should occure
        ///  @param bra is the corresponing bra of state. if state is a bra, then its equal to state. needed as index for the transition graph*/
        bond_type two_bond_update_site(site_type const & target, state_type const & state, state_type const & bra) const {
            for(bond_type b = qmc::start_bond; b < qmc::n_bonds; ++b) {
                if(target.bond[state] == target.neighbor[b]->bond[state] and target.spin[state] != target.neighbor[b]->spin[state]) {
                    return b;
                }
            }
            return qmc::none;
        }
        
        void state_swap(state_type const & s1, state_type const & s2) {
            std::for_each(begin(), end(),
                [&](site_struct & s){
                    if(s.swap_lvl[qmc::invert_state - s1] == true)
                        std::swap(s.bond[s1], s.bond[s2]);
                }
            );
            //~ init_loops();
        }
        
        template<typename T> //T must suport an operator()(uint, uint, uint)
        void set_swap_lvl(T const & region) {
            
            //set the swap_lvl (only bra)
            for(state_type bra = qmc::start_state; bra < qmc::n_bra - 2; ++bra) {
                state_type ket = qmc::invert_state - bra;
                for(index_type i = 0; i < H_; ++i)
                    for(index_type j = 0; j < L_; ++j) {
                        grid_[i][j].swap_lvl[bra] = region(bra, i, j);
                        grid_[i][j].swap_lvl[ket] = 0;
                        //~ grid_[i][j].swap_lvl[ket] = qmc::n_states - 1 - region(bra, i, j); //TODO
                    }
            }
            //swap the kets
            for(state_type bra = qmc::start_state; bra < qmc::n_bra - 2; ++bra) {
                state_type ket = qmc::invert_state - bra;
                for(index_type i = 0; i < H_; ++i)
                    for(index_type j = 0; j < L_; ++j) {
                        if(grid_[i][j].swap_lvl[bra] > bra) {
                            std::swap(grid_[i][j].bond[qmc::invert_state - grid_[i][j].swap_lvl[bra]], grid_[i][j].bond[ket]);
                        }
                    }
            }
            //label the loops
            uint loop_var = 0;
            for(state_type bra = qmc::start_state; bra < qmc::n_bra - 2; ++bra) {
                for(index_type i = 0; i < H_; ++i) {
                    for(index_type j = 0; j < L_; ++j) {
                        if(grid_[i][j].check[bra] != true) {
                            follow_loop_multi_swap(&grid_[i][j], loop_var, bra);
                            ++loop_var;
                        }
                    }
                }
            }
        }
        
        void follow_loop_multi_swap(site_type * const start, loop_type const & loop_var, state_type const & bra) {
            site_type * next = start;
            site_type * peek = start;
            state_type level = bra;
                
            if(next->swap_lvl[level] != bra){ //start with bra if level not swap_lvl
                alternator_[level] = qmc::invert_state - bra;
            }
            
            do {
                next->check[level] = true;
                next->loop[level] = loop_var;
                peek = next_in_loop(next, level);
                if(peek->swap_lvl[level] != next->swap_lvl[level]) {
                    if(alternator_[level] > qmc::n_bra) { //it's a ket
                        if(level != next->swap_lvl[level]) {
                            
                            level = next->swap_lvl[level];
                            
                            if(level != peek->swap_lvl[level]) { //double jump
                                level = peek->swap_lvl[level];
                            }
                            
                        }
                        else
                            level = peek->swap_lvl[level];
                        
                        alternator_[level] = qmc::invert_state - level;
                    }
                }
                next = peek;
            } while(next != start or level != bra);
        }
        
        
        
        uint64_t stage2(state_type const & state) {
            assert(H_ * L_ * 2 > 64);
            std::bitset<64> res(0);
            for(index_type i = 0; i < H_; ++i) {
                for(index_type j = 0; j < L_; ++j) {
                    
                    if(grid_[i][j].bond[state] == qmc::right)
                        res[2 * i * L_ + j] = 1;
                    if(grid_[i][j].bond[state] == qmc::down)
                        res[(2 * i + 1) * L_ + j] = 1;
                }
            }
            return res.to_ulong();
        }
        
        //=================== getter ===================
        /*///  returns a reference to the site (i, j)
        ///  @param i index for the height
        ///  @param j index for the lenght*/
        site_type & operator()(index_type const i, index_type const j) {
            return grid_[i][j];
        }
        
        /*///  returns a const reference to the site (i, j)
        ///  @param i index for the height
        ///  @param j index for the lenght*/
        site_type const & operator()(index_type const i, index_type const j) const {
            return grid_[i][j];
        }
        
        //=================== loop analysis ===================
        void init_loops(state_type const & bra) {
            available_[bra].clear();
            n_loops_[bra] = 0;
            std::for_each(begin(), end(), 
                [&](site_type & s) {
                    if(s.check[bra] == false)
                    {
                        //~ follow_loop(&s, n_loops_[bra], bra);
                        follow_loop_tpl(&s, bra, 
                            [&](site_type * next){
                                next->check[bra] = true;
                                next->loop[bra] = n_loops_[bra];
                            }
                        );
                        ++n_loops_[bra];
                    }
                }
            );
            clear_check();
        }
        
        /*///  \brief returns a map with infos about the loopsize-distribution
        ///  
        ///  @param bra is the index for the transition graph
        ///  
        ///  returns a map with the loop lenght as key an the amount of occurance as value (devide by (H*L)/2 for percentage)*/
        std::map<uint, uint> loop_analysis(state_type const & bra) const {
            std::map<uint, uint> l;
            for(index_type i = 0; i < H_; ++i)  {
                for(index_type j = 0; j < L_; ++j)  {
                    ++(l[grid_[i][j].loop[bra]]);
                }
            }
            
            std::map<uint, uint> res;
            std::for_each(l.begin(), l.end(),
                [&](std::pair<const uint, uint> & p) {
                    ++(res[p.second]);
                }
            );
            return res;
        }
        
        loop_type n_loops(std::vector<state_type> const & vec) const {
            loop_type res(0);
            std::for_each(vec.begin(), vec.end(), 
                [&](state_type const & bra) {
                    res += n_loops(bra);
                }
            );
            return res;
        }
        loop_type n_loops(state_type const & bra) const {
            loop_type res(0);
            auto l = loop_analysis(bra);
            std::for_each(l.begin(), l.end(),
                [&](std::pair<const uint, uint> & p) {
                    res += p.second;
                }
            );
            return res;
        }
        
        loop_type n_loops(state_type const &  bra, loop_type const & size) const {
            return loop_analysis(bra)[size];
        }
        
        loop_type n_all_loops() const {
            loop_type res(0);
            for(state_type bra = qmc::start_state; bra < qmc::n_bra; ++bra) {
                if(bra != qmc::swap_bra1 and bra != qmc::swap_bra2)
                    res += n_loops_[bra];
            }
            return res;
        }
        
        loop_type n_swap_loops() const {
            return n_loops_[qmc::swap_bra1] + n_loops_[qmc::swap_bra2];
        }
        
        loop_type n_all_loops(loop_type size) const {
            loop_type res(0);
            for(state_type bra = qmc::start_state; bra < qmc::n_bra; ++bra) {
                res += loop_analysis(bra)[size];
            }
            return res;
        }
        
        //=================== cross loop analysis ===================
        std::map<uint, uint> cross_loop_analysis(state_type const & bra1, state_type const & bra2) const {
            std::map<uint, uint> l;
            for(index_type i = 0; i < H_; ++i)  {
                for(index_type j = 0; j < L_; ++j)  {
                    ++(l[grid_[i][j].loop[bra1] + 10000 * grid_[i][j].loop[bra2]]);
                }
            }
            
            std::map<uint, uint> res;
            std::for_each(l.begin(), l.end(),
                [&](std::pair<const uint, uint> & p) {
                    ++(res[p.second]);
                }
            );
            return res;
        }
        
        uint64_t n_cross_loops(state_type const & bra1, state_type const & bra2) const {
            uint64_t res(0);
            auto l = cross_loop_analysis(bra1, bra2);
            std::for_each(l.begin(), l.end(),
                [&](std::pair<const uint, uint> & p) {
                    res += p.second * std::pow(100, p.first - 1);
                }
            );
            return res;
        }
        
        loop_type n_cross_loops(state_type const & bra1, state_type const & bra2, loop_type size) const {
            return cross_loop_analysis(bra1, bra2)[size];
        }
        
        void change_loop(state_type const &  bra, loop_type const & loop) {
            site_struct * start = NULL;
            //find a site that is part of the loop
            for(index_type i = 0; i < H_; ++i)  {
                for(index_type j = 0; j < L_; ++j)  {
                    if(grid_[i][j].loop[bra] == loop) {
                        start = &grid_[i][j];
                        break;
                    }
                }
            }
            site_struct * next = next_in_loop(start, bra);
            site_struct * delay = start;
            //swap bra and ket configuration of this loop
            do {
                std::swap(delay->bond[bra], delay->bond[qmc::invert_state - bra]);
                delay = next;
                next = next_in_loop(next, bra);
            } while(delay != start);
        }
        
        //=================== swap ===================
        template<typename T>
        void swap_region(T const & region) {
            copy_to_swap(qmc::bra, qmc::bra2, region);
            state_swap(qmc::swap_ket1, qmc::swap_ket2);
            build_swap_loops();
        }
        
        //=================== print and iterate ===================
        /*///  \brief print function
        ///  
        ///  @param os is the stream that is printed into. Default is the std::cout ostream
        ///  
        ///  Prints all transitions graphs. */
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
        
        /*///  \brief print function
        ///  
        ///  @param os is the stream that is printed into. Default is the std::cout ostream*/
        void print_all(std::vector<state_type> state = std::vector<state_type>(), uint flags = 1, std::ostream & os = std::cout) const {
            if(state.size() == 0) {
                for(state_type bra = qmc::start_state; bra != qmc::n_bra; ++bra) {
                    state.push_back(bra);
                }
            }
            for(index_type i = 0; i < state.size(); ++i) {
                state_type bra = state[i];
                if(bra == qmc::swap_bra1)
                    os << "swap state 1" << std::endl;
                else if(bra == qmc::swap_bra2)
                    os << "swap state 2" << std::endl;
                else
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
        
        ///  \brief can be used to traverse the array
        site_type * begin() {
            return grid_.data();
        }
        
        ///  \brief can be used to traverse the array
        site_type * end() {
            return grid_.data() + grid_.num_elements();
        }
        
    private:
        /*///  \brief used by the constructor
        ///  
        ///@param init says how the grids should be initialized
        ///  
        ///  is only used once by the constructor at the start. initializes spin and bond structure*/
        void init_grid(std::vector<uint> const init) {
            int state = 0;
            
            for(state_type bra = qmc::start_state; bra != qmc::n_bra; ++bra) {
                std::for_each(begin(), end(), 
                    [&](site_type & s) {
                        state_type ket = qmc::invert_state - bra;
                        alternator_[bra] = bra;
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
        
        /*///  \brief used by the constructor
        ///  
        ///  is also only used once by the constructor at the start. initializes the loop labels*/
        void init_loops() {
            for(state_type bra = qmc::start_state; bra != qmc::n_bra; ++bra)
                init_loops(bra);
        }
        
        uint find_lowest(uint const & nr) const {
            assert(nr != 0);
            int shift = 0;
            while(((nr >> shift) & 1) == 0)
                ++shift;
            return shift;
        }
        
        void follow_loop_build(site_struct * const start, uint const & bra) {
            state_type state = bra;
            site_struct * next = start;
            do {
                next->check[bra] = true;
                int lowest = find_lowest(next->loop[bra]);
                int invert = qmc::invert_bond-lowest;
                if(next->loop[bra] - (1 << lowest) > 0)
                    next->loop[bra] -= (1 << lowest);
                
                next->bond[state] = lowest;
                if(lowest == qmc::me) {
                    next->bond[qmc::invert_state - state] = lowest;
                    break;
                }

                if(next->partner(state)->loop[bra] - (1 << invert) > 0)
                    next->partner(state)->loop[bra] -= (1 << invert);
                next->partner(state)->bond[state] = invert;
                next = next->partner(state);
                state = qmc::invert_state - state;
            } while(next != start);
        }
        
        /*///  \brief returns next site in the loop
        ///  
        ///  @param in is the input site
        ///  @param bra describes what transition graph is chosen*/
        site_type * next_in_loop(site_type * const in, state_type const & bra) {
            alternator_[bra] = qmc::invert_state - alternator_[bra];
            return in->partner(alternator_[bra]);
        }
        
        //------------------- swap -------------------
                
        template<typename T>
        void copy_to_swap(state_type const & bra1, state_type const & bra2, T const & region) {
            std::map<loop_type, bool> loops_in_zone;
            for(index_type i = 0; i < H_; ++i) {
                for(index_type j = 0; j < L_; ++j) {
                    grid_[i][j].loop[qmc::swap_bra1] = grid_[i][j].loop[bra1];
                    grid_[i][j].bond[qmc::swap_bra1] = grid_[i][j].bond[bra1];
                    grid_[i][j].bond[qmc::swap_ket1] = grid_[i][j].bond[qmc::invert_state - bra1];
                    
                    grid_[i][j].loop[qmc::swap_bra2] = grid_[i][j].loop[bra2] + (H_ * L_ / 2);
                    grid_[i][j].bond[qmc::swap_bra2] = grid_[i][j].bond[bra2];
                    grid_[i][j].bond[qmc::swap_ket2] = grid_[i][j].bond[qmc::invert_state - bra2];
                    
                    assert(grid_[i][j].check == 0);
                    
                    bool swap_zone = region(i, j);
                    
                    if(swap_zone) {
                        loops_in_zone[grid_[i][j].loop[qmc::swap_bra1]] = 1;
                        loops_in_zone[grid_[i][j].loop[qmc::swap_bra2]] = 1;
                    }
                    
                    grid_[i][j].swap_lvl[qmc::swap_bra1] = swap_zone;
                    grid_[i][j].swap_lvl[qmc::swap_bra2] = !swap_zone;
                    grid_[i][j].swap_lvl[qmc::swap_ket1] = !swap_zone;
                    grid_[i][j].swap_lvl[qmc::swap_ket2] = swap_zone;
                }
            }
            n_loops_[qmc::swap_bra1] = n_loops_[bra1] + n_loops_[bra2] - loops_in_zone.size();
            n_loops_[qmc::swap_bra2] = 0; //doesn't hold any usefull information
        }
        
        void build_swap_loops() {
            //------------------- first implementation -------------------
            loop_type loop_nr = H_ * L_;
            std::for_each(begin(), end(),
                [&](site_type & s) {
                    if(s.swap_lvl[qmc::swap_bra1] == true)
                        if(s.check[qmc::swap_bra1] == 0) {
                            follow_loop_swap(&s, loop_nr, qmc::swap_bra1);
                            ++loop_nr;
                            ++n_loops_[qmc::swap_bra1];
                        }
                }
            );
            std::for_each(begin(), end(),
                [&](site_type & s) {
                    if(s.swap_lvl[qmc::swap_bra1] == true)
                        if(s.check[qmc::swap_bra2] == 0) {
                            follow_loop_swap(&s, loop_nr, qmc::swap_bra2);
                            ++loop_nr;
                            ++n_loops_[qmc::swap_bra1];
                        }
                }
            );
            assert(n_loops_[qmc::swap_bra2] == 0);
            clear_check();
        }
        
        void follow_loop_swap(site_type * const start, loop_type const & loop_var, state_type const & bra) {
            site_type * next = start;
            site_type * peek = start;
            state_type level = bra;
            do {
                next->check[level] = true;
                next->loop[level] = loop_var;
                peek = next_in_loop(next, level);
                if(peek->swap_lvl[level] != next->swap_lvl[level]) {
                    if(alternator_[level] == qmc::swap_ket1) {
                        level = qmc::swap_bra2;
                        alternator_[level] = qmc::swap_ket2;
                    }
                    else if(alternator_[level] == qmc::swap_ket2) {
                        level = qmc::swap_bra1;
                        alternator_[level] = qmc::swap_ket1;
                    }
                }
                next = peek;
            } while(next != start or level != bra);
        }
    private:
        uint const H_; ///<height
        uint const L_; ///<length
        array_type<site_type> grid_; ///< the actual grid
        
        loop_type n_loops_[qmc::n_bra]; ///< keeps track of the amount of loops in a transition_graph
        std::vector<loop_type> available_[qmc::n_bra]; ///< if a loop is joined, the unused loop label is put on this stack for later usage
        state_type alternator_[qmc::n_bra]; ///< for finding the next_in_loop. one has always to alternate between bra and ket state
    };
}//end namespace perimeter
#endif //__GRID_CLASS_HEADER
