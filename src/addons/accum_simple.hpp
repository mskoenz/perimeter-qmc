// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    03.06.2013 11:14:36 EDT
// File:    accum_simple.hpp

#ifndef __ACCUM_SIMPLE_HEADER
#define __ACCUM_SIMPLE_HEADER

#include <iostream>
#include <cmath>

class accumulator_simple
{
    typedef uint64_t T;
    public:
        accumulator_simple(): count_(T()), sum_(T()) {
        };
        void operator<<(T const & val) {
            sum_ += val;
            ++count_;
        }
        double mean() const {
            return sum_ / double(count_);
        }
        void print(std::ostream & os) const {
            os << sum_ << "/" << count_ << " = ";
            os << mean();
        }
        template<typename Archive>
        void serialize(Archive & ar) {
            ar & count_;
            ar & sum_;
        }
    private:
        T count_;
        T sum_;
};

std::ostream & operator<<(std::ostream & os, accumulator_simple const & acc) {
    acc.print(os);
    return os;
}
#endif //__ACCUM_SIMPLE_HEADER
