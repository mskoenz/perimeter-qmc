// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    18.04.2013 18:21:10 EDT
// File:    measure_periodic_access_2.cpp

#include <iostream>
#include <grid_class.hpp>
#include <addons/timer2_msk.hpp>

using namespace std;
using namespace perimeter;

struct neighbor_struct
{
    int value;
    neighbor_struct * up;
    neighbor_struct * down;
    neighbor_struct * right;
    neighbor_struct * left;
};
std::ostream & operator<<(std::ostream & os, neighbor_struct const n)
{
    os << n.value;
    return os;
}

void setup(grid_class<neighbor_struct> & grid, uint L, uint H)
{
    for(uint i = 0; i < H; ++i)
    {
        for(uint j = 0; j < L; ++j)
        {
            grid(i, j).up = &grid((i+H-1)%H, j);
            grid(i, j).down = &grid((i+1)%H, j);
            grid(i, j).left = &grid(i, (j+L-1)%L);
            grid(i, j).right = &grid(i, (j+1)%L);
            grid(i, j).value = L*i + j;
        }
    }
}

int main(int argc, char* argv[])
{
    long unsigned int res = 0;
    long unsigned int maxk = 10000000lu; // divide by 100 for O2
    uint H = 16;
    uint L = 8;
    grid_class<neighbor_struct> g(H, L);
    setup(g, L, H);
    g.print();
    cout << g(0, 0).value << endl;
    addon::timer_class<addon::normal> timer;
    timer.set_names("ns-per-lookup");
    for(uint k = 0; k < maxk; ++k)
    {
        for(uint i = 0; i < H; ++i)
        {
            for(uint j = 0; j < L; ++j)
            {
                res += 59;
                res += 59;
                res += 60;
                res += 60;
                //~ res += g(i, j).up->value;
                //~ res += g(i, j).down->value;
                //~ res += g(i, j).left->value;
                //~ res += g(i, j).right->value;
            }
        }
    }
    std::cout << res << std::endl;
    timer.set_comment("addition link O3");
    timer.print(timer.elapsed() * 1000 * 1000 * 1000.0 / (maxk * L * H * 4));
    timer.write(timer.elapsed() * 1000 * 1000 * 1000.0 / (maxk * L * H * 4));
    return 0;
}

/*
 

18.04.13 at 18:42:41	extern link O3	ns per lookup: 1.33008		6.81s
18.04.13 at 18:42:52	extern link O3	ns per lookup: 1.32227		6.77s
18.04.13 at 18:49:43	extern link O3	ns per lookup: 1.35547		6.94s

*/
