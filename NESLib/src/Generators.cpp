#include <Generators.hpp>
#include <cmath>
#include "rngs.hpp"

double NegExp(double lambda)
{
    return -log(1 - Random()) / lambda;
}




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
