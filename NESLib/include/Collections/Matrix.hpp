#pragma once

#include "Core.hpp"
#include "LogEngine.hpp"
#include <cstdarg>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <optional>
#include <sstream>
#include <string>
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
static T operator*(std::vector<T> a, std::vector<T> b)
{
    if (a.size() != b.size())
    {
        panic("matrix of different sizes");
    }
    T sum{};
    for (int i = 0; i < a.size(); i++)
    {
        sum += a[i] * b[i];
    }
    return sum;
}

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

    Matrix(size_t rows, size_t columns)
    {
        AllocRows(rows);
        AllocColumns(columns);
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

    std::vector<T> Col(int index) const
    {
        std::vector<T> col{};
        for (auto &vec : _data)
        {
            col.push_back(vec.at(index));
        }
        return col;
    }

    std::vector<T> Row(int index) const
    {
        return _data.at(index);
    }

    void operator()(int row, std::vector<T> newRow)
    {
        _data.at(row) = newRow;
    }

    void operator()(std::vector<T> newcolumn, int col)
    {
        for (int i = 0; i < newcolumn.size(); i++)
        {
            (*this)(i, col) = newcolumn[i];
        }
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

    void AllocColumns(size_t toAdd)
    {
        for (std::vector<T> &vec : _data)
        {
            vec.resize(vec.size() + toAdd);
        }
    }

    void AllocRows(size_t toAdd)
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

    Matrix<T> operator*(const Matrix<T> &oth)
    {
        std::vector<std::vector<T>> newdata{};
        for (int i = 0; i < _data.size(); i++)
        {
            auto row = Row(i);
            std::vector<T> newrow{};
            for (int k = 0; k < oth.Columns(); k++)
            {
                newrow.push_back(row * oth.Col(k));
            }
            newdata.push_back(newrow);
        }
        return newdata;
    }

    Matrix<T> operator+(const Matrix<T> &oth)
    {
    }

    Matrix<T> operator*(T value)
    {
    }

    std::string ToString()
    {
        std::string buf{};
        for (auto &row : _data)
        {
            for (auto &e : row)
            {
                buf += makeformat(" {} ,", e);
            }

            buf[buf.size() - 1] = '\n';
        }
        return buf;
    }

    template <typename F> void Traverse(F &&fnc)
    {
        for (auto &row : _data)
            for (auto &col : row)
                fnc(col);
    }
};

template <typename T>
    requires(IsAdd<T> && IsMul<T>)
static std::vector<T> operator*(std::vector<T> &v, Matrix<T> &mat)
{
    std::vector<T> res{};
    for (int i = 0; i < v.size(); i++)
    {
        T sum{};
        for (int k = 0; k < mat.Columns(); k++)
        {
            sum += v[k] * mat.Col(i)[k];
        }
        res.push_back(sum);
    }
    return res;
}

template <typename T>
    requires(IsAdd<T>)
static std::vector<T> operator-(std::vector<T> &a, std::vector<T> &b)
{
    std::vector<T> res{};
    for (int i = 0; i < a.size(); i++)
    {
        res.push_back(a[i] - b[i]);
    }
    return res;
}

template <typename T, typename F>
    requires(IsAdd<T>)
static bool All(std::vector<T> &&t, F &&fnc)
{
    bool res = true;
    for (auto &a : t)
    {
        if (!fnc(a))
            res = false;
    }
    return res;
}