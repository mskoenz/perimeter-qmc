// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    18.04.2013 20:33:34 EDT
// File:    rng_example.cpp

#include <iostream>
#include <boost/integer.hpp>
#include <timer2_msk.hpp>
#include <random2_msk.hpp>

using namespace std;

int main(int argc, char* argv[])
{
    typedef addon::mersenne RNG;
    //~ typedef addon::mersenne_fast RNG;
    //~ typedef addon::fibonacci RNG;
    typedef int T;
    uint64_t maxi = 1000000000lu;
    int prefactor = 1000000;
    addon::random_class<T, RNG> rng(0, 6);
    rng.seed(0);
    
    addon::timer_class<addon::data> timer(maxi, "rng_test.txt");
    timer.set_names("freq[1/us]", "rng[-]", "type[-]");
    timer.set_comment("rng testing");
    for(long unsigned int i = 0; i < maxi; ++i)
    {
        rng();
        timer.progress(i);
    }
    timer.print(maxi / (prefactor * timer), typeid(RNG).name(), typeid(T).name());
    timer.write(maxi / (prefactor * timer), typeid(RNG).name(), typeid(T).name());
    for(long unsigned int i = 0; i < 10; ++i)
    {
        cout << rng() << endl;
    }
    return 0;
}

/*


18.04.13 at 23:35:18   rng testing  Time[s]: 4.45  rn/us: 224.719  rng: 8mersenne  type: i  

18.04.13 at 23:35:46   rng testing  Time[s]: 4.68  rn/us: 213.675  rng: 8mersenne  type: i  

18.04.13 at 23:35:53   rng testing  Time[s]: 4.92  rn/us: 203.252  rng: 8mersenne  type: i  

18.04.13 at 23:36:20   rng testing  Time[s]: 8.43  rn/us: 118.624  rng: 9fibonacci  type: i  

18.04.13 at 23:36:32   rng testing  Time[s]: 8.41  rn/us: 118.906  rng: 9fibonacci  type: i  

18.04.13 at 23:39:28   rng testing  Time[s]: 8.43  rn/us: 118.624  rng: 9fibonacci  type: i  

18.04.13 at 23:40:07   rng testing  Time[s]: 4.54  rn/us: 220.264  rng: 13mersenne_fast  type: i  

18.04.13 at 23:40:14   rng testing  Time[s]: 4.74  rn/us: 210.97  rng: 13mersenne_fast  type: i  

18.04.13 at 23:40:54   rng testing  Time[s]: 4.68  rn/us: 213.675  rng: 8mersenne  type: i  

18.04.13 at 23:41:00   rng testing  Time[s]: 4.67  rn/us: 214.133  rng: 8mersenne  type: i  


*/
