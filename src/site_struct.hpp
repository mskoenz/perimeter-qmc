// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    18.04.2013 11:08:01 EDT
// File:    site_struct.hpp

#ifndef __SITE_STRUCT_HEADER
#define __SITE_STRUCT_HEADER

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include <boost/integer.hpp>
#include <addons/color.hpp>

#define DEBUG_VAR(x) std::cout << "\033[1;31m" << "  DEBUG_VAR: " << "\033[0;31m" << #x << " = " << x << "\033[0m" << std::endl;
#define DEBUG_MSG(x) std::cout << "\033[1;31m" << "  DEBUG_MSG: " << "\033[0;31m" << x << "\033[0m" << std::endl;

//perimeter is documented in grid_class.hpp
namespace perimeter {
    namespace qmc
    {
        enum state_enum {
              bra = 0
            , bra2
            //~ , bra3
            //~ , bra4
            //~ , bra5
            //~ , ket5
            //~ , ket4
            //~ , ket3
            , ket2
            , ket
            , n_states
            , n_bra = n_states/2
            , invert_state = n_states - 1
        };
        
        enum bond_enum {
              start = 0
            , down = 0
            , right
            , left
            , up
            , n_bonds
            , hori
            , diag_down
            , diag_up
            , none
            , invert_bond = n_bonds - 1
        };
        enum spin_enum
        {
            beta = 0
            , alpha
            , n_spins
            , invert_spin = n_spins - 1
        };
        
    }
    
    struct site_struct {
        typedef int spin_type;
        typedef uint loop_type;
        typedef uint bond_type;
        typedef uint8_t check_type;
        typedef uint state_type;
        
        site_struct(): check(0) {
            for(state_type bra = qmc::start; bra != qmc::n_bra; ++bra) {
                loop[bra] = 0;
                spin[bra] = qmc::beta;
                bond[bra] = qmc::none;
                bond[qmc::invert_state - bra] = qmc::none;
            }
        }
        site_struct(spin_type const & spin_in): check(0) {
            for(state_type bra = qmc::start; bra != qmc::n_bra; ++bra) {
                loop[bra] = 0;
                spin[bra] = spin_in;
                bond[bra] = qmc::none;
                bond[qmc::invert_state - bra] = qmc::none;
            }
        }
        site_struct * partner(bond_type const state) {
            return neighbor[bond[state]];
        }
        void print(state_type const & s12 = qmc::start, std::ostream & os = std::cout) const {
            os << spin[s12];
        }
        std::string print_bond(qmc::bond_enum b, std::string go, std::string no, state_type const & s1) const {
            std::stringstream res;
            state_type s2 = qmc::invert_state - s1;
            
            if(bond[s1] == b and bond[s2] == b)
                res << WHITE << go << NONE;
            else
                if(bond[s1] == b)
                    res << YELLOW << go << NONE;
                else
                    if(bond[s2] == b)
                        res << GREEN << go << NONE;
                    else
                        res << no;
            return res.str();
        }
        std::vector<std::string> const string_print(uint const & L, state_type const & s1) const {
            std::vector<std::string> res;
            std::stringstream os;
            
            if(qmc::n_bonds == 4) {
                os << "  " << print_bond(qmc::up, "|", "", s1) << std::left << std::setw(3) << loop[s1]%1000 << std::right << print_bond(qmc::up, "", " ", s1);
                res.push_back(os.str()); 
                os.str("");//reset ss
                os << print_bond(qmc::left, "--", "  ", s1) << (spin[s1] == 0 ? BLUEB : REDB) << spin[s1] << NONE << print_bond(qmc::right, "---", "   ", s1);
                res.push_back(os.str());
                os.str("");//reset ss
                os << "  " << print_bond(qmc::down, "|", " ", s1) << " " << "  ";
                res.push_back(os.str());
                return res;
            }
            if(qmc::n_bonds == 6) {
                os << print_bond(qmc::diag_up, "\\", " ", s1) << " " << print_bond(qmc::up, "/", "", s1) << std::left << std::setw(3) << loop[s1]%1000 << std::right << print_bond(qmc::up, "", " ", s1);
                res.push_back(os.str()); 
                os.str("");//reset ss
                os << print_bond(qmc::left, "--", "  ", s1) << (spin[s1] == 0 ? BLUEB : REDB) << spin[s1] << NONE << print_bond(qmc::right, "---", "   ", s1);
                res.push_back(os.str());
                os.str("");//reset ss
                os << "  " << print_bond(qmc::down, "/", " ", s1) << " " << print_bond(qmc::diag_down, "\\ ", "  ", s1);
                res.push_back(os.str());
                return res;
            }
            if(qmc::n_bonds == 3) {
                static uint alternate = true;
                
                if(alternate%2) {
                    os << "    " << print_bond(qmc::up, "\\", " ", s1) << "    ";
                    res.push_back(os.str()); 
                    os.str("");//reset ss
                    os << " " << std::setw(3) << loop[s1]%1000 << " " << (spin[s1] == 0 ? BLUEB : REDB) << spin[s1] << NONE << print_bond(qmc::hori, "---", "   ", s1);
                    res.push_back(os.str());
                    os.str("");//reset ss
                    os << "    " << print_bond(qmc::down, "/", " ", s1) << "    ";
                    res.push_back(os.str());
                }
                else {
                    os << "   " << print_bond(qmc::up, "/", " ", s1) << "     ";
                    res.push_back(os.str()); 
                    os.str("");//reset ss
                    os << print_bond(qmc::hori, "--", "  ", s1) << (spin[s1] == 0 ? BLUEB : REDB) << spin[s1] << NONE << " " << std::left << std::setw(3) << loop[s1]%1000 << std::right << "  ";
                    res.push_back(os.str());
                    os.str("");//reset ss
                    os << "   " << print_bond(qmc::down, "\\", " ", s1) << "     ";
                    res.push_back(os.str());
                }
                ++alternate;
                if(alternate % (L + 1) == 0)
                    ++alternate;
                return res;
            }
            
            
        }
        constexpr static const uint print_site_height() {
            return 3;
        }
        
        spin_type spin[qmc::n_bra];
        loop_type loop[qmc::n_bra];
        bond_type bond[qmc::n_states];
        site_struct * neighbor[qmc::n_bonds];
        check_type check;
    };
    
    std::ostream & operator<<(std::ostream & os, site_struct const & site) {
        site.print(qmc::start, os);
        return os;
    }
}//end namespace perimeter
#endif //__SITE_STRUCT_HEADER
