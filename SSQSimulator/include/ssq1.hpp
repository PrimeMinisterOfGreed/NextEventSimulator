#pragma once
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include "DataProvider.hpp"
using namespace boost::accumulators;

using Accumulator = accumulator_set<float, features<tag::mean, tag::variance, tag::sum,tag::min,tag::max>>;


	class ISSQSimulator
	{
	protected:
		long jobIndex = 0;
		Accumulator delays;
		Accumulator waits;
		Accumulator interarrivals;
		Accumulator services;

	public:
		virtual void Execute() = 0;
		virtual void Report();
	};

	class SSQSimulator : public ISSQSimulator
	{
	private:
	
	protected:
		float lastArrival = 0;
		int delayedJobs = 0;
		int numberOfCustomer = 0;
		IDataProvider* _provider;
	public:
		virtual void Execute() override;
		SSQSimulator(IDataProvider* provider): _provider{provider}{}
	};


	class SSQSimulator2 : public SSQSimulator
	{
	public:
		virtual void Execute() override;
		SSQSimulator2(IDataProvider* provider) : SSQSimulator(provider){}
	};