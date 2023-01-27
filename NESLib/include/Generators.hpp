#pragma once

#include "rngs.hpp"
#include <numbers>

double NegExp(double lambda);

struct BaseStream
{
	int Stream;
};

class IRandomVariable
{

public:
	virtual double GetValue()  = 0;

	virtual BaseStream* GetStream()  = 0;
};


class IGenerator
{
public:
	
	virtual double Random(const BaseStream * stream) const = 0;
	virtual BaseStream* GetStream() = 0;
};

class BaseRandomVariable : public IRandomVariable
{
protected:
	bool _registered = false;
	BaseStream* _stream;
	IGenerator* _generator;
public:
	explicit BaseRandomVariable(IGenerator* generator);
	BaseStream* GetStream() override;
    virtual double GetValue() override;
};


class StreamGenerator : public IGenerator
{

protected:

	int _currentStream = 0;
	static StreamGenerator* _instance;
public:

	static StreamGenerator* Instance();
	virtual double Random(const BaseStream* stream) const;
	virtual BaseStream* GetStream();
	
};

class RandomVariable : public BaseRandomVariable
{
public:
	RandomVariable(IGenerator* generator);

	double GetValue() override ;
};

class NegExpVariable : BaseRandomVariable
{
protected:
	double _lambda;


public:
	double GetValue() override;

	explicit NegExpVariable(double lambda, IGenerator* generator);
};

class DoubleStageHyperExpVariable : BaseRandomVariable
{
private:
	double _alpha;
	double _beta;
	double _u1;
	double _u2;

public:
	double GetValue() override;
	DoubleStageHyperExpVariable(double alpha, double beta, double u1, double u2, IGenerator* generator);
};