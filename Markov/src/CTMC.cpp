#include "CTMC.hpp"
#include "DTMC.hpp"
void CTMC::Rasterize()
{
    for (int i = 0; i < _data.size(); i++)
    {
        auto row = Row(i);
        double sum = 0;
        for (int k = 0; k < row.size(); k++)
        {
            if (i != k)
                sum += row[k];
        }
        row[i] = sum;
        (*this)(i, row);
    }
}

DTMC CTMC::ToDtmc()
{
    double max = (*this)(0, 0);
    Traverse([this, &max](double e) {
        if (e > max)
            max = e;
    });

    DTMC res{_data};
}
