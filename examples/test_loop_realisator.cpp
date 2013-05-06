// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    02.05.2013 08:33:46 EDT
// File:    loop_to_braket_realisation_2.cpp

#include <loop_realisator.hpp>
#include <timer2_msk.hpp>

using namespace perimeter;

int main(int argc, char* argv[]) {
    //~ if(argc < 3) {
        //~ std::cout << "not enoutg files!" << std::endl;
        //~ return 0;
    //~ }
    
    if(qmc::n_bonds != qmc::sqr) {
        std::cout << "only square lattice supported" << std::endl;
        return 0;
    }
    
    //~ loop_real_class l1(argv[1]);
    //~ loop_real_class l2(argv[2]);
    //~ loop_real_class la(l1 & l2);
    //~ loop_real_class lo(l1 ^ l2);
    
    //~ loop_real_class l1(53708, 4, 2);
    //~ std::cout << "AND" << std::endl;
    //~ la.print(1+2+8);
    //~ std::cout << "XOR" << std::endl;
    //~ lo.print(1+2+8);
    
    //~ uint64_t work = 1lu<<32;
    //~ uint64_t work = 1lu<<24;
    //~ uint64_t work = 1lu<<16;
    uint64_t work = 1lu<<8;
    addon::timer_class<addon::normal> timer1(work, "test.txt");
    
    //~ loop_real_class ll(4, 4, 0);
    //~ addon::timer_class<addon::normal> timer1(work, "8x2L1.txt");
    //~ addon::timer_class<addon::normal> timer2(work, "8x2L2.txt");
    //~ addon::timer_class<addon::normal> timer3(work, "8x2L3.txt");
    //~ addon::timer_class<addon::normal> timer4(work, "8x2L4.txt");
    //~ addon::timer_class<addon::normal> timer5(work, "8x2L5.txt");
    //~ addon::timer_class<addon::normal> timer6(work, "8x2L6.txt");
    //~ addon::timer_class<addon::normal> timer7(work, "8x2L7.txt");
    //~ addon::timer_class<addon::normal> timer8(work, "8x2L8.txt");
    
    for(uint64_t i = 0; i < work; ++i) {
        //~ loop_real_class ll(8, 2, i);
        loop_real_class ll(2, 2, i);
        if(ll.conform()) {
            if(ll.n_loops() == 1)
                timer1.write(ll);
            //~ else if(ll.n_loops() == 2)
                //~ timer2.write(ll);
            //~ else if(ll.n_loops() == 3)
                //~ timer3.write(ll);
            //~ else if(ll.n_loops() == 4)
                //~ timer4.write(ll);
            //~ else if(ll.n_loops() == 5)
                //~ timer5.write(ll);
            //~ else if(ll.n_loops() == 6)
                //~ timer6.write(ll);
            //~ else if(ll.n_loops() == 7)
                //~ timer7.write(ll);
            //~ else if(ll.n_loops() == 8)
                //~ timer8.write(ll);
        }

        timer1.progress(i);
    }
    
    return 0;
}
