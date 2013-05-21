// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    18.04.2013 11:08:01 EDT
// File:    site_struct.hpp

#ifndef __SITE_STRUCT_HEADER
#define __SITE_STRUCT_HEADER

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <bitset>

#include <boost/integer.hpp>
#include <addons/color.hpp>

#define DEBUG_VAR(x) std::cout << "\033[1;31m" << "  DEBUG_VAR: " << "\033[0;31m" << #x << " = " << x << "\033[0m" << std::endl;
#define DEBUG_MSG(x) std::cout << "\033[1;31m" << "  DEBUG_MSG: " << "\033[0;31m" << x << "\033[0m" << std::endl;

//perimeter is documented in grid_class.hpp
namespace perimeter {
    ///  \brief namespace for all compiletime information
    ///  
    ///  contains all enums that determine the grid type, the amount of states and also the spin-states
    namespace qmc {
        ///  \brief contains the information about the states
        ///  
        ///  n_states has to be even, since a transition graph always needs a bra and a ket.
        ///  the bra and the corresponding ket have to be arranged in such a manner, that
        ///  invert_states - ket == bra and vice versa.
        ///  n_bra specifies how many transition graphes there are
        enum state_enum {
              start_state = 0
            , bra = 0
            , bra2
            , bra3
            , swap_bra1
            , swap_bra2
            , swap_ket2
            , swap_ket1
            , ket3
            , ket2
            , ket
            , n_states
            , n_bra = n_states/2
            , invert_state = n_states - 1
        };
        ///  \brief contains the information about the bonds
        ///  
        ///  the bonds infront of n_states define what neighbor relations are possible. e.g. down, left, right, up
        ///  would define a square grid. if diag_up and diag_down is added, one gets a triangular grid.
        ///  with just up, down and hori (for horizontal) one gets the hexagonal grid.
        ///  the bonds have to be arranged in such a manner, that
        ///  invert_bond - bond == opposite_bond. e.g. up and down are opposite
        enum bond_enum {
              start_bond = 1
            , me = 0
            , down
            , right
            , left
            , up
            , n_bonds
            , hori
            , diag_down
            , diag_up
            , none
            , invert_bond = n_bonds - 1 + start_bond
            , tri = 6 + start_bond
            , sqr = 4 + start_bond
            , hex = 3 + start_bond
        };
        ///  \brief contains the information about the spins
        ///  
        ///  the spins have to be arranged in such a manner, that
        ///  invert_spin - spin == opposite_spin
        enum spin_enum {
              start_spin = 0
            , beta = 0
            , alpha
            , n_spins
            , invert_spin = n_spins - 1
        };
        
    }
    ///  \brief the type of the sites in the grid

    typedef int spin_type; ///< the spin type, for now just an int
    typedef uint loop_type; ///< used for the loop label
    typedef uint bond_type; ///< based on bond_enum, but since the enum is not usable as an index, its an uint
    typedef std::bitset<qmc::n_states> check_type; ///< used for the check variable. a bool would also work, but an uint8_t needs the same space and provides more option
    typedef uint state_type; ///< names the type of the state. again, casting from and to enum all the time would be cumbersome
    typedef uint swap_lvl_type;

    struct site_struct {
        
        ///  \brief default constructor
        site_struct(): check(0) {
            for(state_type bra = qmc::start_state; bra != qmc::n_bra; ++bra) {
                loop[bra] = 0;
                spin[bra] = qmc::beta;
                spin[qmc::invert_state - bra] = qmc::beta;
                bond[bra] = qmc::none;
                bond[qmc::invert_state - bra] = qmc::none;
            }
        }
        ///  \brief spin constructor
        ///  
        ///  does exactly the as the default, just lets you set the spin
        site_struct(spin_type const & spin_in): check(0) {
            for(state_type bra = qmc::start_state; bra != qmc::n_bra; ++bra) {
                loop[bra] = 0;
                spin[bra] = spin_in;
                spin[qmc::invert_state - bra] = spin_in;
                bond[bra] = qmc::none;
                bond[qmc::invert_state - bra] = qmc::none;
            }
        }
        ///  \brief returns the neightbor of state of (*this)
        ///  
        ///  @param state names the state in which one wants to know the entanglement-parter
        site_struct * partner(bond_type const state) {
            return neighbor[bond[state]];
        }
        ///  \brief prints the spin of state s12 (12 bc it can be bra or ket) to os
        void print(state_type const & s12 = qmc::start_state, std::ostream & os = std::cout) const {
            os << spin[s12];
            //~ os << swap_lvl[s12];
        }
        ///  \brief the fancy print-function used by the grid_class
        std::vector<std::string> const string_print(uint const & L, state_type const & s1, uint const & what) const {
            std::vector<std::string> res;
            std::stringstream os;
            
            if(qmc::n_bonds == qmc::sqr) {
                os << "  " << print_bond(qmc::up, "|", "", s1, what) << std::left << std::setw(3) << loop[s1]%1000 << std::right << print_bond(qmc::up, "", " ", s1, what);
                res.push_back(os.str()); 
                os.str("");//reset ss
                os << print_bond(qmc::left, "--", "  ", s1, what) << print_spin(s1, what) << print_bond(qmc::right, "---", "   ", s1, what);
                res.push_back(os.str());
                os.str("");//reset ss
                os << "  " << print_bond(qmc::down, "|", " ", s1, what) << " " << "  ";
                res.push_back(os.str());
                return res;
            }
            if(qmc::n_bonds == qmc::tri) {
                os << print_bond(qmc::diag_up, "\\", " ", s1, what) << " " << print_bond(qmc::up, "/", "", s1, what) << std::left << std::setw(3) << loop[s1]%1000 << std::right << print_bond(qmc::up, "", " ", s1, what);
                res.push_back(os.str()); 
                os.str("");//reset ss
                os << print_bond(qmc::left, "--", "  ", s1, what) << print_spin(s1, what) << print_bond(qmc::right, "---", "   ", s1, what);
                res.push_back(os.str());
                os.str("");//reset ss
                os << "  " << print_bond(qmc::down, "/", " ", s1, what) << " " << print_bond(qmc::diag_down, "\\ ", "  ", s1, what);
                res.push_back(os.str());
                return res;
            }
            if(qmc::n_bonds == qmc::hex) {
                static uint alternate = true;
                
                if(alternate%2) {
                    os << "    " << print_bond(qmc::up, "\\", " ", s1, what) << "    ";
                    res.push_back(os.str()); 
                    os.str("");//reset ss
                    os << " " << std::setw(3) << loop[s1]%1000 << " " << print_spin(s1, what) << print_bond(qmc::hori, "---", "   ", s1, what);
                    res.push_back(os.str());
                    os.str("");//reset ss
                    os << "    " << print_bond(qmc::down, "/", " ", s1, what) << "    ";
                    res.push_back(os.str());
                }
                else {
                    os << "   " << print_bond(qmc::up, "/", " ", s1, what) << "     ";
                    res.push_back(os.str()); 
                    os.str("");//reset ss
                    os << print_bond(qmc::hori, "--", "  ", s1, what) << print_spin(s1, what) << " " << std::left << std::setw(3) << loop[s1]%1000 << std::right << "  ";
                    res.push_back(os.str());
                    os.str("");//reset ss
                    os << "   " << print_bond(qmc::down, "\\", " ", s1, what) << "     ";
                    res.push_back(os.str());
                }
                ++alternate;
                if(alternate % (L + 1) == 0)
                    ++alternate;
                return res;
            }
            
            
        }
        ///  \brief returns how many lines a fancy plot needs
        constexpr static const uint print_site_height() {
            return 3;
        }
        
        spin_type spin[qmc::n_states]; ///< looplabel for state
        loop_type loop[qmc::n_bra]; ///< looplabel for each transitiongraph
        bond_type bond[qmc::n_states]; ///< bond-direction for each state
        site_struct * neighbor[qmc::n_bonds]; ///< pointes structure to determine neighbor relations. same for all states
        check_type check; ///< shared by all states
        swap_lvl_type swap_lvl[qmc::n_states];
    private:
        ///  \brief plots the bonds in differente colors, depending how the config is
        std::string print_bond(qmc::bond_enum b, std::string go, std::string no, state_type const & s1, uint const & what) const {
            std::stringstream res;
            state_type s2 = qmc::invert_state - s1;
            
            if(what == 0) { //print bra and ket
                if(bond[s1] == b and bond[s2] == b)
                    res << WHITE << go << NONE;
                else
                    if(bond[s1] == b)
                        res << MAGENTA << go << NONE;
                    else
                        if(bond[s2] == b)
                            res << GREEN << go << NONE;
                        else
                            res << no;
            }
            else if(what == 1) { //print bra only
                if(bond[s1] == b)
                    res << MAGENTA << go << NONE;
                else
                    res << no;
            }
            else if(what == 2) { //print ket only
                if(bond[s2] == b)
                    res << GREEN << go << NONE;
                else
                    res << no;
            }
                
            return res.str();
        }
        
        std::string print_spin(state_type const & s1, uint const & what) const {
            std::stringstream res;
            
            state_type s2 = qmc::invert_state - s1;
            
            if(what == 0) { //print bra and ket
                if(spin[s1] != spin[s2])
                    res << YELLOWB << "X" << NONE;
                else
                    res << (spin[s1] == 0 ? BLUEB : REDB) << spin[s1] << NONE;
            }
            else if(what == 1) { //print bra only
                res << (spin[s1] == 0 ? BLUEB : REDB) << spin[s1] << NONE;
            }
            else if(what == 2) { //print ket only
                res << (spin[s2] == 0 ? BLUEB : REDB) << spin[s2] << NONE;
            }
            
            return res.str();
        }
    };
    
    std::ostream & operator<<(std::ostream & os, site_struct const & site) {
        site.print(qmc::start_state, os);
        return os;
    }
}//end namespace perimeter
#endif //__SITE_STRUCT_HEADER
