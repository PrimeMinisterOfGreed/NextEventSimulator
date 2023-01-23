#include <Generators.hpp>
#include <cmath>
#include "rngs.hpp"

double NegExp(double lambda)
{
    return -log(1 - Random()) / lambda;
}


StreamGenerator* StreamGenerator::_instance;

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
    if(_instance == nullptr)
        _instance = new StreamGenerator();
    return _instance;
}


double NegExpVariable::GetValue() const
{
    return -log(1 - _generator->Random(this)) / _lambda;
}

NegExpVariable::NegExpVariable(double lambda, IGenerator * generator) : _lambda(lambda), BaseRandomVariable(generator)
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

BaseRandomVariable::BaseRandomVariable(IGenerator *generator): _generator(generator)
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
