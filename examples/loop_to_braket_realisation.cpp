// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    29.04.2013 16:59:00 EDT
// File:    loop_to_braket_realisation.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <grid_class.hpp>
#include <map>

using namespace perimeter;

std::vector<std::string> s1_raw;
std::vector<std::string> s2_raw;
std::map<uint, uint> m;

void read_in(char * name) {
    std::ifstream in(name);
    std::string temp;
    char c;
    if(in.is_open()) {
        in >> c;
        if(c == '1') {
            while(in.peek() != '2') {
                getline(in, temp);
                if(temp.size() > 3)
                    s1_raw.push_back(temp);
            };
        }
        else
            in >> c;
        
        in >> c;
        while(in.peek() != '3') {
            getline(in, temp);
            if(temp.size() > 3)
                s2_raw.push_back(temp);
        };
    }
    else {
        std::cout << "file-fail" << std::endl;
    }
}
void print() {
    std::cout << "s1" << std::endl;
    std::for_each(s1_raw.begin(), s1_raw.end(),
        [](std::string & s){
            std::cout << s << std::endl;
        }
    );
    std::cout << "s2" << std::endl;
    std::for_each(s2_raw.begin(), s2_raw.end(),
        [](std::string & s){
            std::cout << s << std::endl;
        }
    );
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
void set_right(int i, int j, grid_class & grid, uint state) {
    if(grid(i, j).bond[state] != qmc::none)
        state = qmc::invert_state - state;
    grid(i, j).bond[state] = qmc::right;
    grid(i, j+1).bond[state] = qmc::left;
}
void set_down(int i, int j, grid_class & grid, uint state) {
    if(grid(i, j).bond[state] != qmc::none)
        state = qmc::invert_state - state;
    grid(i, j).bond[state] = qmc::down;
    grid(i+1, j).bond[state] = qmc::up;
}
void init_grid(grid_class & grid, uint state, std::vector<std::string> raw) {
    std::for_each(grid.begin(), grid.end(),
        [&](site_struct & s){
            s.bond[state] = qmc::none;
        }
    );
    
    for(uint i = 0; i < raw.size(); ++i) {
        if(raw[i][0] == '+') {
            for(uint j = 2; j < raw[i].size(); j+= 4) {
                if(raw[i][j] == '-')
                    set_right(i/2, (j-2)/4, grid, state);
            }
        }
        else {
            for(uint j = 0; j < raw[i].size(); j+= 4) {
                if(raw[i][j] == '|')
                    set_down(i/2, (j)/4, grid, state);
            }
        }
    }
    grid.init_loops();
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

void recursion(std::vector<std::pair<uint, uint>> index, grid_class & grid) {
    if(index.size() == 0) {
        ket_swap(grid);
        ++m[get_n_loops(grid)];
        ket_swap(grid);
        grid.init_loops();
        return;
    }
    uint loop = index.back().first;
    uint bra = index.back().second;
    
    auto index2 = index;
    index2.pop_back();
    
    //~ std::cout << "0";
    recursion(index2, grid);
    change_loop(grid, loop, bra);
    
    
    //~ std::cout << "1";
    recursion(index2, grid);
    change_loop(grid, loop, bra);
    
}

int main(int argc, char* argv[])
{
    if(argc < 2)
        return 0;
    read_in(argv[1]);
    
    int H = (s1_raw.size() - 1) / 2 + 1;
    int L = (s1_raw[0].size() - 1) / 4 + 1;
    
    std::cout << H << std::endl;
    std::cout << L << std::endl;
    grid_class grid(H, L);
    init_grid(grid, qmc::bra, s1_raw);
    init_grid(grid, qmc::bra2, s2_raw);
    
    auto idx = build_index(grid);
    
    recursion(idx, grid);
    
    std::for_each(m.begin(), m.end(),
        [&](std::pair<const uint, uint> & s){
            std::cout << "n_loop: " << s.first << " occures # " << s.second << " times" << std::endl;
        }
    );
    std::cout << "maximum is: " << 2 * (H * L / 2) << std::endl;
    return 0;
}
