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

#define DEBUG_VAR(x) std::cout << "\033[1;31m" << "  DEBUG_VAR: " << "\033[0;31m" << #x << " = " << x << "\033[0m" << std::endl;

///  \brief where all the physics happens
///  
///  
namespace perimeter {
    ///  \brief universal grid class
    ///
    ///  @param T grid_type (sites)
    ///  
    template<typename T>
    class grid_class { //grid is shorter than lattice ; 
        ///  the type of the sites
        typedef T grid_type;
        
        ///  the array where the sites are stored in
        template<typename U> 
        using array_type = boost::multi_array<U,2>;
        
        ///  a shared pointer to the actual array, since some instances can share some arrays
        template<typename U> 
        using array_ptr_type = std::shared_ptr<array_type<U>>;
        
        ///  the container where the index-lookup is stored in
        ///  \sa lookup_L_
        template<typename U>
        using vector_type = std::vector<U>;
        
        ///  again a shared pointer since multiple instances can share the index-lookup
        template<typename U> 
        using vector_ptr_type = std::shared_ptr<vector_type<U>>;
        
    public:
        ///  the type of the index (usualy an unsigned int)
        typedef typename vector_type<grid_type>::size_type index_type;
        
        ///standart constructor
        ///@param H height of the grid
        ///@param L length of the grid
        inline grid_class(uint const H, uint const L): 
                H_(H)
              , L_(L)
              , lookup_H_fwd(new std::vector<index_type>(H_, index_type()))
              , lookup_L_fwd(new std::vector<index_type>(L_, index_type()))
              , lookup_H_rev(new std::vector<index_type>(H_, index_type()))
              , lookup_L_rev(new std::vector<index_type>(L_, index_type()))
              , grid_(new boost::multi_array<grid_type,2>(boost::extents[H_][L_])) {
            index_type i = 1;
            //here the periodic boundaries are set up
            std::for_each(lookup_H_fwd->begin(), lookup_H_fwd->end(), [&](index_type & ind) {ind = i%H_; ++i;});
            i = 1;
            std::for_each(lookup_L_fwd->begin(), lookup_L_fwd->end(), [&](index_type & ind) {ind = i%L_; ++i;});
            i = H_ - 1;
            std::for_each(lookup_H_rev->begin(), lookup_H_rev->end(), [&](index_type & ind) {ind = i%H_; ++i;});
            i = L_ - 1;
            std::for_each(lookup_L_rev->begin(), lookup_L_rev->end(), [&](index_type & ind) {ind = i%L_; ++i;});
            i = 0;
        }
                
        ///  returns a reference to the site (i, j)
        ///  @param i index for the height
        ///  @param j index for the lenght
        grid_type & operator()(index_type const i, index_type const j) {
            return (*grid_)[i][j];
        }
        
        ///  returns a reference to the lower neighbor of the site (i, j)
        ///  @param i index for the height
        ///  @param j index for the lenght
        grid_type & down(index_type const i, index_type const j) {
            //~ return *(((*grid_)[i][j]).down);
            return (*grid_)[(*lookup_H_fwd)[i]][j];
        }
        
        ///  returns a reference to the upper neighbor of the site (i, j)
        ///  @param i index for the height
        ///  @param j index for the lenght
        grid_type & up(index_type const i, index_type const j) {
            //~ return *(((*grid_)[i][j]).up);
            return (*grid_)[(*lookup_H_rev)[i]][j];
        }
        
        ///  returns a reference to the right neighbor of the site (i, j)
        ///  @param i index for the height
        ///  @param j index for the lenght
        grid_type & right(index_type const i, index_type const j) {
            //~ return *(((*grid_)[i][j]).right);
            return (*grid_)[i][(*lookup_L_fwd)[j]];
        }
        
        ///  returns a reference to the left neighbor of the site (i, j)
        ///  @param i index for the height
        ///  @param j index for the lenght
        grid_type & left(index_type const i, index_type const j) {
            //~ return *(((*grid_)[i][j]).left);
            return (*grid_)[i][(*lookup_L_rev)[j]];
        }
        
        ///  \brief print function
        ///  @param os is the stream that is printed into. Default is the std::cout ostream
        void print(std::ostream & os = std::cout) const {
            for(index_type i = 0; i < H_; ++i)
            {
                for(index_type j = 0; j < L_; ++j)
                {
                    os << std::setw(3) << (*grid_)[i][j] << " ";
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
                    in = (*grid_)[i][j].string_print(L_);
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
            return grid_->data();
        }
        
        ///  \brief can be used to traverse the array
        grid_type * end() {
            return grid_->data() + grid_->num_elements();
        }
    private:
        uint const H_; ///<height
        uint const L_; ///<length
        vector_ptr_type<index_type> lookup_H_fwd;   ///< neccesary bc of the boundary conditions (more efficient then if or mod)
        vector_ptr_type<index_type> lookup_L_fwd;   ///< same as grid_class<T>#lookup_H_fwd
        vector_ptr_type<index_type> lookup_H_rev;   ///< same as grid_class<T>#lookup_H_fwd
        vector_ptr_type<index_type> lookup_L_rev;   ///< same as grid_class<T>#lookup_H_fwd
        array_ptr_type<grid_type> grid_;            ///< the actual grid
    };
}//end namespace perimeter
#endif //__GRID_CLASS_HEADER
