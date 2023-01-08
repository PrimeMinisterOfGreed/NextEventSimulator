#include "DataProvider.hpp"
#include "Generators.hpp"
#include "rngs.hpp"
#include <stdexcept>

enum
{
    ARRIVAL,
    INTERARRIVAL,
    SERVICE
};

double GetValue(std::ifstream &stream)
{
    float result = -1;
    stream >> result;
    return result;
}

double BaseDataProvider::GetArrival()
{
    if (_dataUsed[ARRIVAL])
        Next();
    _dataUsed[ARRIVAL] = true;
    return _data[ARRIVAL];
}

double BaseDataProvider::GetService()
{
    if (_dataUsed[SERVICE])
        Next();
    _dataUsed[SERVICE] = true;
    return _data[SERVICE];
}

double BaseDataProvider::GetInterArrival()
{
    if (_dataUsed[INTERARRIVAL])
        Next();
    _dataUsed[INTERARRIVAL] = true;
    return _data[INTERARRIVAL];
}

BaseDataProvider::BaseDataProvider()
{
  
}

void TraceDrivenDataProvider::Next()
{
    if ((!_dataUsed[ARRIVAL] || !_dataUsed[SERVICE]) && _init)
        throw std::invalid_argument("Trying to acquire new data when the old one is not used");
    _dataUsed[ARRIVAL] = false;
    _dataUsed[SERVICE] = false;
    _dataUsed[INTERARRIVAL] = false;
    if (_interArrivalMode)
    {

        auto interarrival = GetValue(_filestream);
        _data[ARRIVAL] += interarrival;
        _data[INTERARRIVAL] = interarrival;
    }
    else
    {
        auto arrival = GetValue(_filestream);
        _data[INTERARRIVAL] = arrival - _data[ARRIVAL];
        _data[ARRIVAL] = arrival;
    }
    _data[SERVICE] = GetValue(_filestream);
    if (_data[SERVICE] < 0)
        _end = true;
}

TraceDrivenDataProvider::TraceDrivenDataProvider(std::string &filePath, bool interArrivalMode)
    : _filestream{*new std::ifstream{}}, _interArrivalMode{interArrivalMode}
{
    _filestream.open(filePath);
    Next();
    _init = true;
}

inline bool DoubleStreamNegExpRandomDataProvider::end()
{
    return _data[ARRIVAL] >= _endTime;
}

void DoubleStreamNegExpRandomDataProvider::Next()
{
    if ((!_dataUsed[ARRIVAL] || !_dataUsed[SERVICE]) && _init)
        throw std::invalid_argument("Trying to acquire new data when the old one is not used");
    _dataUsed[ARRIVAL] = false;
    _dataUsed[SERVICE] = false;
    _dataUsed[INTERARRIVAL] = false;
    SelectStream(1);
    double interarrival = NegExp(0.1);
    SelectStream(2);
    double serviceTime = NegExp(0.14);

    if (_data[INTERARRIVAL] != 0)
    {
        _data[ARRIVAL] +=  interarrival;
    }
    _data[INTERARRIVAL] = interarrival;
    _data[SERVICE] = serviceTime;
}

DoubleStreamNegExpRandomDataProvider::DoubleStreamNegExpRandomDataProvider(double endTime) : _endTime(endTime)
{
    Next();
    _init = true;
}
