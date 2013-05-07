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

std::map<uint, uint> loop_dist1;
std::map<uint, uint> loop_dist2;

std::vector<std::pair<uint, uint>> build_index(grid_class & grid) {
    //=================== init loop_dist ===================
    std::for_each(grid.begin(), grid.end(),
        [&](site_struct & s){
            ++loop_dist1[s.loop[qmc::bra]];
            ++loop_dist2[s.loop[qmc::bra2]];
        }
    );
    
    uint loop1 = grid.n_loops(qmc::bra);
    uint loop2 = grid.n_loops(qmc::bra2);
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
void recursion(std::vector<std::pair<uint, uint>> index, grid_class & grid, addon::timer_class<addon::normal> & timer, std::map<uint, uint> & accum) {
    //~ static uint ws1 = grid.n_windH(qmc::bra) + grid.n_windL(qmc::bra);
    //~ static uint ws2 = grid.n_windH(qmc::bra2) + grid.n_windL(qmc::bra2);
    if(index.size() == 0) {
        grid.state_swap(qmc::ket, qmc::ket2);
        ++accum[grid.n_all_loops()];
        //~ grid.print_all();
        grid.state_swap(qmc::ket, qmc::ket2);
        
        //~ if(ws1 != grid.n_windH(qmc::bra) + grid.n_windL(qmc::bra)) {
            //~ std::cout << "fail: " << ws1 << " " << grid.n_windH(qmc::bra) + grid.n_windL(qmc::bra) << std::endl;
        //~ }
        //~ if(ws2 != grid.n_windH(qmc::bra2) + grid.n_windL(qmc::bra2)) {
            //~ std::cout << "fail: " << ws2 << " " << grid.n_windH(qmc::bra2) + grid.n_windL(qmc::bra2) << std::endl;
        //~ }
        timer.progress(rec_done);
        ++rec_done;
        return;
    }
    uint loop = index.back().first;
    uint bra = index.back().second;
    
    auto index2 = index;
    index2.pop_back();
    
    //~ std::cout << "0";
    recursion(index2, grid, timer, accum);
    if(bra == qmc::bra)
        if(loop_dist1[loop] == 2)
            return;
    if(bra == qmc::bra2)
        if(loop_dist2[loop] == 2)
            return;

    grid.change_loop(bra, loop);
    recursion(index2, grid, timer, accum);
    grid.change_loop(bra, loop);
    
}
double get_config_mean(std::map<uint, uint> & accum) {
    double mean = 0;
    double count = 0;
    std::for_each(accum.begin(), accum.end(),
        [&](std::pair<const uint, uint> & s){
            mean += s.first * s.second;
            count += s.second;
        }
    );
    return mean/count;
}
int main(int argc, char* argv[])
{
    if(argc < 5) {
        std::cout << "not enough numbers!" << std::endl;
        return 0;
    }
    uint H = atoi(argv[1]);
    uint L = atoi(argv[2]);
    uint64_t nr1 = atol(argv[3]);
    uint64_t nr2 = atol(argv[4]);
    
    //=================== parse ===================
    loop_real_class L1(H, L, nr1);
    loop_real_class L2(H, L, nr2);
    loop_real_class LX(L1 ^ L2);
    loop_real_class LA(L1 & L2);
    //~ //=================== grid setup ===================
    //~ std::cout << "grid..." << std::endl;
    
    grid_class grid(H, L);
    grid.read_in(qmc::bra, L1);
    grid.read_in(qmc::bra2, L2);
    
    if(argc > 6) {
        L1.print(1+16);
        L2.print(1+16);
        LX.print(1+16);
        return 0;
    }
    //~ grid.print_all();
    //~ grid.state_swap(qmc::bra, qmc::bra2);
    //~ grid.print_all();
    //~ std::cout << grid.n_loops(qmc::bra) << std::endl;
    //~ std::cout << grid.n_loops(qmc::bra2) << std::endl;
    //~ std::cout << grid.n_all_loops() << std::endl;
    //~ LX.print(16+1);
    //~ std::cout << grid.n_cross_loops(qmc::bra, qmc::bra2) << std::endl;
    //~ std::cout << grid.n_cross_loops(qmc::bra, qmc::bra2, 2) << std::endl;
    //~ 
    //~ //=================== recursion ===================
    //~ std::cout << "recursion..." << std::endl;
    addon::timer_class<addon::normal> timer(std::pow(2, grid.n_all_loops()));
    
    std::map<uint, uint> accum;
    auto idx = build_index(grid);
    recursion(idx, grid, timer, accum);
    
    //=================== calculating config_mean ===================
    double config_mean = get_config_mean(accum);
    //=================== print ===================
    std::string name = "out.txt";
    if(argc >= 6)
        name = std::string(argv[5]);
    std::ofstream of(name, std::ios_base::app);
    
    of << std::setw(2) << grid.n_loops(qmc::bra) << "/" << std::setw(2) << grid.n_loops(qmc::bra2);
    of << " cmean: " << std::setw(8) << config_mean;
    of << " cross: " << std::setw(2) << grid.n_cross_loops(qmc::bra, qmc::bra2);
    of << "/" << std::setw(2) << grid.n_cross_loops(qmc::bra, qmc::bra2, 2);
    of << " LA: " << std::setw(2) << LA.n_loops() << "/" << std::setw(2) << LA.n_loops(1);
    of << " LX: " << LX.n_loops() << "/" << std::setw(2) << LX.n_loops(1);
    of << " wS1: " << std::setw(2) << L1.windH() << "/" << std::setw(2) << L1.windL();
    of << " wS2: " << std::setw(2) << L2.windH() << "/" << std::setw(2) << L2.windL();
    of << " idw: " << identical_winding(L1, L2);
    of << " xr: " << LX.vert_count(0) + LX.vert_count(1)*100 + LX.vert_count(2)*10000 + LX.vert_count(3) * 1000000;
    of << " xr2: " << LX.vert_count(4) + LX.vert_count(5)*100 + LX.vert_count(6)*10000;
    of << " an: " << LA.vert_count(0) + LA.vert_count(1)*100 + LA.vert_count(2)*10000 + LA.vert_count(3) * 1000000;
    of << " an2: " << LA.vert_count(4) + LA.vert_count(5)*100 + LA.vert_count(6)*10000;
    of << "             poss: " << std::setw(11) << std::pow(2, grid.n_all_loops());
    of << " max: "  << std::setw(3) << 2 * (H * L / 2);
    of << " " << nr1 << "/" << nr2;
    of << std::endl;
    //~ //=================== done ===================
    //~ std::cout << "job done" << std::endl;
    return 0;
}
