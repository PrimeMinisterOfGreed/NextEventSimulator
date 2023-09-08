#include "DataCollector.hpp"
#include "Measure.hpp"
#include "Station.hpp"
#include <map>
#include <vector>

class DataWriter
{
  private:
    std::string _lines[1000]{};
    int _currentIndex = 0;

  public:
    std::string header;
    static DataWriter &Instance();
    void Write(std::string data);
    void Flush();
};
