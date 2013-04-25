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

//perimeter is documented in grid_class.hpp
namespace perimeter {
    namespace qmc
    {
        enum state_enum {
              bra = 0
            , ket
            , n_states
            , invert_state = ket
        };
        
        enum bond_enum {
              start = 0
            , down = 0
            , right
            , diag_down
            , diag_up
            , left
            , up
            , n_bonds
            , hori
            , none
            , invert_bond = up //has to be the last item before n_bonds
        };
        enum spin_enum
        {
            beta = 0
            , alpha
            , n_spins
            , invert_spin = alpha
        };
        
    }
    
    struct site_struct {
        typedef int spin_type;
        typedef uint loop_type;
        typedef uint bond_type;
        typedef uint8_t check_type;
        typedef uint state_type;
        
        site_struct(): spin(qmc::beta), loop(0), bond{qmc::none, qmc::none}, check(0) {
        }
        site_struct(spin_type const spin): spin(spin), loop(0), bond{qmc::none, qmc::none}, check(0) {
        }
        site_struct * partner(bond_type const state) {
            return neighbor[bond[state]];
        }
        void print(std::ostream & os = std::cout) const {
            //~ os << spin;
            os << int(check);
        }
        std::string print_bond(qmc::bond_enum b, std::string go, std::string no) const {
            std::stringstream res;
            if(bond[qmc::bra] == b and bond[qmc::ket] == b)
                res << WHITE << go << NONE;
            else
                if(bond[qmc::bra] == b)
                    res << YELLOW << go << NONE;
                else
                    if(bond[qmc::ket] == b)
                        res << GREEN << go << NONE;
                    else
                        res << no;
            return res.str();
        }
        std::vector<std::string> const string_print(uint const & L) const {
            std::vector<std::string> res;
            std::stringstream os;
            
            if(qmc::n_bonds == 4) {
                os << "  " << print_bond(qmc::up, "|", "") << std::left << std::setw(3) << loop%1000 << std::right << print_bond(qmc::up, "", " ");
                res.push_back(os.str()); 
                os.str("");//reset ss
                os << print_bond(qmc::left, "--", "  ") << (spin == 0 ? BLUEB : REDB) << spin << NONE << print_bond(qmc::right, "---", "   ");
                res.push_back(os.str());
                os.str("");//reset ss
                os << "  " << print_bond(qmc::down, "|", " ") << " " << "  ";
                res.push_back(os.str());
                return res;
            }
            if(qmc::n_bonds == 6) {
                os << print_bond(qmc::diag_up, "\\", " ") << " " << print_bond(qmc::up, "/", "") << std::left << std::setw(3) << loop%1000 << std::right << print_bond(qmc::up, "", " ");
                res.push_back(os.str()); 
                os.str("");//reset ss
                os << print_bond(qmc::left, "--", "  ") << (spin == 0 ? BLUEB : REDB) << spin << NONE << print_bond(qmc::right, "---", "   ");
                res.push_back(os.str());
                os.str("");//reset ss
                os << "  " << print_bond(qmc::down, "/", " ") << " " << print_bond(qmc::diag_down, "\\ ", "  ");
                res.push_back(os.str());
                return res;
            }
            if(qmc::n_bonds == 3) {
                static uint alternate = true;
                
                if(alternate%2) {
                    os << "    " << print_bond(qmc::up, "\\", " ") << "    ";
                    res.push_back(os.str()); 
                    os.str("");//reset ss
                    os << " " << std::setw(3) << loop%1000 << " " << (spin == 0 ? BLUEB : REDB) << spin << NONE << print_bond(qmc::hori, "---", "   ");
                    res.push_back(os.str());
                    os.str("");//reset ss
                    os << "    " << print_bond(qmc::down, "/", " ") << "    ";
                    res.push_back(os.str());
                }
                else {
                    os << "   " << print_bond(qmc::up, "/", " ") << "     ";
                    res.push_back(os.str()); 
                    os.str("");//reset ss
                    os << print_bond(qmc::hori, "--", "  ") << (spin == 0 ? BLUEB : REDB) << spin << NONE << " " << std::left << std::setw(3) << loop%1000 << std::right << "  ";
                    res.push_back(os.str());
                    os.str("");//reset ss
                    os << "   " << print_bond(qmc::down, "\\", " ") << "     ";
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
        
        spin_type spin;
        loop_type loop;
        bond_type bond[qmc::n_states];
        site_struct * neighbor[qmc::n_bonds];
        check_type check;
    };
    
    std::ostream & operator<<(std::ostream & os, site_struct const & site) {
        site.print(os);
        return os;
    }
}//end namespace perimeter
#endif //__SITE_STRUCT_HEADER
