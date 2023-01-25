#pragma once

#include "rngs.hpp"
#include <numbers>

double NegExp(double lambda);

class IRandomVariable
{

public:
    virtual double GetValue() const = 0;

    virtual int GetStream() const = 0;

    virtual void SetStream(int stream) = 0;
};


class IGenerator
{
public:
    virtual double Random(const IRandomVariable *variable) const = 0;

    virtual void RegisterRandomVariable(IRandomVariable *variable) = 0;
};

class BaseRandomVariable : public IRandomVariable
{
protected:
    int _stream = -1;
    const IGenerator *_generator;
public:
    explicit BaseRandomVariable(IGenerator *generator);

    int GetStream() const override;

    void SetStream(int stream) override;

};


class StreamGenerator : public IGenerator
{

protected:

    int _currentStream = 0;
    static StreamGenerator *_instance;
public:
    static StreamGenerator *Instance();

    double Random(const IRandomVariable *variable) const override;

    void RegisterRandomVariable(IRandomVariable *variable) override;
};

class RandomVariable : public BaseRandomVariable
{
public:
    RandomVariable(IGenerator *generator);

    double GetValue() const override;
};

class NegExpVariable : BaseRandomVariable
{
protected:
    double _lambda;


public:
    double GetValue() const override;

    explicit NegExpVariable(double lambda, IGenerator *generator);
};

class DoubleStageHyperExpVariable : BaseRandomVariable
{
private:
    double _alpha;
    double _beta;
    double _u1;
    double _u2;

public:
    double GetValue() const override;
    DoubleStageHyperExpVariable(double alpha, double beta, double u1, double u2, IGenerator * generator);
};