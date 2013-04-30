// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    29.04.2013 16:59:00 EDT
// File:    loop_to_braket_realisation.cpp

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <grid_class.hpp>
#include <map>
#include <bitset>
#include <timer2_msk.hpp>

using namespace perimeter;

std::vector<std::string> s1_raw;
std::vector<std::string> s2_raw;
const int size = 32;
const int size2 = 4;
std::bitset<size> s1;
std::bitset<size> s2;
std::map<uint, uint> m;
std::vector<std::vector<std::bitset<size2>>> m3210;
uint H;
uint L;


void read_in(char * name, std::vector<std::string> & raw) {
    std::ifstream in(name);
    std::string temp;
    if(in.is_open()) {
        
        while(in) {
            getline(in, temp);
            if(temp.size() > 3)
                raw.push_back(temp);
        };
    }
    else {
        std::cout << "file-fail" << std::endl;
    }
    in.close();
}
void print() {
    for(uint i = 0; i < m3210.size(); ++i)
    {
        for(uint j = 0; j < m3210[i].size(); ++j)
        {
            std::cout << m3210[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
void ket_swap(grid_class & grid) {
    std::for_each(grid.begin(), grid.end(),
        [](site_struct & s){
            uint temp = s.bond[qmc::ket];
            s.bond[qmc::ket] = s.bond[qmc::ket2];
            s.bond[qmc::ket2] = temp;
        }
    );
}
uint get_n_loops(grid_class & grid) {
    grid.init_loops();
    return grid.n_loops_[qmc::bra] + grid.n_loops_[qmc::bra2];
}
void init_parse(std::vector<std::string> raw, std::bitset<size> & sx) {
    for(uint i = 0; i < H; ++i) {
        for(uint j = 0; j < L; ++j) {
            m3210[i][j] = 0;
        }
    }
    uint shift = 0;
    for(uint i = 0; i < raw.size(); ++i) {
        if(raw[i][0] == '+') {
            for(uint j = 2; j < raw[i].size(); j+= 4) {
                if(raw[i][j] == '-') {
                    m3210[i/2][(j-2)/4][qmc::right] = true;
                    m3210[i/2][((j-2)/4+1)%L][qmc::left] = true;
                    sx[shift] = 1;
                }
                ++shift;
            }
        }
        else {
            for(uint j = 0; j < raw[i].size(); j+= 4) {
                if(raw[i][j] == '|') {
                    m3210[i/2][j/4][qmc::down] = true;
                    m3210[(i/2+1)%H][j/4][qmc::up] = true;
                    sx[shift] = 1;
                }
                ++shift;
            }
        }
    }
    for(uint i = 0; i < H; ++i) {
        for(uint j = 0; j < L; ++j) {
            ++m[m3210[i][j].to_ulong()%4];
        }
    }
}
int find_lowest(uint nr) {
    int shift = 0;
    while(((nr >> shift) & 1) == 0)
        ++shift;
    return shift;
}
void follow_loop(site_struct * start, uint const & bra, grid_class & grid) {
    uint state = bra;
    site_struct * next = start;
    uint count = 0;
    do {
        ++count;
        ++(next->check);
        int lowest = find_lowest(next->loop[bra]);
        int invert = 3-lowest;
        if(std::bitset<size2>(next->loop[bra]).count() > 1)
            next->loop[bra] -= (1 << lowest);
        
        next->bond[state] = lowest;
        if(std::bitset<size2>(next->partner(state)->loop[bra]).count() > 1)
            next->partner(state)->loop[bra] -= (1 << invert);
        next->partner(state)->bond[state] = invert;
        next = next->partner(state);
        state = qmc::invert_state - state;
    } while(next != start);
}
void init_grid(grid_class & grid, uint bra) {
    //=================== clear bonds ===================
    std::for_each(grid.begin(), grid.end(),
        [&](site_struct & s){
            s.bond[bra] = qmc::none;
            s.bond[qmc::invert_state - bra] = qmc::none;
        }
    );
    //=================== copy parse to loop ===================
    for(uint i = 0; i < H; ++i) {
        for(uint j = 0; j < L; ++j) {
            grid(i, j).loop[bra] = m3210[i][j].to_ulong();
        }
    }
    //=================== construct bond-structure ===================
    uint const level = 1;
    std::for_each(grid.begin(), grid.end(), 
        [&](site_struct & s) {
            if(s.check != level)
            {
                follow_loop(&s, bra, grid);
            }
        }
    );
    grid.clear_check();
    
    //~ grid.print_all();
    //~ std::cout << "init loops" << std::endl;
    grid.init_loops();
    //~ std::cout << "done init loops" << std::endl;
    //~ grid.print_all();
}
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
        ket_swap(grid);
        ++m[get_n_loops(grid)];
        ket_swap(grid);
        grid.init_loops();
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
    change_loop(grid, loop, bra);
    
    
    //~ std::cout << "1";
    recursion(index2, grid, timer);
    change_loop(grid, loop, bra);
    
}

uint lsad(grid_class & grid) {//loop_shift_addition_distribution
    assert(m.size() == 0);
    std::for_each(grid.begin(), grid.end(),
        [&](site_struct & s){
            ++m[s.loop[qmc::bra] + (1000 * s.loop[qmc::bra2])];
        }
    );
    uint res = m.size();
    m.clear();
    return res;
}
int main(int argc, char* argv[])
{
    if(argc < 3)
        return 0;
    //=================== parse ===================
    std::string file1(argv[1]);
    std::string file2(argv[2]);
    std::cout << "parse..." << std::endl;
    read_in(argv[1], s1_raw);
    read_in(argv[2], s2_raw);
    
    H = (s1_raw.size()) / 2;
    L = (s1_raw[0].size()) / 4;
    
    //~ std::cout << H << std::endl;
    //~ std::cout << L << std::endl;
    
    for(uint i = 0; i < H; ++i) {
        m3210.push_back(std::vector<std::bitset<size2>>(L, 0));
    }
    
    //=================== grid setup ===================
    std::cout << "grid..." << std::endl;
    grid_class grid(H, L);
    init_parse(s1_raw, s1);
    init_grid(grid, qmc::bra);
    init_parse(s2_raw, s2);
    init_grid(grid, qmc::bra2);
    
    grid.init_loops();
    uint l1 = grid.n_loops_[qmc::bra];
    uint l2 = grid.n_loops_[qmc::bra2];
    
    
    //=================== calculating connective number ===================
    double med = 0;
    int count = 0;
    double conectivity;
    std::for_each(m.begin(), m.end(),
        [&](std::pair<const uint, uint> & s) {
            //~ if(s != *m.begin())
                //~ of << ", ";
            //~ of << s.first << "[" << s.second << "]";
            med += s.first * s.second;
            count += s.second;
        }
    );
    //~ of << std::endl;
    conectivity = med/count;
    m.clear();
    
    //=================== recursion ===================
    std::cout << "recursion..." << std::endl;
    addon::timer_class<addon::normal> timer(std::pow(2, l1 + l2));
    
    med = 0;
    count = 0;
    auto idx = build_index(grid);
    recursion(idx, grid, timer);
    
    //=================== calculating config_mean ===================
    double config_mean;
    std::for_each(m.begin(), m.end(),
        [&](std::pair<const uint, uint> & s){
            //~ if(s != *m.begin())
                //~ of << ", ";
            //~ of << s.first << "[" << s.second << "]";
            med += s.first * s.second;
            count += s.second;
        }
    );
    //~ of << std::endl;
    config_mean = med/count;
    m.clear();
    
    //=================== print ===================
    std::string name = "out.txt";
    if(argc >= 4)
        name = std::string(argv[3]);
    std::ofstream of(name, std::ios_base::app);
    int p1b = file1.rfind("/")+1;
    int p2b = file2.rfind("/")+1;
    int p1e = file1.rfind(".");
    int p2e = file2.rfind(".");
    
    of << l1 << "/" << l2;
    of << " cmean: " << std::setw(8) << config_mean;
    of << " lsad: " << std::setw(2) << lsad(grid);
    of << " con: " << std::setw(8) << conectivity;
    of << "  poss: " << std::setw(8) << std::pow(2, l1 + l2);
    of << " max: "  << std::setw(3) << 2 * (H * L / 2);
    of << " " << file1.substr(p1b, p1e-p1b) << " " << file2.substr(p2b, p2e-p2b) << std::endl;
    //=================== done ===================
    std::cout << "job done" << std::endl;
    return 0;
}
