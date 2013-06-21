#ifndef ACC_DOUBLE_HEADER
#define ACC_DOUBLE_HEADER

#include <iostream>
#include <cmath>

class accumulator_double
{
    public:
        accumulator_double(): count_(0), sum_(0), sum2_(0) {
        };
        void operator<<(double const & val) {
            sum_ += val;
            sum2_ += val * val;
            ++count_;
        }
        double mean() const {
            return sum_ / count_;
        }
        double deviation() const {
            using std::sqrt;
            return sqrt(sum2_ / (count_ - 1) - sum_ * sum_ / count_ / (count_ - 1));
        }
        double error() const {
            return deviation() / sqrt(count_);
        }
        void print(std::ostream & os) const {
            os << mean();
            os << "+/-";
            os << error();
        }
        template<typename Archive>
        void serialize(Archive & ar) {
            ar & count_;
            ar & sum_;
            ar & sum2_;
        }
    private:
        uint64_t count_;
        double sum_;
        double sum2_;
};

std::ostream & operator<<(std::ostream & os, accumulator_double const & acc) {
    acc.print(os);
    return os;
}
#endif
