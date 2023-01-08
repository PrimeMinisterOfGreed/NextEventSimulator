#include "rngs.hpp"
#include <Generators.hpp>
#include <cmath>

// non credo sia corretto, forse va usato il metodo di composizione
double NegExp(double lambda)
{
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -log(1 - u) / lambda;
}
