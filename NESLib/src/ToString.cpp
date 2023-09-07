#include "ToString.hpp"
#include "Collections/LinkedList.hpp"
#include <string>



template <> std::string EventList::ToString()
{
    std::stringstream buffer{};
    auto ptr = begin();
    auto en = end();
    while (ptr != en)
    {
        buffer << "|Job:" << (*ptr).Name << ",TA:" << std::to_string((*ptr).ArrivalTime)
               << " ,TS:" << std::to_string((*ptr).ServiceTime) << ", Type:" << (char)(*ptr).Type
               << ", TO:" << std::to_string((*ptr).OccurTime) << "|-->";
        ptr++;
    }
    return *new std::string(buffer.str());
}