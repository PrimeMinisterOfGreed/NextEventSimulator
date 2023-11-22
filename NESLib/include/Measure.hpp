//
// Created by drfaust on 03/02/23.
//

#pragma once

#include "EventHandler.hpp"
#include "LogEngine.hpp"
#include "rvms.h"
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>

using Interval = std::pair<double, double>;

class BaseMeasure
{
  protected:
    std::string _name;
    std::string _unit;
    size_t _count;

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
        return makeformat("{}({})", Name(), Unit());
    }
};

template <typename T> class Measure : public BaseMeasure
{
  private:
    T _lastAccumulatedValue;

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
        return makeformat("{}", _lastAccumulatedValue);
    }

    void operator()(T value)
    {
        Accumulate(value);
    }
};

template <typename T = double, int Moments = 2> class Accumulator : public Measure<T>
{

  private:
    T _sum[Moments];

    T get(int moment) const
    {
        if (moment >= Moments)
            throw std::invalid_argument("moment selected is over Moments stored");
        return _sum[moment];
    }

    void constexpr ForMoment(std::function<void(T &, int)> operation)
    {
        for (int i = 0; i < Moments; i++)
            operation(_sum[i], i + 1);
    }

  public:
    EventHandler<T> OnCatch;

    Accumulator(std::string name, std::string unit) : Measure<T>(name, unit)
    {
    }

    void Accumulate(T value) override
    {
        Measure<T>::Accumulate(value);
        ForMoment([value](T &val, int moment) { val += pow(value, moment); });

        OnCatch.Invoke(value);
    }

    std::string Heading() override
    {
        std::string head = makeformat("{};", Measure<T>::Heading());
        auto name = Measure<T>::Name();
        head += makeformat("meanOf{};", name);
        head += makeformat("varianceOf{};", name);
        head += makeformat("lowerBoundOf{};", name);
        head += makeformat("upperBoundOf{}", name);
        return head;
    }

    std::string Csv() override
    {
        std::string csv = makeformat("{};", Measure<T>::Csv());
        csv += makeformat("{};", mean());
        csv += makeformat("{};", variance());
        csv += makeformat("{};", confidence().first);
        csv += makeformat("{}", confidence().second);
        return csv;
    }

    void Reset()
    {
        ForMoment([](T &val, int moment) { val = 0; });
    }

    inline T mean(int moment = 0) const
    {
        return get(moment) / this->Count();
    }

    inline T variance() const
    {
        return mean(1) - pow(mean(0), 2); // this can lead to catastrophic cancellation
    }

    inline T sum() const
    {
        return _sum[0];
    }

    Interval confidence()
    {
        double u = mean(0);
        double sigma = variance();
        double alpha = 1 - 0.95;
        auto count = this->Count();
        double delta = idfStudent(count - 1, 1 - (alpha / 2)) * (sigma / sqrt(count - 1));
        return {u - delta, u + delta};
    }
};

template <typename T = double, int Moments = 2> class CovariatedMeasure
{
  private:
    T _acc[Moments];
    Accumulator<T, Moments> &_m1;
    Accumulator<T, Moments> &_m2;
    size_t _count = 0;

  public:
    CovariatedMeasure(Accumulator<T, Moments> &m1, Accumulator<T, Moments> &m2) : _m1{m1}, _m2(m2)
    {
    }

    CovariatedMeasure() : _m1{*new Accumulator<T, Moments>{}}, _m2{*new Accumulator<T, Moments>{}}
    {
    }

    void Accumulate(T v1, T v2)
    {
        _m1(v1);
        _m2(v2);
    }

    void operator()(T v1, T v2)
    {
        Accumulate(v1, v2);
    }

    T covariation() const
    {
    }
};
