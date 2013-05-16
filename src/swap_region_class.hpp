// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    06.05.2013 14:45:54 EDT
// File:    swap_region_class.hpp

#ifndef __SWAP_REGION_CLASS_HEADER
#define __SWAP_REGION_CLASS_HEADER

#include <iostream>
#include <fstream>
#include <string>

#include <site_struct.hpp>

namespace perimeter {
    class swap_region_class {
    public:
        swap_region_class(std::string filename): grow_level_(0) {
            std::ifstream in(filename);
            std::string temp;
            if(in.is_open()) {
                uint consistent(0);
                
                stage1_.clear();
                while(in) {
                    getline(in, temp);
                    if(temp.size() > 1) {
                        if(consistent == 0)
                            consistent = count_sites(temp);
                        if(count_sites(temp) == consistent) {
                            stage1_.push_back(temp);
                        }
                        else
                            throw std::runtime_error("inconsistent amount of sites in swap_region_class constructor");
                    }
                };
                H_ = stage1_.size();
                L_ = consistent;
            }
            else {
                std::cout << "file-fail in swap_region_class constructor" << std::endl;
            }
            convert_1_to_2();
        }
        void print(uint flags = 1) const {
            if((flags&1) == 1) {
                std::cout << "--------stage1-graphical--------" << std::endl;
                for(uint i = 0; i < stage1_.size(); ++i) {
                    std::cout << "    " << stage1_[i] << std::endl;
                }
            }
            if((flags&2) == 2) {
                std::cout << "--------stage2-matrix--------" << std::endl;
                for(uint i = 0; i < H_; ++i) {
                    std::cout << "    ";
                    for(uint j = 0; j < L_; ++j) {
                        std::cout << stage2_[i][j] << " ";
                    }
                    std::cout << std::endl;
                }
            }
        }
        bool operator()(uint const & i, uint const & j) const {
            return bool(stage2_[i][j]);
        }
        void invert() {
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    stage2_[i][j] = !stage2_[i][j];
                }
            }
        }
        void set_grow(std::vector<bond_type> const & grow_dir) {
            grow_dir_ = grow_dir;
        }
        void grow(uint steps = 1) {
            grow_level_ += steps;
            for(uint k = 0; k < steps; ++k) {
                for(uint i = 0; i < H_; ++i) {
                    for(uint j = 0; j < L_; ++j) {
                        if(stage2_[i][j] == 1) {
                            std::for_each(grow_dir_.begin(), grow_dir_.end(), 
                                [&](uint const & dir) {
                                    switch(dir) {
                                        case(qmc::down):
                                            if(stage2_[(i + 1) % H_][j] == 0)
                                                stage2_[(i + 1) % H_][j] = 2;
                                            break;
                                        case(qmc::right):
                                            if(stage2_[i][(j + 1) % L_] == 0 and qmc::n_bonds != qmc::hex)
                                                stage2_[i][(j + 1) % L_] = 2;
                                            break;
                                        case(qmc::diag_down):
                                            if(stage2_[(i + 1) % H_][(j + 1) % L_] == 0 and qmc::n_bonds == qmc::tri)
                                            stage2_[(i + 1) % H_][(j + 1) % L_] = 2;
                                            break;
                                        case(qmc::diag_up):
                                            if(stage2_[(i + H_ - 1) % H_][(j + L_ - 1) % L_] == 0 and qmc::n_bonds == qmc::tri)
                                                stage2_[(i + H_ - 1) % H_][(j + L_ - 1) % L_] = 2;
                                            break;
                                        case(qmc::left):
                                            if(stage2_[i][(j + L_ - 1) % L_] == 0 and qmc::n_bonds != qmc::hex)
                                                stage2_[i][(j + L_ - 1) % L_] = 2;
                                            break;
                                        case(qmc::up):
                                            if(stage2_[(i + H_ - 1) % H_][j] == 0)
                                                stage2_[(i + H_ - 1) % H_][j] = 2;
                                            break;
                                        case(qmc::hori):
                                            if((i+j) % 2 == 1) {
                                                if(stage2_[i][(j + L_ - 1) % L_] == 0  and qmc::n_bonds == qmc::hex)
                                                    stage2_[i][(j + L_ - 1) % L_] = 2;
                                            }
                                            else {
                                                if(stage2_[i][(j + 1) % L_] == 0  and qmc::n_bonds == qmc::hex)
                                                    stage2_[i][(j + 1) % L_] = 2;
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
                        if(stage2_[i][j] == 2)
                            stage2_[i][j] = 1;
                    }
                }
            }
        }
        void write(std::string filename) {
            convert_2_to_1();
            std::ofstream os(filename);
            for(uint i = 0; i < H_; ++i) {
                os << stage1_[i] << std::endl;
            }
            os.close();
        }
    private:
        void convert_1_to_2() {
            stage2_ = std::vector<std::vector<uint>>(H_, std::vector<uint>(L_, 0));
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    stage2_[i][j] = (stage1_[i][2*j] == '1' ? 1 : 0);
                }
            }
        }
        void convert_2_to_1() {
            stage1_ = std::vector<std::string>(H_, "");
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    if(stage2_[i][j] > 0)
                        stage1_[i] += "1 ";
                    else
                        stage1_[i] += "0 ";
                }
            }
        }
        uint count_sites(std::string const & in) const {
            uint res(0);
            for(uint i = 0; i < in.size(); ++i) {
                if(in[i] == '1' or in[i] == '0')
                    ++res;
            }
            return res;
        }
        private:
        uint H_;
        uint L_;
        std::vector<std::string> stage1_;
        std::vector<std::vector<uint>> stage2_;
        std::vector<bond_type> grow_dir_;
        uint grow_level_;
    };
}//end namespace perimeter

#endif //__SWAP_REGION_CLASS_HEADER
