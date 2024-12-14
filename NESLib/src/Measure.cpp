/**
 * @file Measure.cpp
 * @author implementazione per gli accumulatori, maggiori informazioni @see Measure.hpp
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Measure.hpp"
#include "Core.hpp"
#include "rvms.h"
#include <cmath>
#include <numeric>
#include <vector>

std::vector<double> slice(std::vector<double>::iterator begin, std::vector<double>::iterator end)
{
    std::vector<double> res{};
    auto itr = begin;
    while (itr != end)
    {
        res.push_back(*itr);
    }
    return res;
}

/**
 * @brief Accumula una coppia (valore,intervallo_di_tempo) sostanzialmente da un 
 * ciclo di rigeneramento
 * 
 * @param value 
 * @param time 
 */
void CovariatedMeasure::Accumulate(double value, double time)
{
    _count++;
    _current[0] = value;
    _current[1] = time;
    _sum[0] += value;
    _sum[1] += value * value;
    _times[0] += time;
    _times[1] += time * time;
    _weightedsum += (value * time);
}

/**
 * @brief ritorna lo stimatore puntuale calcolato partendo dalle variabili accumulate
 * 
 * @return double 
 */
double CovariatedMeasure::R() const
{
    return _sum[0] / _times[0];
}


/**
 * @brief Calcola l'intervallo di confidenza usando la formula proposta dalle slide
 * assieme alla distribuzione t_{\infty}. Non controlla quanti campioni ha accumulato
 * per dare l'opportunità al compilatore di ottimizzare il valore e non calcolarlo ogni volta
 * è cura dello sviluppatore assicurarsi che almeno 40 campioni vengano raccolti
 * @return Interval 
 */
Interval CovariatedMeasure::confidence() const
{
    double alpha = 1 - _confidence;
    double r = _sum[0] / _times[0];
    double a = sqrt(((double)_count / (_count - 1.0)));
    double b = (sqrt(_sum[1] - (2 * R() * _weightedsum) + (pow(R(), 2) * _times[1])));
    double delta = a * (b / _times[0]);
    double t = idfNormal(0, 1, 1 - (alpha / 2));
    return Interval(R(), delta ,t);
}


