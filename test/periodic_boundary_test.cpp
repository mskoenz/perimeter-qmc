// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.04.2013 20:25:01 EDT
// File:    periodic_boundary_test.cpp

#include <iostream>
#include <grid_class.hpp>

using namespace std;
using namespace perimeter;

int main(int argc, char* argv[])
{
    grid_class<int> g(12, 10);
    g.print();
    int i = 0;
    std::for_each(g.begin(), g.end(), [&](int & g) {g = i++;});
    
    if(g.up(0, 0) != 110)
        return 1;
    if(g.down(0, 0) != 10)
        return 1;
    if(g.left(0, 0) != 9)
        return 1;
    if(g.right(0, 0) != 1)
        return 1;
        
    return 0;
}
