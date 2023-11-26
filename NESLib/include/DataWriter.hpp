#include "DataCollector.hpp"
#include "Measure.hpp"
#include "Station.hpp"
#include <map>
#include <vector>

class DataWriter
{
  private:
    struct DataFile
    {
        std::string fileName{};
        std::vector<DataCollector *> collectors{};
        bool isInCategory(const DataCollector &collector)
        {
            if (collectors.size() == 0)
                return true;
            if (collectors.at(0)->Header() == collector.Header())
                return true;
            return false;
        }
    };
    int _currentIndex = 0;
    std::vector<DataFile> _dataFiles;

  protected:
    void WriteFile(const DataFile &dataFile);

  public:
    std::string header;
    static DataWriter &Instance()
    {
        static DataWriter instance{};
        return instance;
    }

    void Register(DataCollector *collector);
    void Flush();
    void SnapShot();
};
