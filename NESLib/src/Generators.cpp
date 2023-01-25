#include <Generators.hpp>
#include <cmath>
#include "rngs.hpp"

double NegExp(double lambda)
{
    return -log(1 - Random()) / lambda;
}


StreamGenerator *StreamGenerator::_instance;

double StreamGenerator::Random(const IRandomVariable *variable) const
{
    SelectStream(variable->GetStream());
    return ::Random();
}

void StreamGenerator::RegisterRandomVariable(IRandomVariable *variable)
{
    variable->SetStream(_currentStream);
    _currentStream++;
}


StreamGenerator *StreamGenerator::Instance()
{
    if (_instance == nullptr)
        _instance = new StreamGenerator();
    return _instance;
}


double NegExpVariable::GetValue() const
{
    return -log(1 - _generator->Random(this)) / _lambda;
}

NegExpVariable::NegExpVariable(double lambda, IGenerator *generator) : _lambda(lambda), BaseRandomVariable(generator)
{

}


int BaseRandomVariable::GetStream() const
{
    return _stream;
}

void BaseRandomVariable::SetStream(int stream)
{
    _stream = stream;
}

BaseRandomVariable::BaseRandomVariable(IGenerator *generator) : _generator(generator)
{
    generator->RegisterRandomVariable(this);
}

double RandomVariable::GetValue() const
{
    return _generator->Random(this);
}

RandomVariable::RandomVariable(IGenerator *generator) : BaseRandomVariable(generator)
{

}

double DoubleStageHyperExpVariable::GetValue() const
{
    double x = _generator->Random(this);
    return _alpha*(1/_u1)* exp(-x/_u1)+_beta*(1/_u2)* exp(-x/_u2);
}

DoubleStageHyperExpVariable::DoubleStageHyperExpVariable(double alpha, double beta, double u1, double u2,
                                                         IGenerator *generator) : BaseRandomVariable(generator),
                                                                                  _alpha(alpha), _u1(u1), _u2(u2),
                                                                                  _beta(beta)
{

}

