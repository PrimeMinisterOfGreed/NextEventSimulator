#include "DataProvider.hpp"

enum
{
	ARRIVAL,
	INTERARRIVAL,
	SERVICE
};

double GetValue(std::ifstream& stream)
{
	float result = -1;
	stream >> result;
	return result;
}

double FileDataProvider::GetArrival()
{
	return data[ARRIVAL];
}

double FileDataProvider::GetService()
{
	return data[SERVICE];
}

double FileDataProvider::GetInterArrival()
{
	return data[INTERARRIVAL];
}

void FileDataProvider::Next()
{
	if (_interArrivalMode)
	{

		auto interarrival = GetValue(_filestream);
		data[ARRIVAL] += data[INTERARRIVAL] + interarrival;
		data[INTERARRIVAL] = interarrival;
	}
	else
	{
		auto arrival = GetValue(_filestream);
		data[INTERARRIVAL] = arrival - data[ARRIVAL];
		data[ARRIVAL] = arrival;
	}
	data[SERVICE] = GetValue(_filestream);
	if (data[SERVICE] < 0) _end = true;
}

FileDataProvider::FileDataProvider(std::string& filePath, bool interArrivalMode): _filestream{*new std::ifstream{}},_interArrivalMode{interArrivalMode},data{0,0,0}
{
	_filestream.open(filePath);
}
