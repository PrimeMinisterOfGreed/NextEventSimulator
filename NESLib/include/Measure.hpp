/**
 * @file Measure.hpp
 * @author matteo.ielacqua
 * @brief contiene una raccolta di accumulatori, implementati sulla falsa riga di ciò che offre
 * la libreria boost::accumulator
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "Core.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "rvms.h"
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include "fmt/format.h"
#include <functional>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <valarray>
#include <vector>

/**
 * @brief classe che rappresenta un intervallo di confidenza
 * restituisce confine superiore, confine inferiore ,larghezza dell'intervallo e precisione dell'intervallo
 */
struct Interval
{
    double _mean;
    double _delta;
    double _t;
  public:
    Interval(double mean, double delta,double t) : _mean(mean), _delta(delta),_t(t)
    {
    }

    double higher() const
    {
        return _mean + _delta*_t;
    }
    double lower() const
    {
        return _mean - _delta*_t;
    }

    double width() const
    {
        return higher() - lower();
    }

    double precision() const
    {
        return _delta / _mean;
    }

    bool isInTval(double tval)
    {
        return lower() <= tval && tval <= higher();
    }

    double tvalDiff(double tval)
    {
        if (tval < lower())
            return lower() - tval;
        if (tval > higher())
            return higher() - tval;
        return 0.0;
    }
};

/**
 * @brief Classe base che rappresenta i metodi comuni a tutte le misure 
 * 
 */
class BaseMeasure
{
  protected:
    std::string _name;
    std::string _unit;
    size_t _count{};

  public:
    BaseMeasure(std::string name, std::string unit) : _name(name), _unit(unit), _count(0)
    {
    }
    std::string Name() const
    {
        return _name;
    }
    size_t Count() const
    {
        return _count;
    }
    std::string Unit() const
    {
        return _unit;
    }
    virtual std::string Heading()
    {
        return fmt::format("{}({})", Name(), Unit());
    }

    virtual void Reset()
    {
        _count = 0;
    }

    virtual bool operator==(BaseMeasure &measure)
    {
        return this->_name == measure.Name() && this->Unit() == measure.Unit();
    }

    virtual bool operator!=(BaseMeasure &measure)
    {
        return !(*this == measure);
    }
    virtual ~BaseMeasure() = default;
};

/**
 * @brief classe generica per i tipi che possono essere accumulati 
 * e possiedono il concetto di unità di misura
 * @tparam T 
 */
template <typename T> class Measure : public BaseMeasure
{
  private:
    T _lastAccumulatedValue{};

  public:
    Measure(std::string name, std::string unit) : BaseMeasure(name, unit)
    {
    }

    virtual void Accumulate(T value)
    {
        _lastAccumulatedValue = value;
        _count++;
    }

    T Current() const
    {
        return _lastAccumulatedValue;
    }

    virtual std::string Csv()
    {
        return fmt::format("{}", _lastAccumulatedValue);
    }

    virtual std::string Json()
    {
        return fmt::format("\"lastAccumulatedValue\":{}", _lastAccumulatedValue);
    }

    void operator()(T value)
    {
        Accumulate(value);
        core_assert(_lastAccumulatedValue != NAN, "Value {} is drifting", Name());
    }

    void Reset() override
    {
        BaseMeasure::Reset();
        _lastAccumulatedValue = T{};
    }
};

/**
 * @brief Classe accumulatrice per misure che hanno diversi momenti 
 * sulla media, vista questa caratteristica il tipo accettato è solo double 
 * utilizzata nel progetto per accumulare l'active time del tagged customer entro 
 * il ciclo di rigeneramento
 * @tparam Moments 
 */
template <int Moments = 2> class Accumulator : public Measure<double>
{

  private:
    double _confidence = 0.90;
    double _precision = 0.05;
    double _sum[Moments]{};

    double get(int moment) const
    {
        if (moment >= Moments)
            panic("moment selected is over Moments stored");
        return _sum[moment];
    }

    void constexpr ForMoment(std::function<void(double &, int)> &&operation)
    {
        for (int i = 0; i < Moments; i++)
            operation(_sum[i], i + 1);
    }

  public:
    Accumulator(std::string name, std::string unit) : Measure<double>(name, unit)
    {
    }

    Accumulator() : Measure<double>("", "")
    {
    }

    Accumulator<> &WithConfidence(double confidence)
    {
        _confidence = confidence;
        return *this;
    }

    Accumulator<> &WithPrecision(double precision)
    {
        _precision = precision;
        return *this;
    }

    void Accumulate(double value) override
    {
        Measure<double>::Accumulate(value);
        ForMoment([value](double &val, int moment) {
            val += pow(value, moment);
            core_assert(val != NAN, "Value {} is drifting in moment {}", val, moment);
        });
    }

    std::string Heading() override
    {
        std::string head = fmt::format("{};", Measure<double>::Heading());
        auto name = Measure<double>::Name();
        head += fmt::format("meanOf{};", name);
        head += fmt::format("varianceOf{};", name);
        head += fmt::format("lowerBoundOf{};", name);
        head += fmt::format("upperBoundOf{}", name);
        return head;
    }

    std::string Csv() override
    {
        std::string csv = fmt::format("{};", Measure<double>::Csv());
        csv += fmt::format("{};", mean());
        csv += fmt::format("{};", variance());
        csv += fmt::format("{};", confidence().higher());
        csv += fmt::format("{}", confidence().lower());
        return csv;
    }

    std::string Json() override
    {
        return fmt::format("{},\n \"samples\":{}\n \"mean\":{},\n \"variance\":{},\n \"lower_bound\":{},\n "
                           "\"higher_bound\":{} \n \"width\":{},\n \"precision\":{}\n",
                           Measure<double>::Json(), this->Count(), mean(), variance(), confidence().lower(),
                           confidence().higher(), confidence().width(), confidence().precision());
    }

    virtual void Reset() override
    {
        Measure<double>::Reset();
        ForMoment([](double &val, int moment) { val = 0; });
    }

    inline double mean(int moment = 0) const
    {
        return get(moment) / Count();
    }

    inline double variance() const
    {
        return mean(1) - pow(mean(0), 2); // this can lead to catastrophic cancellation
    }

    inline double sum() const
    {
        return _sum[0];
    }

    Interval confidence()
    {

        double u = mean(0);
        double sigma = variance();
        double alpha = 1 - _confidence;
        auto count = Count();
        double delta = 0.0;
        auto t = 0.0;
        if (count < 40)
        {
            t = idfStudent(count - 1, 1 - (alpha / 2)) * (sigma / sqrt(count - 1));
        }
        else
        {
            t = idfNormal(0.0, 1.0, 1 - alpha / 2);
        }
        delta = (variance()) / sqrtf(count);
        return {u, delta,t};
    }
};

/**
 * @brief formattatore per la classe accumulator 
 * 
 * @tparam  
 */
template <> struct fmt::formatter<Accumulator<>>
{
    char mode[16]{};
    constexpr auto parse(format_parse_context &ctx) -> format_parse_context::iterator
    {
        auto itr = ctx.begin();
        int i = 0;
        while (itr != ctx.end() && *itr != '}')
        {
            mode[i] = *itr;
            itr++;
            i++;
        }
        return itr;
    }

    auto format(Accumulator<> m, format_context &ctx) const -> format_context::iterator
    {

        if (strcmp(mode, "csv") == 0)
        {
            return fmt::format_to(ctx.out(), "{};{};{};{};{};{}", m.Name(), m.mean(), m.confidence().lower(),
                                  m.confidence().higher(), m.Count(), m.confidence().precision());
        }
        else
        {
            return fmt::format_to(ctx.out(), "Measure: {}, R: {},LB:{}, HB:{}, Samples:{}, Variance:{} ,Precision:{}",
                                  m.Name(), m.mean(), m.confidence().lower(), m.confidence().higher(), m.Count(),
                                  m.variance(), m.confidence().precision());
        }
    }
};

/**
 * @brief classe che rappresenta le misure che vanno accumulate
 * considerando la loro forte natura correlata, come ad esempio tutte le osservazioni
 * compiute nell'ambito di una simulazione con il metodo rigenerativo
 */
struct CovariatedMeasure : BaseMeasure
{
    double _confidence = 0.90;
    double _precision = 0.05;
    double _current[2]{};
    double _sum[2]{};
    double _times[2]{};
    double _weightedsum{};
    virtual void Accumulate(double value, double time);

    double R() const;
    double variance() const;
    Interval confidence() const;

    CovariatedMeasure(std::string name, std::string unit) : BaseMeasure(name, unit)
    {
    }

    CovariatedMeasure() : BaseMeasure("", "")
    {
    }

    CovariatedMeasure &WithConfidence(double confidence)
    {
        _confidence = confidence;
        return *this;
    }

    CovariatedMeasure &WithPrecision(double precision)
    {
        _precision = precision;
        return *this;
    }

    std::pair<double, double> Current()
    {
        return {_current[0], _current[1]};
    }

    void operator()(double value, double time)
    {
        Accumulate(value, time);
    }

    void Reset() override
    {
        memset(_sum, 0, sizeof(double) * 2);
        memset(_times, 0, sizeof(double) * 2);
        _weightedsum = 0;
        BaseMeasure::Reset();
    }

    double times(int moment = 0)
    {
        return _times[moment];
    }

    double sum(int moment = 0)
    {
        return _sum[moment];
    }
};

/**
 * @brief formattatore per la classe covariated measure 
 * 
 * @tparam  
 */
template <> struct fmt::formatter<CovariatedMeasure>
{
    char mode[16]{};
    constexpr auto parse(format_parse_context &ctx) -> format_parse_context::iterator
    {
        auto itr = ctx.begin();
        int i = 0;
        while (itr != ctx.end() && *itr != '}')
        {
            mode[i] = *itr;
            itr++;
            i++;
        }
        return itr;
    }

    auto format(const CovariatedMeasure &m, format_context &ctx) const -> format_context::iterator
    {

        if (strcmp(mode, "csv") == 0)
        {
            return fmt::format_to(ctx.out(), "{};{};{};{};{};{}", m.Name(), m.R(), m.confidence().lower(),
                                  m.confidence().higher(), m.Count(), m.confidence().precision());
        }
        else
        {
            return fmt::format_to(ctx.out(), "Measure: {}, R: {:5f},LB:{:5f}, HB:{:5f}, Samples:{} ,Precision:{:5f}", m.Name(),
                                  m.R(), m.confidence().lower(), m.confidence().higher(), m.Count(),
                                  m.confidence().precision());
        }
    }
};


