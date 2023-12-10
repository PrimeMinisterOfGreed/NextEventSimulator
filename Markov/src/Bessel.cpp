#include "Bessel.hpp"
#include <boost/math/special_functions/math_fwd.hpp>
#include <cmath>
double Bessel(int order, int limit, int pos)
{
    auto term1 = [](int x, int m, int k) {
        double ex = k + 2 * m;
        return std::pow(x / 2, ex);
    };
    auto term2 = [](int k, int m) { return boost::math::factorial<double>(k + m) * boost::math::factorial<double>(m); };
    auto sum = 0.0;
    for (int m = 0; m < limit; m++)
    {
        sum += term1(pos, m, order) / term2(order, m);
    }
    return sum;
}
