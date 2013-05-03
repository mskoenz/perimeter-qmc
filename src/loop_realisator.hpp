// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    02.05.2013 14:36:22 EDT
// File:    loop_realsiator.hpp

#ifndef __LOOP_REALSIATOR_HEADER
#define __LOOP_REALSIATOR_HEADER

#include <site_struct.hpp>

#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <assert.h>
#include <map>
#include <algorithm>

//perimeter is documented in grid_class.hpp
namespace perimeter {

    struct loop_struct {
        loop_struct(): loop(0), check(false) {}
        uint loop;
        bool check;
    };
    bool operator==(loop_struct const & l1, loop_struct const & l2) {
        if(l1.loop != l2.loop or l1.check != l2.check)
            return false;
        return true;
    }
    class loop_real_class {
        
        typedef std::vector<std::string> stage1_type;
        static const uint stage2_size = 128;
        typedef std::bitset<stage2_size> stage2_type;
        static const uint stage3_size = 6;
        typedef std::bitset<stage3_size> bitset_bond;
        typedef std::vector<std::vector<bitset_bond>> stage3_type;
        typedef std::vector<std::vector<loop_struct>> stage4_type; //loop_id
    public:
        loop_real_class(std::string const & filename) {
            std::ifstream in(filename);
            std::string temp;
            if(in.is_open()) {
            
                while(in) {
                    getline(in, temp);
                    if(temp.size() > 3)
                        stage1_.push_back(temp);
                };
            }
            else {
                std::cout << "file-fail" << std::endl;
            }
            H_ = (stage1_.size()) / 2;
            L_ = (stage1_[0].size()) / 4;
            
            assert(H_%2 == 0);
            assert(L_%2 == 0);
            assert(H_ > 0);
            assert(L_ > 0);
            assert(stage2_size > qmc::n_bonds);
            
            in.close();
            
            convert_1_to_2();
            convert_1_to_3();
            create_loop_layer();
        }
        loop_real_class(uint H, uint L, stage2_type const & in): H_(H), L_(L), stage1_(), stage2_(in) {
            assert(H_%2 == 0);
            assert(L_%2 == 0);
            assert(H_ > 0);
            assert(L_ > 0);
            assert(stage2_size > qmc::n_bonds);
            
            convert_2_to_1();
            convert_2_to_3();
            create_loop_layer();
        }
        loop_real_class(loop_real_class const & arg): H_(arg.H_), L_(arg.L_), stage1_(arg.stage1_), stage2_(arg.stage2_), stage3_(arg.stage3_), stage4_(arg.stage4_) {
            assert(arg == (*this));
        }
        operator uint64_t() {
            return stage2_.to_ulong();
        }
        //------------------- converter -------------------
        void convert_1_to_2() {
            stage2_ = 0;
            uint shift = 0;
            for(uint i = 0; i < stage1_.size(); ++i) {
                if(stage1_[i][0] == '+') {
                    for(uint j = 2; j < stage1_[i].size(); j+= 4) {
                        if(stage1_[i][j] == '-')
                            stage2_[shift] = 1;
                        ++shift;
                    }
                }
                else {
                    for(uint j = 0; j < stage1_[i].size(); j+= 4) {
                        if(stage1_[i][j] == '|') 
                            stage2_[shift] = 1;
                        ++shift;
                    }
                }
                assert(shift < stage2_size);
            }
        }
        void convert_1_to_3() {
            stage3_.clear();
            
            
            for(uint i = 0; i < H_; ++i) {
                stage3_.push_back(std::vector<bitset_bond>(L_, 0));
            }
            for(uint i = 0; i < stage1_.size(); ++i) {
                if(stage1_[i][0] == '+') {
                    for(uint j = 2; j < stage1_[i].size(); j+= 4) {
                        if(stage1_[i][j] == '-') {
                            stage3_[i/2][(j-2)/4][qmc::right] = true;
                            stage3_[i/2][((j-2)/4+1)%L_][qmc::left] = true;
                        }
                    }
                }
                else {
                    for(uint j = 0; j < stage1_[i].size(); j+= 4) {
                        if(stage1_[i][j] == '|') {
                            stage3_[i/2][j/4][qmc::down] = true;
                            stage3_[(i/2+1)%H_][j/4][qmc::up] = true;
                        }
                    }
                }
            }
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    if(stage3_[i][j].count() == 0)
                        stage3_[i][j][qmc::me] = true;
                }
            }
        }
        void convert_2_to_1() {
            stage1_.clear();
            
            for(uint i = 0; i < H_; ++i) {
                std::string l_bone = "";
                std::string e_bone = "";
                for(uint j = 0; j < L_; ++j) {
                    l_bone += "+   ";
                    e_bone += "    ";
                }
                stage1_.push_back(l_bone);
                stage1_.push_back(e_bone);
            }
            
            for(uint i = 0; i < 2*H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    if(stage2_[i * L_ + j] == true) {
                        if(i % 2 == 0) { //horizontal
                            stage1_[i][4*j+1] = '-';
                            stage1_[i][4*j+2] = '-';
                            stage1_[i][4*j+3] = '-';
                        }
                        else { //vertical
                            stage1_[i][4*j] = '|';
                        }
                    }
                }
            }    
        }
        void convert_2_to_3() {
            convert_2_to_1();
            convert_1_to_3();
        }
        void convert_3_to_1() {
            convert_3_to_2();
            convert_2_to_1();
        }
        void convert_3_to_2() {
            stage2_ = 0;
            
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    if(stage3_[i][j][qmc::down] == true)
                        stage2_[(2*i+1)*L_ + j] = true;
                    if(stage3_[i][j][qmc::right] == true)
                        stage2_[(2*i)*L_ + j] = true;
                    if(stage3_[i][j][qmc::left] == true)
                        stage2_[(2*i)*L_ + (j + L_- 1 )%L_] = true;
                    if(stage3_[i][j][qmc::up] == true)
                        stage2_[((2*i-1 + H_)%H_)*L_ + j] = true;
                        
                        
                }
            }
        }
        
        //------------------- loop creation -------------------
        void recursion_loop(uint const & i, uint const & j, uint const & loop_nr) {
            if(stage4_[i][j].check == true)
                return;
            stage4_[i][j].check = true;
            stage4_[i][j].loop = loop_nr;
            
            if(stage3_[i][j][qmc::down] == true)
                recursion_loop((i + 1) % H_, j, loop_nr);
            if(stage3_[i][j][qmc::right] == true)
                recursion_loop(i, (j + 1) % L_, loop_nr);
            if(stage3_[i][j][qmc::left] == true)
                recursion_loop(i, (j + L_ - 1) % L_, loop_nr);
            if(stage3_[i][j][qmc::up] == true)
                recursion_loop((i + H_ - 1) % H_, j, loop_nr);
        }
        void create_loop_layer() {
            stage4_.clear();
            for(uint i = 0; i < H_; ++i) {
                stage4_.push_back(std::vector<loop_struct>(L_, loop_struct()));
            }
            
            loop_nr_ = 0;
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    if(stage4_[i][j].check != true) {
                        recursion_loop(i, j, loop_nr_);
                        ++loop_nr_;
                    }
                }
            }
            
            //~ for(uint i = 0; i < H_; ++i) {
                //~ for(uint j = 0; j < L_; ++j) {
                    //~ stage4_[i][j].check = false;
                //~ }
            //~ }
        }
        //------------------- check conformality -------------------
        bool conform() const { //checks if the loop_structure is valid for braket-conversion
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    if(stage3_[i][j].count() > 2 or stage3_[i][j][qmc::me] == true)
                        return false;
                    if(stage3_[i][j].count() == 1) {
                        if(stage3_[i][j][qmc::down] == true) {
                            if(stage3_[(i + 1) % H_][j].count() != 1 or stage3_[(i + 1) % H_][j][qmc::up] != true)
                                return false;
                        }
                        else if(stage3_[i][j][qmc::up] == true) {
                            if(stage3_[(i + H_ - 1) % H_][j].count() != 1 or stage3_[(i + H_ - 1) % H_][j][qmc::down] != true)
                                return false;
                        }
                        else if(stage3_[i][j][qmc::right] == true) {
                            if(stage3_[i][(j + 1) % L_].count() != 1 or stage3_[i][(j + 1) % L_][qmc::left] != true)
                                return false;
                        }
                        else if(stage3_[i][j][qmc::left] == true) {
                            if(stage3_[i][(j + L_ - 1) % L_].count() != 1 or stage3_[i][(j + L_ - 1) % L_][qmc::right] != true)
                                return false;
                        }
                    }
                }
            }
            return true;
        }
        //~ bool constant_winding() {
            //~ std::vector<int> windH(H_, 0);
            //~ std::vector<int> windL(L_, 0);
            //~ //------------------- winding H -------------------
            //~ for(uint i = 0; i < 2*H_; ++i) {
                //~ for(uint j = 0; j < L_; ++j) {
                    //~ if(i%4 == 0)
                        //~ windL[j] += stage2_[i * L_ + j];
                    //~ else if(i%4 == 2)
                        //~ windL[j] -= stage2_[i * L_ + j];
                    //~ else 
                        //~ windH[i/2] += (j%2==0 ? -1:1) *stage2_[i * L_ + j];
                //~ }
            //~ }
    //~ 
            //~ int start = std::abs(windH[0]);
            //~ for(uint i = 0; i < H_; ++i) {
                //~ windH[i] = std::abs(windH[i]);
                //~ if(start != windH[i])
                    //~ return false;
            //~ }
            //~ 
            //~ start = std::abs(windL[0]);
            //~ for(uint j = 0; j < L_; ++j) {
                //~ windL[j] = std::abs(windL[j]);
                //~ if(start != windL[j])
                    //~ return false;
            //~ }
            //~ 
            //~ windH_ = windH[0];
            //~ windL_ = windL[0];
            //~ 
            //~ return true;
        //~ }
        //------------------- getter -------------------
        uint const & H() const {
            return H_;
        }
        uint const & L() const {
            return L_;
        }
        bitset_bond const & operator()(uint const & i, uint const & j) const {
            return stage3_[i][j];
        }
        std::map<uint, uint> loop_analysis() const {
            std::map<uint, uint> l;
            
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    ++(l[stage4_[i][j].loop]);
                }
            }
            std::map<uint, uint> res;
            std::for_each(l.begin(), l.end(),
                [&](std::pair<const uint, uint> & p) {
                    ++(res[p.second]);
                }
            );
            return res;
        }
        uint const & n_loops() const {
            return loop_nr_;
        }
        uint n_loops(uint size) const {
            return loop_analysis()[size];
        }
        //------------------- print flags-------------------
        void print(uint stage = 1) const {
            if((stage&1) == 1) {
                std::cout << "--------stage1-graphical--------" << std::endl;
                for(uint i = 0; i < stage1_.size(); ++i) {
                    std::cout << "    " << stage1_[i] << std::endl;
                }
            }
            if((stage&2) == 2) {
                std::cout << "--------stage2-code--------" << std::endl;
                std::cout << stage2_ << "  " << stage2_.to_ulong() << std::endl;
            }
            if((stage&4) == 4) {
                std::cout << "--------stage3-neighbor-------" << std::endl;
                for(uint i = 0; i < H_; ++i) {
                    for(uint j = 0; j < L_; ++j) {
                        std::cout << " " << stage3_[i][j];
                    }
                    std::cout << std::endl;
                }
            }
            if((stage&8) == 8) {
                std::cout << "--------stage4-loop-------" << std::endl;
                for(uint i = 0; i < H_; ++i) {
                    std::cout << "    ";
                    for(uint j = 0; j < L_; ++j) {
                        std::cout << " " << stage4_[i][j].loop;
                    }
                    std::cout << std::endl;
                }
            }
            if((stage&16) == 16) {
                std::cout << "--------info-loop-------" << std::endl;
                std::cout << "    loop_nr: " << loop_nr_ << std::endl;
            }
        }
        
        //------------------- friends -------------------
        friend bool operator==(loop_real_class const & l1, loop_real_class const & l2);
        friend loop_real_class operator&(loop_real_class const & l1, loop_real_class const & l2);
        friend loop_real_class operator|(loop_real_class const & l1, loop_real_class const & l2);
        friend loop_real_class operator^(loop_real_class const & l1, loop_real_class const & l2);
    private:
        uint H_;
        uint L_;
        stage1_type stage1_;
        stage2_type stage2_;
        stage3_type stage3_;
        stage4_type stage4_;
        uint loop_nr_;
    };

    bool operator==(loop_real_class const & l1, loop_real_class const & l2) {
        if(l1.H_ != l2.H_)
            return false;
        if(l1.H_ != l2.H_)
            return false;
        
        
        if(!std::equal(l1.stage1_.begin(), l1.stage1_.end(), l2.stage1_.begin()))
            return false;
        
        if(l1.stage2_ != l2.stage2_)
            return false;
        
        for(uint i = 0; i < l1.stage3_.size(); ++i)
            if(!std::equal(l1.stage3_[i].begin(), l1.stage3_[i].end(), l2.stage3_[i].begin()))
                return false;
        for(uint i = 0; i < l1.stage4_.size(); ++i)
            if(!std::equal(l1.stage4_[i].begin(), l1.stage4_[i].end(), l2.stage4_[i].begin()))
                return false;
        
        return true;
    }
    loop_real_class operator&(loop_real_class const & l1, loop_real_class const & l2) {
        assert(l1.H_ == l2.H_);
        assert(l1.L_ == l2.L_);
        return loop_real_class(l1.H_, l1.L_, (l1.stage2_ & l2.stage2_));
    }
    loop_real_class operator|(loop_real_class const & l1, loop_real_class const & l2) {
        assert(l1.H_ == l2.H_);
        assert(l1.L_ == l2.L_);
        return loop_real_class(l1.H_, l1.L_, (l1.stage2_ | l2.stage2_));
    }
    loop_real_class operator^(loop_real_class const & l1, loop_real_class const & l2) {
        assert(l1.H_ == l2.H_);
        assert(l1.L_ == l2.L_);
        return loop_real_class(l1.H_, l1.L_, (l1.stage2_ ^ l2.stage2_));
    }

}//end namespace perimeter

#endif //__LOOP_REALSIATOR_HEADER
