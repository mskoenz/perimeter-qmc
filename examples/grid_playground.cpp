// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.04.2013 20:36:35 EDT
// File:    grid_playground.cpp

#include <iostream>
#include <map>

#include <grid_class.hpp>
#include <site_struct.hpp>
#include <sim_class.hpp>
#include <bash_parameter2_msk.hpp>

#include <version/version_info.hpp>

using namespace std;
using namespace perimeter;

int main(int argc, char* argv[])
{
    version_info();

    double L;
    double H;
    addon::parameter.set(H, "H", 6); //set with default
    addon::parameter.set(L, "L", 10);
    addon::parameter.set("init", 1); //0 == horizontal bonds / 1 == vertical bonds
    addon::parameter.read(argc, argv);
    
    sim_class s(addon::parameter.get());
    int nr = -1;
    int st = 0;
    s.print();
    while(nr != -1)
    {
        std::cout << "nr s: " << std::endl;
        std::cin >> nr;
        std::cin >> st;
        s.two_bond_update(int(nr / int(L)), nr%int(L), (st == 0 ? qmc::bra : qmc::ket));
        s.print();
    }
        
    return 0;
}
