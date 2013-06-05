// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    03.06.2013 11:14:36 EDT
// File:    accum_simple.hpp

#ifndef __ACCUM_SIMPLE_HEADER
#define __ACCUM_SIMPLE_HEADER

#include <iostream>
#include <cmath>

using namespace std;

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
        void operator<<(bool const & val) {
            if(val)
                ++sum_;
            ++count_;
        }
        double mean() const {
            return sum_ / double(count_);
            //~ return count_;
        }
        void print(std::ostream & os) const {
            os << sum_ << "/" << count_ << " = ";
            os << mean();
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
