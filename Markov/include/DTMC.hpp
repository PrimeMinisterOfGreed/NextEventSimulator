#pragma once
#include "Collections/Matrix.hpp"
#include <vector>
std::vector<double> vectorMethod(Matrix<double> &dtmc, double threshold);
std::vector<double> powerMatrixMethod(Matrix<double> &dtmc);