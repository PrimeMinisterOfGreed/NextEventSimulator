#pragma once

#include "rngs.hpp"
#include "rvgs.h"
#include <numbers>
#include "EventHandler.hpp"


class IRandomVariable
{
protected:
	virtual double GetValue() = 0;
public:
    double operator()(){GetValue();}
};





class NegExpVariable : public BaseRandomVariable
{
protected:
	double _lambda;
	double GetValue() override;
public:
	explicit NegExpVariable(double lambda, IGenerator* generator);
};

class DoubleStageHyperExpVariable : public BaseRandomVariable
{
private:
	double _alpha;
	double _beta;
	double _u1;
	double _u2;
protected:
	double GetValue() override;

public:
	DoubleStageHyperExpVariable(double alpha, double beta, double u1, double u2, IGenerator* generator);
};