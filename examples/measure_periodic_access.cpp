// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.04.2013 10:44:18 EDT
// File:    measure_periodic_access.cpp

#include <iostream>
#include <grid_class.hpp>
#include <addons/timer2_msk.hpp>

/*need to be copied into the left/right/up/down fct for measuring
//~ return (*grid_)[(i - 1 + L_)&(H_-1)][j];
//~ return (*grid_)[(i - 1 + L_)%H_][j];
//~ if(i == 0)
    //~ return (*grid_)[H_ - 1][j];
//~ else
    //~ return (*grid_)[i - 1][j];

//~ return (*grid_)[(i + 1)&(H_-1)][j];
//~ return (*grid_)[(i + 1)%H_][j];
//~ if(i == H_ - 1)
    //~ return (*grid_)[0][j];
//~ else
    //~ return (*grid_)[i + 1][j];

//~ return (*grid_)[i][(j + 1)&(L_-1)];
//~ return (*grid_)[i][(j + 1)%L_];
//~ 
//~ if(j == L_ - 1)
    //~ return (*grid_)[i][0];
//~ else
    //~ return (*grid_)[i][j + 1];

//~ return (*grid_)[i][(j - 1 + L_)&(L_-1)];
//~ return (*grid_)[i][(j - 1 + L_)%L_];
//~ if(j == 0)
    //~ return (*grid_)[i][L_ - 1];
//~ else
    //~ return (*grid_)[i][j - 1];
*/

using namespace std;
using namespace perimeter;

int main(int argc, char* argv[])
{
    long unsigned int res = 0;
    long unsigned int maxk = 10000000lu; // divide by 100 for O2
    uint H = 16;
    uint L = 8;
    grid_class<int> g(H, L);
    int i = 0;
    std::for_each(g.begin(), g.end(), [&](int & g) {g = i++;});
    g.print();
    
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
                //~ res += g.up(i, j);
                //~ res += g.down(i, j);
                //~ res += g.left(i, j);
                //~ res += g.right(i, j);
            }
        }
    }
    std::cout << res << std::endl;
    timer.set_comment("addition O3");
    timer.print(timer.elapsed() * 1000 * 1000 * 1000.0 / (maxk * L * H * 4));
    timer.write(timer.elapsed() * 1000 * 1000 * 1000.0 / (maxk * L * H * 4));
    return 0;
}

/*
 
17.04.13 at 20:03:28	callOnly O0	ns per lookup: 3.94531		20.2s

17.04.13 at 20:03:35	callOnly O1	ns per lookup: 0.0292969		0.15s

17.04.13 at 20:04:07	callOnly O2		0s

17.04.13 at 20:04:20	callOnly O3		0s

17.04.13 at 20:04:52	sumOnly O0	ns per lookup: 2.5957		13.29s

17.04.13 at 20:04:59	sumOnly O1	ns per lookup: 0.03125		0.16s

17.04.13 at 20:05:17	sumOnly O2		0s

17.04.13 at 20:05:35	sumOnly O3		0s

17.04.13 at 20:07:10	lookup const O0	ns per lookup: 128.711		6.59s

17.04.13 at 20:07:22	lookup const O1	ns per lookup: 4.29688		0.22s

17.04.13 at 20:07:48	lookup const O1	ns per lookup: 3.72266		19.06s

17.04.13 at 20:08:13	lookup const O2	ns per lookup: 3.14062		16.08s

17.04.13 at 20:08:33	lookup const O3	ns per lookup: 1.92578		9.86s

17.04.13 at 20:09:11	lookup ref O0	ns per lookup: 134.961		6.91s

17.04.13 at 20:09:38	lookup ref O1	ns per lookup: 3.69922		18.94s

17.04.13 at 20:10:01	lookup ref O2	ns per lookup: 3.14062		16.08s

17.04.13 at 20:10:17	lookup ref O3	ns per lookup: 1.94922		9.98s

17.04.13 at 20:13:20	ifelse O0	ns per lookup: 125.391		6.42s

17.04.13 at 20:13:55	ifelse O1	ns per lookup: 5.72461		29.31s

17.04.13 at 20:14:26	ifelse O2	ns per lookup: 3.30859		16.94s

17.04.13 at 20:14:50	ifelse O3	ns per lookup: 3.18945		16.33s

17.04.13 at 20:16:32	mod O0	ns per lookup: 134.57		6.89s

17.04.13 at 20:17:28	mod O1	ns per lookup: 9.63867		49.35s

17.04.13 at 20:18:22	mod O2	ns per lookup: 9.14844		46.84s

17.04.13 at 20:19:08	mod O3	ns per lookup: 6.29883		32.25s

17.04.13 at 20:19:34	bin O0	ns per lookup: 130.078		6.66s

17.04.13 at 20:20:03	bin O1	ns per lookup: 4.04883		20.73s

17.04.13 at 20:20:25	bin O2	ns per lookup: 2.74219		14.04s

17.04.13 at 20:20:41	bin O3	ns per lookup: 1.76172		9.02s


*/
