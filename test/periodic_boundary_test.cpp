// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.04.2013 20:25:01 EDT
// File:    periodic_boundary_test.cpp

#include <iostream>
#include <grid_class.hpp>

using namespace std;
using namespace perimeter;

int main(int argc, char* argv[])
{
    grid_class g(12, 10);
    g.print();
    int i = 0;
    std::for_each(g.begin(), g.end(), [&](site_struct & g) {g.spin = i++;});
    
    if(qmc::n_bonds == 4) {
        if(g(0, 0).neighbor[qmc::up]->spin != 110)
            return 1;
        if(g(0, 0).neighbor[qmc::down]->spin != 10)
            return 1;
        if(g(0, 0).neighbor[qmc::left]->spin != 9)
            return 1;
        if(g(0, 0).neighbor[qmc::right]->spin != 1)
            return 1;
    }
        
    return 0;
}
