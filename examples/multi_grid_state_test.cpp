// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    26.04.2013 10:34:22 EDT
// File:    multi_grid_state_test.cpp

#include <bash_parameter2_msk.hpp>
#include <timer2_msk.hpp>
#include <random2_msk.hpp>

#include <sim_class.hpp>

#include <version/version_info.hpp>

#include <iostream>

using namespace std;
using namespace perimeter;
using namespace addon;


int main(int argc, char* argv[])
{
    double H;
    double L;
    double maxi;
    parameter.set("init", 0);
    parameter.set("p", 0);
    parameter.set(H, "H", 4);
    parameter.set(L, "L", 10);
    parameter.set(maxi, "i", 1000);
    parameter.read(argc, argv);
    
    sim_class s(parameter.get());
    
    addon::random_class<int, fibonacci> rngH(0, H);
    rngH.seed(0);
    addon::random_class<int, fibonacci> rngL(0, L);
    rngL.seed(1);
    
    addon::random_class<int, fibonacci> rngH2(0, H);
    rngH2.seed(0);
    addon::random_class<int, fibonacci> rngL2(0, L);
    rngL2.seed(1);
    
    for(uint i = 0; i < maxi; ++i)
    {
        s.two_bond_update(rngH(), rngL(), qmc::bra);
        s.two_bond_update(rngH(), rngL(), qmc::ket);
        s.two_bond_update(rngH2(), rngL2(), qmc::bra2);
        s.two_bond_update(rngH2(), rngL2(), qmc::ket2);
    }
    return 0;
}
