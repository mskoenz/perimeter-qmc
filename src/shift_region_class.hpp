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
        shift_region_class(std::string filename): grow_level_(2) {
            std::ifstream in(filename);
            std::string temp;
            if(in.is_open()) {
                size_t consistent(0);
                
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
        shift_region_class(size_t const & H, size_t const & L, double const & spacing): H_(H), L_(L), N_(2), grow_level_(2) {
            stage2_ = std::vector<std::vector<std::vector<size_t>>>(N_, std::vector<std::vector<size_t>>(H_, std::vector<size_t>(L_, 0)));
            set_grow(std::vector<bond_type>(1, qmc::right));
            
            size_t grow_count = 0;
            for(size_t j = 0; j < 2; ++j) {
                for(size_t i = 0; i < H_; ++i) {
                    stage2_[1][i][j] = 1;
                    if(std::round((2.0 - spacing) * H_) > grow_count++)
                        stage2_[0][i][j] = 1;
                }
            }
            convert_2_to_1();
        }
        
        void print(size_t flags = 1) const {
            if((flags&1) == 1) {
                std::cout << "--------stage1-graphical--------" << std::endl;
                for(size_t n = 0; n < stage1_.size(); ++n) {
                    std::cout << "state: " << n << std::endl;
                    for(size_t i = 0; i < stage1_[n].size(); ++i) {
                        std::cout << "    " << stage1_[n][i] << std::endl;
                    }
                }
            }
            if((flags&2) == 2) {
                std::cout << "--------stage2-matrix--------" << std::endl;
                for(size_t n = 0; n < N_; ++n) {
                    std::cout << "state: " << n << std::endl;
                    for(size_t i = 0; i < H_; ++i) {
                        std::cout << "    ";
                        for(size_t j = 0; j < L_; ++j) {
                            std::cout << (*this)(n, i, j) << " ";
                        }
                        std::cout << std::endl;
                    }
                }
            }
        }
        size_t operator()(size_t const & n, size_t const & i, size_t const & j) const {
            return stage2_[n][i][j];
        }
        bool operator()(size_t const & i, size_t const & j) const { //backwards compatible
            assert(N_ == 1);
            return bool(stage2_[0][i][j]);
        }
        void set_grow(std::vector<bond_type> const & grow_dir) {
            grow_dir_ = grow_dir;
        }
        void invert() {
            if(N_ != 1)
                return;
            for(size_t i = 0; i < H_; ++i)
                for(size_t j = 0; j < L_; ++j)
                    stage2_[0][i][j] = !stage2_[0][i][j];
        }
        
        size_t & get_neighbor(size_t const & level, size_t const & i, size_t const & j, size_t const & dir) {
            switch(dir) {
                case(qmc::down):
                    return stage2_[level][(i + 1) % H_][j];
                    break;
                case(qmc::right):
                    return stage2_[level][i][(j + 1) % L_];
                    break;
                case(qmc::diag_down):
                    return stage2_[level][(i + 1) % H_][(j + 1) % L_];
                    break;
                case(qmc::diag_up):
                    return stage2_[level][(i + H_ - 1) % H_][(j + L_ - 1) % L_];
                    break;
                case(qmc::left):
                    return stage2_[level][i][(j + L_ - 1) % L_];
                    break;
                case(qmc::up):
                    return stage2_[level][(i + H_ - 1) % H_][j];
                    break;
                case(qmc::hori):
                    if((i+j) % 2 == 1)
                        return stage2_[level][i][(j + L_ - 1) % L_];
                    else
                        return stage2_[level][i][(j + 1) % L_];
                default:
                    throw std::runtime_error("false direction given in shift_region_class::get_neighbor");
            }
        }
        
        void grow_partial(double steps_in = 1) {
            steps_in -= 2;
            size_t steps = std::abs(steps_in) + 1;
            size_t grow_count = 0;
            size_t max_grow = std::round(std::abs(steps_in) * H_);
            const size_t just_grown = 1000;
            grow_level_ += steps;
            for(size_t level = 0; level < N_; ++level) {
                grow_count = 0;
                for(size_t k = 0; k < steps; ++k) {
                    for(size_t j = 0; j < L_; ++j) {
                        for(size_t i = 0; i < H_; ++i) {
                            size_t i_eff = steps_in < 0 ? H_ - i - 1: i;
                            size_t j_eff = steps_in < 0 ? L_ - j - 1: j;
                            size_t grow_factor = stage2_[level][i_eff][j_eff];
                            if(grow_factor > 0 and grow_factor < just_grown) {
                                std::for_each(grow_dir_.begin(), grow_dir_.end(), 
                                    [&](size_t const & dir) {
                                        if(get_neighbor(level, i_eff, j_eff, dir) == 0){
                                            if(grow_count++ < max_grow) {
                                                if(steps_in < 0)
                                                    stage2_[level][i_eff][j_eff] = just_grown;
                                                else
                                                    get_neighbor(level, i_eff, j_eff, dir) = grow_factor + just_grown;
                                            }
                                        }
                                    }
                                );
                            }
                        }
                    }
                    for(size_t i = 0; i < H_; ++i) {
                        for(size_t j = 0; j < L_; ++j) {
                            if(stage2_[level][i][j] >= just_grown)
                                stage2_[level][i][j] -= just_grown;
                        }
                    }
                }
            }
        }
        
        void grow(int steps_in = 1) {
            steps_in -= 2;//TODO: remove later
            const size_t just_grown = 1000;
            size_t steps = std::abs(steps_in);
            grow_level_ += steps;
            for(size_t level = 0; level < N_; ++level) {
                for(size_t k = 0; k < steps; ++k) {
                    for(size_t i = 0; i < H_; ++i) {
                        for(size_t j = 0; j < L_; ++j) {
                            size_t grow_factor = stage2_[level][i][j];
                            if(grow_factor > 0 and grow_factor < just_grown) {
                                std::for_each(grow_dir_.begin(), grow_dir_.end(), 
                                    [&](size_t const & dir) {
                                        if(get_neighbor(level, i, j, dir) == 0){
                                            if(steps_in < 0)
                                                stage2_[level][i][j] = just_grown;
                                            else
                                                get_neighbor(level, i, j, dir) = grow_factor + just_grown;
                                        }
                                    }
                                );
                            }
                        }
                    }
                    for(size_t i = 0; i < H_; ++i) {
                        for(size_t j = 0; j < L_; ++j) {
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
            for(size_t n = 0; n < N_; ++n) {
                for(size_t i = 0; i < H_; ++i) {
                    os << stage1_[n][i] << std::endl;
                }
                if(n != N_ - 1)
                    os << std::endl;
            }
            os.close();
        }
    private:
        void convert_1_to_2() {
            stage2_ = std::vector<std::vector<std::vector<size_t>>>(N_, std::vector<std::vector<size_t>>(H_, std::vector<size_t>(L_, 0)));
            for(size_t n = 0; n < N_; ++n) {
                for(size_t i = 0; i < H_; ++i) {
                    for(size_t j = 0; j < L_; ++j) {
                        stage2_[n][i][j] = (stage1_[n][i][2*j] - '0');
                    }
                }
            }
        }
        void convert_2_to_1() {
            stage1_ =  std::vector<std::vector<std::string>>(N_, std::vector<std::string>(H_, ""));
            for(size_t n = 0; n < N_; ++n) {
                for(size_t i = 0; i < H_; ++i) {
                    for(size_t j = 0; j < L_; ++j) {
                        stage1_[n][i] += '0' + (*this)(n, i, j);
                        stage1_[n][i] += " ";
                    }
                }
            }
        }
        size_t count_sites(std::string const & in) const {
            size_t res(0);
            for(size_t i = 0; i < in.size(); ++i) {
                if(in[i] >= '0' and in[i] <= '9')
                    ++res;
            }
            return res;
        }
    private:
        size_t H_;
        size_t L_;
        size_t N_;
        std::vector<std::vector<std::string>> stage1_;
        std::vector<std::vector<std::vector<size_t>>> stage2_;
        std::vector<bond_type> grow_dir_;
        size_t grow_level_;
    };
}//end namespace perimeter

#endif //__SWAP_REGION_CLASS_HEADER
