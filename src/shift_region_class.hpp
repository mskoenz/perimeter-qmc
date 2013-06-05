// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    06.05.2013 14:45:54 EDT
// File:    shift_region_class.hpp

#ifndef __SWAP_REGION_CLASS_HEADER
#define __SWAP_REGION_CLASS_HEADER

#include <iostream>
#include <fstream>
#include <string>

#include <site_struct.hpp>

namespace perimeter {
    class shift_region_class {
    public:
        shift_region_class(std::string filename): grow_level_(0) {
            std::ifstream in(filename);
            std::string temp;
            if(in.is_open()) {
                uint consistent(0);
                
                stage1_.clear();
                stage1_.push_back(std::vector<std::string>());
                
                while(in) {
                    getline(in, temp);
                    if(temp.size() > 1) {
                        if(consistent == 0)
                            consistent = count_sites(temp);
                        if(count_sites(temp) == consistent) {
                            stage1_.back().push_back(temp);
                        }
                        else
                            throw std::runtime_error("inconsistent amount of sites in shift_region_class constructor");
                    }
                    else {
                        if(stage1_.back().size() > 0)
                            stage1_.push_back(std::vector<std::string>());
                    }
                };
                stage1_.pop_back();
                H_ = stage1_[0].size();
                L_ = consistent;
                N_ = stage1_.size();
            }
            else {
                std::cout << "file-fail in shift_region_class constructor" << std::endl;
            }
            convert_1_to_2();
        }
        void print(uint flags = 1) const {
            if((flags&1) == 1) {
                std::cout << "--------stage1-graphical--------" << std::endl;
                for(uint n = 0; n < stage1_.size(); ++n) {
                    std::cout << "state: " << n << std::endl;
                    for(uint i = 0; i < stage1_[n].size(); ++i) {
                        std::cout << "    " << stage1_[n][i] << std::endl;
                    }
                }
            }
            if((flags&2) == 2) {
                std::cout << "--------stage2-matrix--------" << std::endl;
                for(uint n = 0; n < N_; ++n) {
                    std::cout << "state: " << n << std::endl;
                    for(uint i = 0; i < H_; ++i) {
                        std::cout << "    ";
                        for(uint j = 0; j < L_; ++j) {
                            std::cout << (*this)(n, i, j) << " ";
                        }
                        std::cout << std::endl;
                    }
                }
            }
        }
        uint operator()(uint const & n, uint const & i, uint const & j) const {
            return stage2_[n][i][j];
        }
        bool operator()(uint const & i, uint const & j) const { //backwards compatible
            assert(N_ == 1);
            return bool(stage2_[0][i][j]);
        }
        void set_grow(std::vector<bond_type> const & grow_dir) {
            grow_dir_ = grow_dir;
        }
        void invert() {
            if(N_ != 1)
                return;
            for(uint i = 0; i < H_; ++i)
                for(uint j = 0; j < L_; ++j)
                    stage2_[0][i][j] = !stage2_[0][i][j];
        }
        void grow(int steps_in = 1) {
            steps_in -= 2;//TODO: remove later
            const uint just_grown = 1000;
            uint steps = std::abs(steps_in);
            grow_level_ += steps;
            for(uint level = 0; level < N_; ++level) {
                for(uint k = 0; k < steps; ++k) {
                    for(uint i = 0; i < H_; ++i) {
                        for(uint j = 0; j < L_; ++j) {
                            uint grow_factor = stage2_[level][i][j];
                            if(grow_factor > 0 and grow_factor < just_grown) {
                                std::for_each(grow_dir_.begin(), grow_dir_.end(), 
                                    [&](uint const & dir) {
                                        switch(dir) {
                                            case(qmc::down):
                                                if(stage2_[level][(i + 1) % H_][j] == 0){
                                                    if(steps_in < 0)
                                                        stage2_[level][i][j] = just_grown;
                                                    else
                                                        stage2_[level][(i + 1) % H_][j] = grow_factor + just_grown;
                                                }
                                                break;
                                            case(qmc::right):
                                                if(stage2_[level][i][(j + 1) % L_] == 0 and qmc::n_bonds != qmc::hex){
                                                    if(steps_in < 0)
                                                        stage2_[level][i][j] = just_grown;
                                                    else
                                                        stage2_[level][i][(j + 1) % L_] = grow_factor + just_grown;
                                                }
                                                break;
                                            case(qmc::diag_down):
                                                if(stage2_[level][(i + 1) % H_][(j + 1) % L_] == 0 and qmc::n_bonds == qmc::tri){
                                                    if(steps_in < 0)
                                                        stage2_[level][i][j] = just_grown;
                                                    else
                                                        stage2_[level][(i + 1) % H_][(j + 1) % L_] = grow_factor + just_grown;
                                                }
                                                break;
                                            case(qmc::diag_up):
                                                if(stage2_[level][(i + H_ - 1) % H_][(j + L_ - 1) % L_] == 0 and qmc::n_bonds == qmc::tri){
                                                    if(steps_in < 0)
                                                        stage2_[level][i][j] = just_grown;
                                                    else
                                                        stage2_[level][(i + H_ - 1) % H_][(j + L_ - 1) % L_] = grow_factor + just_grown;
                                                }
                                                break;
                                            case(qmc::left):
                                                if(stage2_[level][i][(j + L_ - 1) % L_] == 0 and qmc::n_bonds != qmc::hex){
                                                    if(steps_in < 0)
                                                        stage2_[level][i][j] = just_grown;
                                                    else
                                                        stage2_[level][i][(j + L_ - 1) % L_] = grow_factor + just_grown;
                                                }
                                                break;
                                            case(qmc::up):
                                                if(stage2_[level][(i + H_ - 1) % H_][j] == 0){
                                                    if(steps_in < 0)
                                                        stage2_[level][i][j] = just_grown;
                                                    else
                                                        stage2_[level][(i + H_ - 1) % H_][j] = grow_factor + just_grown;
                                                }
                                                break;
                                            case(qmc::hori):
                                                if((i+j) % 2 == 1) {
                                                    if(stage2_[level][i][(j + L_ - 1) % L_] == 0  and qmc::n_bonds == qmc::hex){
                                                        if(steps_in < 0)
                                                            stage2_[level][i][j] = just_grown;
                                                        else
                                                            stage2_[level][i][(j + L_ - 1) % L_] = grow_factor + just_grown;
                                                    }
                                                }
                                                else {
                                                    if(stage2_[level][i][(j + 1) % L_] == 0  and qmc::n_bonds == qmc::hex){
                                                        if(steps_in < 0)
                                                            stage2_[level][i][j] = just_grown;
                                                        else
                                                            stage2_[level][i][(j + 1) % L_] = grow_factor + just_grown;
                                                    }
                                                }
                                                break;
                                        }
                                    }
                                );
                            }
                        }
                    }
                    for(uint i = 0; i < H_; ++i) {
                        for(uint j = 0; j < L_; ++j) {
                            if(stage2_[level][i][j] >= just_grown)
                                stage2_[level][i][j] -= just_grown;
                        }
                    }
                }
            }
        }
        void write(std::string filename) {
            convert_2_to_1();
            
            std::ofstream os(filename);
            for(uint n = 0; n < N_; ++n) {
                for(uint i = 0; i < H_; ++i) {
                    os << stage1_[n][i] << std::endl;
                }
                if(n != N_ - 1)
                    os << std::endl;
            }
            os.close();
        }
    private:
        void convert_1_to_2() {
            stage2_ = std::vector<std::vector<std::vector<uint>>>(N_, std::vector<std::vector<uint>>(H_, std::vector<uint>(L_, 0)));
            for(uint n = 0; n < N_; ++n) {
                for(uint i = 0; i < H_; ++i) {
                    for(uint j = 0; j < L_; ++j) {
                        stage2_[n][i][j] = (stage1_[n][i][2*j] - '0');
                    }
                }
            }
        }
        void convert_2_to_1() {
            stage1_ =  std::vector<std::vector<std::string>>(N_, std::vector<std::string>(H_, ""));
            for(uint n = 0; n < N_; ++n) {
                for(uint i = 0; i < H_; ++i) {
                    for(uint j = 0; j < L_; ++j) {
                        stage1_[n][i] += '0' + (*this)(n, i, j);
                        stage1_[n][i] += " ";
                    }
                }
            }
        }
        uint count_sites(std::string const & in) const {
            uint res(0);
            for(uint i = 0; i < in.size(); ++i) {
                if(in[i] >= '0' and in[i] <= '9')
                    ++res;
            }
            return res;
        }
    private:
        uint H_;
        uint L_;
        uint N_;
        std::vector<std::vector<std::string>> stage1_;
        std::vector<std::vector<std::vector<uint>>> stage2_;
        std::vector<bond_type> grow_dir_;
        uint grow_level_;
    };
}//end namespace perimeter

#endif //__SWAP_REGION_CLASS_HEADER
