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
    const IGenerator* _generator;
public:
    BaseRandomVariable(IGenerator *generator);

    int GetStream() const override;

    void SetStream(int stream) override;

};


class StreamGenerator : public IGenerator
{

protected:

    int _currentStream = 0;

public:


    double Random(const IRandomVariable *variable) const override;

    void RegisterRandomVariable(IRandomVariable *variable) override;
};

class NegExpVariable : BaseRandomVariable
{
protected:
    double _lambda;



public:
    double GetValue() const override;
    explicit NegExpVariable(double lambda, IGenerator * generator);
};