#pragma once
#include <iostream>
#include <fstream>


class IDataProvider
{
public:
	virtual double GetArrival() = 0;
	virtual double GetService() = 0;
	virtual double GetInterArrival() = 0;
	virtual void Next() = 0;
	virtual bool end() = 0;
};

class FileDataProvider : public IDataProvider
{
private:
	std::ifstream& _filestream;
	bool _end = false;
	bool _interArrivalMode = false;
	double data[3];
public:
	
	// Ereditato tramite IDataProvider
	virtual double GetArrival() override;
	virtual double GetService() override;
	virtual double GetInterArrival() override;
	virtual void Next() override;
	bool IsEof() const { return _filestream.eof() || _end; }
	FileDataProvider(std::string& filePath, bool interArrivalMode = false);
	virtual bool end() override { return IsEof(); }
};

