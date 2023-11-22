#pragma once

#include "DataCollector.hpp"
#include <cstddef>
#include <initializer_list>
#include <vector>
template <typename T>
concept IsAdd = requires(T a) {
    {
        a + a
    };
};

template <typename T>
concept IsMul = requires(T a) {
    {
        a *a
    };
};

template <typename T>
    requires(IsAdd<T> && IsMul<T>)
struct Matrix
{

    std::vector<std::vector<T>> _data;

    Matrix(int prealloc)
    {
        for (int i = 0; i < prealloc; i++)
        {
            _data.push_back(std::vector<T>{prealloc});
            _data[i].resize(prealloc);
        }
    }

    Matrix()
    {
    }

    Matrix(std::vector<std::vector<T>> data) : _data(data)
    {
    }

    size_t Columns() const
    {
        return _data[0].size();
    }

    size_t Rows() const
    {
        return _data.size();
    }

    bool AddRow(std::vector<T> row)
    {
        if (_data.size() > 0 && row.size() != _data[0].size())
        {
            return false;
        }
        _data.push_back(row);
        return true;
    }

    bool AddCol(std::vector<T> col)
    {
        if (col.size() != _data.size())
        {
            return false;
        }
        for (int i = 0; i < col.size(); i++)
        {
            _data.at(i).push_back(col.at(i));
        }
    }

    void AllocColumns(int toAdd)
    {
        for (std::vector<T> &vec : _data)
        {
            vec.resize(vec.size() + toAdd);
        }
    }

    void AllocRows(int toAdd)
    {
        size_t ref = _data[0].size();
        for (int i = 0; i < toAdd; i++)
        {
            _data.push_back(std::vector<T>{ref});
        }
    }

    T &operator()(int row, int col)
    {
        return _data.at(row).at(col);
    }
};

template <typename T>
    requires(IsAdd<T> && IsMul<T>)
struct DTMCMatrix : public Matrix<T>
{
    using Base = Matrix<T>;
    enum class Result
    {
        NO_SQUARE,
        INVALID_SUM,
        ABSORBING_STATES,
        OK
    };

    Result Validate()
    {
        auto &data = Base::_data;
        if (data.size() != data[0].size())
        {
            return Result::NO_SQUARE;
        }

        for (auto &row : data)
        {
            T sum{};
            for (auto &elem : row)
            {
                sum += elem;
            }
            if (sum != 1.0)
                return Result::INVALID_SUM;
        }

        for (int i = 0; i < data.size(); i++)
        {
            auto &vec = data[i];
            bool fail = true;
            for (int j = 0; j < vec.size(); j++)
            {
                if (i != j && vec[i] > 0.0)
                    fail = false;
            }
            if (fail)
                return Result::ABSORBING_STATES;
        }
        return Result::OK;
    }
};
