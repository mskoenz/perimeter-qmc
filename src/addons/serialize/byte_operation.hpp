// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    29.05.2013 18:53:20 EDT
// File:    bitoperation.hpp

#ifndef __BITOPERATION_HEADER
#define __BITOPERATION_HEADER

#include <iostream> //for uint_8

namespace util {

    namespace detail {
        typedef uint8_t byte_op_size_type;
    }
    template<typename T>
    inline void clear_byte(T & t, detail::byte_op_size_type const & pos) {
        uint8_t * p = (uint8_t *) & t;
        p[pos] = 0;
    }
    template<typename T>
    inline uint8_t read_byte(T const & t, detail::byte_op_size_type const & pos) {
        uint8_t * p = (uint8_t *) & t;
        return p[pos];
    }
    template<typename T>
    inline void write_byte(T & t, detail::byte_op_size_type const & pos, uint8_t const & in) {
        uint8_t * p = (uint8_t *) & t;
        p[pos] = in;
    }
    
    template<typename T>
    inline bool read_bit(T const & unit, uint8_t const & pos) {
        return (unit & (T(1) << pos));
    }
    template<typename T>
    inline void set_bit(T & unit, uint8_t const & pos) {
        unit |= (T(1) << pos);
    }
    template<typename T>
    inline void clear_bit(T & unit, uint8_t const & pos) {
        unit &= ~(T(1) << pos);
    }
    template<typename T>
    inline void write_bit(T & unit, uint8_t const & pos, bool state = true) { 
        state ? set_bit(unit, pos) : clear_bit(unit, pos);
    }
    template<typename T>
    inline void flip_bit(T & unit, uint8_t const & pos) {
        unit ^= (T(1) << pos);
    }
    
}//end namespace util
#endif //__BITOPERATION_HEADER
