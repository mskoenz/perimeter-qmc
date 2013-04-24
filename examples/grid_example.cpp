// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.04.2013 20:36:35 EDT
// File:    grid_example.cpp

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

    addon::parameter.set("H", 4); //set with default
    addon::parameter.set("L", 10);
    addon::parameter.set("init", 1); //0 == horizontal bonds / 1 == vertical bonds
    addon::parameter.read(argc, argv);
    
    //~ grid_class<site_struct> g(10, 10);
    //~ int i = 0;
    //~ std::for_each(g.begin(), g.end(), [&](int & g) {g = i++;});
    //~ g.print();
    //~ g.print_all();

    //~ sim_class s(parameter.get());
    //~ s.two_bond_update(3, 2, qmc::bra);
    //~ s.two_bond_update(3, 2, qmc::ket);
    //~ s.two_bond_update(0, 2, qmc::bra);
    //~ s.two_bond_update(0, 2, qmc::ket);
    //~ s.two_bond_update(3, 2, qmc::bra);
    //~ s.two_bond_update(0, 9, qmc::ket);
    
    double H;
    double L;
    
    //~ for(uint j = 1; j < 50; ++j)
    //~ {
        addon::parameter.set(H, "H", 128);
        addon::parameter.set(L, "L", 128);
        addon::random_class<int, addon::fibonacci> rngH(0, H);
        addon::random_class<int, addon::fibonacci> rngL(0, L);
        sim_class s(addon::parameter.get());
        std::cout << sizeof(s) << std::endl;
        std::cout << sizeof(double) << std::endl;
        double acc = 0;
        double N = 100000000;
        addon::timer_class<addon::data> timer(N, "update_test.txt");
        timer.set_names("accept", "updates/us", "L", "H");
        for(uint i = 0; i < N; ++i)
        {
            acc += s.two_bond_update(rngH(), rngL(), i%2);
            timer.progress(i);
        }
        timer.write(acc/N, N/(1000000*timer), L, H);
        timer.print(acc/N, N/(1000000*timer), L, H);
    //~ }
        
    return 0;
}
