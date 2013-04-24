// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    18.04.2013 11:08:01 EDT
// File:    site_struct.hpp

#ifndef __SITE_STRUCT_HEADER
#define __SITE_STRUCT_HEADER

#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/integer.hpp>
#include <addons/color.hpp>

namespace perimeter {
    namespace qmc
    {
        enum state_enum {
              bra = 0
            , ket
            , n_states
        };
        
        enum bond_enum {
              start = 0
            , down = 0
            , right
            , left
            , up
            , n_bonds
            , none
            , invert = up //has to be the last item before n_bonds
        };
    }
    
    struct site_struct {
        typedef int spin_type;
        typedef int loop_type;
        typedef uint bond_type;

        site_struct(): spin(0), loop(0), bond{qmc::none, qmc::none} {
        }
        site_struct(spin_type const spin): spin(spin), loop(0), bond{qmc::none, qmc::none} {
        }
        void print(std::ostream & os = std::cout) const {
            os << spin;
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
        std::vector<std::string> const string_print() const {
            std::vector<std::string> res;
            std::stringstream os;
            os << " " << std::setw(1) << (int(loop/100) == 0 ? ' ' : char('0' + int(loop/100))) << print_bond(qmc::up, "|", "") << std::setw(2) << std::setfill('0') << loop%100 << std::setfill(' ') << print_bond(qmc::up, "", " ");
            res.push_back(os.str());
            os.str("");//reset ss
            os << print_bond(qmc::left, "--", "  ") << (spin == 0 ? BLUEB : REDB) << spin << NONE << print_bond(qmc::right, "--", "  ");
            res.push_back(os.str());
            os.str("");//reset ss
            os << "  " << print_bond(qmc::down, "|", " ") << "  ";
            res.push_back(os.str());
            return res;
        }
        constexpr static const uint print_site_height() {
            return 3;
        }
        
        spin_type spin;
        loop_type loop;
        bond_type bond[qmc::n_states];
        site_struct * neighbor[qmc::n_bonds];
    };
    
    std::ostream & operator<<(std::ostream & os, site_struct const & site) {
        site.print(os);
        return os;
    }
}//end namespace perimeter
#endif //__SITE_STRUCT_HEADER
