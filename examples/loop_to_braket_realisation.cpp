// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    29.04.2013 16:59:00 EDT
// File:    loop_to_braket_realisation.cpp

#include <timer2_msk.hpp>
#include <grid_class.hpp>
#include <loop_realisator.hpp>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <bitset>

using namespace perimeter;

/*
void change_loop(grid_class & grid, uint loop, uint bra) {
    site_struct * start = NULL;
    std::for_each(grid.begin(), grid.end(),
        [&](site_struct & s){
            if(start == NULL and s.loop[bra] == loop) {
                start = &s;
            }
        }
    );
    site_struct * next = grid.next_in_loop(start, bra);
    site_struct * delay = start;
    do {
        std::swap(delay->bond[bra], delay->bond[qmc::invert_state - bra]);
        delay = next;
        next = grid.next_in_loop(next, bra);
    } while(delay != start);
}
std::vector<std::pair<uint, uint>> build_index(grid_class & grid) {
    grid.init_loops();
    //=================== init loop_dist ===================
    std::for_each(grid.begin(), grid.end(),
        [&](site_struct & s){
            ++loop_dist1[s.loop[qmc::bra]];
            ++loop_dist2[s.loop[qmc::bra2]];
        }
    );
    
    uint loop1 = grid.n_loops_[qmc::bra];
    uint loop2 = grid.n_loops_[qmc::bra2];
    std::vector<std::pair<uint, uint>> res;
    for(uint i = 0; i < loop2; ++i) {
        res.push_back(std::make_pair(loop2 - 1 - i, qmc::bra2));
    }
    for(uint i = 0; i < loop1; ++i) {
        res.push_back(std::make_pair(loop1 - 1 - i, qmc::bra));
    }
    return res;
}
double rec_done = 0;
void recursion(std::vector<std::pair<uint, uint>> index, grid_class & grid, addon::timer_class<addon::normal> & timer) {
    if(index.size() == 0) {
        grid.state_swap(qmc::ket, qmc::ket2);
        //~ ++m[get_n_loops(grid)];
        //~ grid.print_all();
        grid.state_swap(qmc::ket, qmc::ket2);
        timer.progress(rec_done);
        ++rec_done;
        return;
    }
    uint loop = index.back().first;
    uint bra = index.back().second;
    
    auto index2 = index;
    index2.pop_back();
    
    //~ std::cout << "0";
    recursion(index2, grid, timer);
    //~ if(bra == qmc::bra)
        //~ if(loop_dist1[loop] == 2)
            //~ return;
    //~ if(bra == qmc::bra2)
        //~ if(loop_dist2[loop] == 2)
            //~ return;

    change_loop(grid, loop, bra);
    
    
    //~ std::cout << "1";
    recursion(index2, grid, timer);
    change_loop(grid, loop, bra);
    
}
*/
int main(int argc, char* argv[])
{
    if(argc < 3) {
        std::cout << "not enoutg files!" << std::endl;
        return 0;
    }
    
    //=================== parse ===================
    std::string file1(argv[1]);
    std::string file2(argv[2]);
    std::cout << "parse..." << std::endl;
    loop_real_class L1(file1);
    loop_real_class L2(file2);
    loop_real_class LX(L1 ^ L2);
    //=================== grid setup ===================
    std::cout << "grid..." << std::endl;
    
    grid_class grid(L1.H(), L1.L());
    grid.read_in(qmc::bra, L1);
    grid.read_in(qmc::bra2, L2);
    
    grid.print_all();
    grid.state_swap(qmc::bra, qmc::bra2);
    grid.print_all();
    std::cout << grid.n_loops() << std::endl;
    LX.print(16+1);
    std::cout << grid.n_cross_loops(qmc::bra, qmc::bra2) << std::endl;
    std::cout << grid.n_cross_loops(qmc::bra, qmc::bra2, 2) << std::endl;
    
    //=================== recursion ===================
    std::cout << "recursion..." << std::endl;
    addon::timer_class<addon::normal> timer(std::pow(2, grid.n_loops()));
    
    //~ auto idx = build_index(grid);
    //~ recursion(idx, grid, timer);
    
    //=================== calculating config_mean ===================
    //~ double config_mean;
    //~ std::for_each(m.begin(), m.end(),
        //~ [&](std::pair<const uint, uint> & s){
            //~ //if(s != *m.begin())
                //~ //of << ", ";
            //~ //of << s.first << "[" << s.second << "]";
            //~ med += s.first * s.second;
            //~ count += s.second;
        //~ }
    //~ );
    //~ //of << std::endl;
    //~ config_mean = med/count;
    //~ m.clear();
    
    //=================== print ===================
    //~ std::string name = "out.txt";
    //~ if(argc >= 4)
        //~ name = std::string(argv[3]);
    //~ std::ofstream of(name, std::ios_base::app);
    //~ int p1b = file1.rfind("/")+1;
    //~ int p2b = file2.rfind("/")+1;
    //~ int p1e = file1.rfind(".");
    //~ int p2e = file2.rfind(".");
    //~ 
    //~ of << std::setw(2) << l1 << "/" << std::setw(2) << l2;
    //~ of << " cmean: " << std::setw(8) << config_mean;
    //~ of << " lsad: " << std::setw(2) << lsad(grid).first;
    //~ of << "/" << std::setw(2) << lsad(grid).second;
    //~ of << " andl: " << std::setw(2) << multi_and_loop << "/" << std::setw(2) << single_and_loop;
    //~ of << "        xorc: " << std::setw(2) << XOR.count();
    //~ of << " con: " << std::setw(8) << conectivity;
    //~ of << "  poss: " << std::setw(11) << std::pow(2, l1 + l2);
    //~ of << " max: "  << std::setw(3) << 2 * (H * L / 2);
    //~ of << " " << file1.substr(p1b, p1e-p1b) << " " << file2.substr(p2b, p2e-p2b);
    //~ of << std::endl;
    //~ //=================== done ===================
    //~ std::cout << "job done" << std::endl;
    return 0;
}
