// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.04.2013 10:45:13 EDT
// File:    grid_class.hpp

#ifndef __GRID_CLASS_HEADER
#define __GRID_CLASS_HEADER

#include <site_struct.hpp>

#include <boost/integer.hpp>
#include <boost/multi_array.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <algorithm>
#include <typeinfo>

///  \brief where all the physics happens
///  
///  
namespace perimeter {
    ///  \brief grid class with site_struct as sites
    ///
    ///
    class grid_class { //grid is shorter than lattice ; 
        ///  the type of the sites
        typedef site_struct grid_type;
        
        ///  the array where the sites are stored in
        template<typename U> 
        using array_type = boost::multi_array<U,2>;
        
        template<typename U> 
        using vector_type = std::vector<U>;
        
    public:
        typedef typename vector_type<grid_type>::size_type index_type;
        ///standart constructor
        ///@param H height of the grid
        ///@param L length of the grid
        inline grid_class(uint const H, uint const L, uint const init = 0): 
                H_(H)
              , L_(L)
              , grid_(boost::extents[H_][L_]) {
            assert(H_%2==0);
            assert(L_%2==0);
            assert(H_>0);
            assert(L_>0);
            init_grid(init);
            init_loops();
        }
        
        void init_grid(uint const init) {
            assert(H_%2==0);
            assert(L_%2==0);
            assert(H_>0);
            assert(L_>0);
            int state = 0;
            std::for_each(begin(), end(), 
                [&](site_struct & s) {
                    s.spin = (state + state / L_)%2;
                    if(init == 0) {
                        
                        if(qmc::n_bonds == 3) {
                            s.bond[qmc::bra] = qmc::hori;
                            s.bond[qmc::ket] = qmc::hori;
                        }
                        else {
                            s.bond[qmc::bra] = (state%2==0 ? qmc::right:qmc::left);
                            s.bond[qmc::ket] = (state%2==0 ? qmc::right:qmc::left);
                        }
                    }
                    else if(init == 1) {
                        s.bond[qmc::bra] = (state/L_%2==0 ? qmc::down:qmc::up);
                        s.bond[qmc::ket] = (state/L_%2==0 ? qmc::down:qmc::up);
                    }
                    else if(init == 2) {
                        if(qmc::n_bonds == 3) {
                            s.bond[qmc::bra] = (state/L_%3==0 ? qmc::down: (state/L_%3==2 ? qmc::hori : qmc::up));
                            s.bond[qmc::ket] = (state/L_%3==0 ? qmc::down: (state/L_%3==2 ? qmc::hori : qmc::up));
                        }
                        else if(qmc::n_bonds == 6) {
                            s.bond[qmc::bra] = (state/L_%2==0 ? qmc::diag_down:qmc::diag_up);
                            s.bond[qmc::ket] = (state/L_%2==0 ? qmc::diag_down:qmc::diag_up);
                        }
                    }

                    s.loop = state;
                    ++state;
                }
            );
            //initialising the neighbor structure
            for(uint i = 0; i < H_; ++i) {
                for(uint j = 0; j < L_; ++j) {
                    grid_[i][j].neighbor[qmc::up] = &grid_[(i+H_-1)%H_][j];
                    grid_[i][j].neighbor[qmc::down] = &grid_[(i+1)%H_][j];
                    
                    if(qmc::n_bonds == 3) {
                        grid_[i][j].neighbor[qmc::hori] = &grid_[i][(j+L_ + 1 - 2*((i+j)%2) )%L_];
                    }
                    else {
                        grid_[i][j].neighbor[qmc::left] = &grid_[i][(j+L_-1)%L_];
                        grid_[i][j].neighbor[qmc::right] = &grid_[i][(j+1)%L_];
                    }
                    
                    if(qmc::n_bonds == 6) {
                        grid_[i][j].neighbor[qmc::diag_up] = &grid_[(i+H_-1)%H_][(j+L_-1)%L_];
                        grid_[i][j].neighbor[qmc::diag_down] = &grid_[(i+1)%H_][(j+1)%L_];
                    }
                }
            }
        }
        
        void init_loops() {
            uint8_t check_level = grid_[0][0].check + 1;
            uint loop_var = 0;
            
            std::for_each(begin(), end(), 
                [&](site_struct & s) {
                    if(s.check != check_level)
                    {
                        ++(s.check);
                        s.loop = loop_var;
                        site_struct * next = next_in_loop(&s);
                        while(next->check != check_level) {
                            ++(next->check);
                            next->loop = loop_var;
                            next = next_in_loop(next);
                        };
                        ++loop_var;
                    }
                }
            );
        }
        
        site_struct * next_in_loop(site_struct * in) {
            static bool alternator = true;
            alternator = !alternator;
            return in->partner(alternator);
        }
        ///  returns a reference to the site (i, j)
        ///  @param i index for the height
        ///  @param j index for the lenght
        grid_type & operator()(index_type const i, index_type const j) {
            return grid_[i][j];
        }
        grid_type const & operator()(index_type const i, index_type const j) const {
            return grid_[i][j];
        }
        
        ///  \brief print function
        ///  @param os is the stream that is printed into. Default is the std::cout ostream
        void print(std::ostream & os = std::cout) const {
            for(index_type i = 0; i < H_; ++i)
            {
                for(index_type j = 0; j < L_; ++j)
                {
                    os << std::setw(3) << grid_[i][j] << " ";
                }
                os << std::endl;
            }
        }
        
        void print_all(std::ostream & os = std::cout) const {
            const uint kmax = grid_type::print_site_height();
            
            array_type<std::string> s(boost::extents[kmax * H_][L_]);
            vector_type<std::string> in;
            for(index_type i = 0; i < H_; ++i)
            {
                for(index_type j = 0; j < L_; ++j)
                {
                    in = grid_[i][j].string_print(L_);
                    for(index_type k = 0; k < kmax; ++k)
                    {
                        s[i * kmax + k][j] = in[k];
                    }
                }
            }
            for(index_type i = 0; i < H_ * kmax; ++i)
            {
                if(qmc::n_bonds == 6)
                    for(index_type j = 0; j < H_*kmax - i; ++j)
                        os << " ";
                    
                for(index_type j = 0; j < L_; ++j)
                    os << s[i][j];
                os << std::endl;
            }
        }
        
        ///  \brief can be used to traverse the array
        grid_type * begin() {
            return grid_.data();
        }
        
        ///  \brief can be used to traverse the array
        grid_type * end() {
            return grid_.data() + grid_.num_elements();
        }
    private:
        uint const H_; ///<height
        uint const L_; ///<length
        array_type<grid_type> grid_;            ///< the actual grid
    };
}//end namespace perimeter
#endif //__GRID_CLASS_HEADER
