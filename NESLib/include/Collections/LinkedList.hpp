#pragma once

#include "Event.hpp"
#include "LogEngine.hpp"
#include "Node.hpp"
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <fmt/core.h>
#include <fmt/format.h>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

template <class T>
    requires Comparable<T>
class DoubleLinkedList
{
  private:
    Node<T> *_begin;
    Node<T> *_end;
    size_t _count = 0;

  public:
    NodeIterator<T> begin() const
    {
        return NodeIterator(_begin);
    }

    NodeIterator<T> end() const
    {
        return NodeIterator(_end);
    }

    inline size_t Count() const
    {
        return _count;
    }

    void Push(T val);
    T Pull();
    void Enqueue(T val);
    T Dequeue();
    void Insert(T val, int index);
    void Insert(T val, std::function<bool(const T &, const T &)> comparer);
    void Clear();
    int IndexOf(T val);
    bool Contains(T val);
    DoubleLinkedList<T> Take(std::function<bool(const T &)> predicate);
    std::string ToString();
    ~DoubleLinkedList();
};

template <class T>
    requires Comparable<T>
bool DoubleLinkedList<T>::Contains(T val)
{
    return IndexOf(val) != -1;
}

template <class T>
    requires Comparable<T>
int DoubleLinkedList<T>::IndexOf(T val)
{
    auto itr = begin();
    int p = 0;
    while (itr != end())
    {
        if (itr() == val)
            return p;
        itr++;
    }
    return -1;
}

template <class T>
    requires Comparable<T>
void DoubleLinkedList<T>::Push(T val)
{
    Node<T> *new_node = new Node<T>(val);
    if (_begin != nullptr)
        *_begin << *new_node;
    if (_end == nullptr)
        _end = new_node;
    _begin = new_node;
    _count++;
}

template <class T>
    requires Comparable<T>
T DoubleLinkedList<T>::Pull()
{
    if (_begin == nullptr)
        throw new std::invalid_argument("Trying to pull from the list but is empty");
    Node<T> *res = _begin;
    if (_begin != _end)
    {
        _begin = _begin->Next();
        if (_begin != nullptr)
            _begin->_previous = nullptr;
    }
    else
    {
        _begin = nullptr;
        _end = nullptr;
    }
    auto val = res->Value();
    _count--;
    res->_next = nullptr;
    res->_previous = nullptr;
    delete res;
    return val;
}

template <class T>
    requires Comparable<T>
inline void DoubleLinkedList<T>::Enqueue(T val)
{
    Node<T> *newNode = new Node<T>(val);
    if (_begin == nullptr)
    {
        _begin = newNode;
        _end = newNode;
    }
    else
    {
        auto ptr = _end;
        *ptr >> *newNode;
        *newNode << *ptr;
        _end = newNode;
    }
    _count++;
}

template <class T>
    requires Comparable<T>
inline T DoubleLinkedList<T>::Dequeue()
{
    return Pull();
}

template <class T>
    requires Comparable<T>
void DoubleLinkedList<T>::Insert(T val, int index)
{
    if (index > _count)
        throw new std::invalid_argument("Index out of bounds");
    if (index == 0)
        Push(val);
    else if (index == _count)
        Enqueue(val);
    else
    {
        Node<T> *newNode = new Node<T>(val);
        auto itr = (begin() + index);
        auto ptr = itr();
        Node<T> &prev = (itr--)();
        prev >> newNode;
        ptr << newNode;
        _count++;
    }
}

template <class T>
    requires Comparable<T>
void DoubleLinkedList<T>::Insert(T val, std::function<bool(const T &, const T &)> comparer)
{
    Node<T> *newNode = new Node<T>(val);
    auto itr = begin();
    if (_begin == nullptr || comparer(*itr, newNode->Value()))
        Push(val);
    else
    {
        while (itr != end() && !comparer(*itr, newNode->Value()))
            itr++;

        if (itr == end() && !comparer(*itr, newNode->Value()))
            Enqueue(val);
        else
        {
            Node<T> *prev = itr()->Previous();
            *prev >> *newNode;
            *itr() << *newNode;
            _count++;
        }
    }
}

template <class T>
    requires Comparable<T>
inline void DoubleLinkedList<T>::Clear()
{
    if (_count == 0)
        return;
    auto itr = begin();
    while (itr != end())
    {
        Node<T> *prev = itr();
        itr++;
        delete prev;
    }
    delete _end;
    _begin = nullptr;
    _end = nullptr;
    _count = 0;
}

template <typename T>
    requires Comparable<T>
inline DoubleLinkedList<T>::~DoubleLinkedList()
{
    Clear();
}

template <typename T>
    requires Comparable<T>
inline DoubleLinkedList<T> DoubleLinkedList<T>::Take(std::function<bool(const T &)> predicate)
{
    auto list = DoubleLinkedList{};
    for (auto e : *this)
    {
        if (predicate(e))
        {
            list.Push(e);
        }
    }
    return list;
}

template <typename T> struct fmt::formatter<DoubleLinkedList<T>>
{
    std::string fmt = "";
    std::string innerfmt = "{}";
    constexpr auto parse(format_parse_context &ctx) -> format_parse_context::iterator
    {
        for (auto &c : ctx)
        {
            fmt += c;
        }
        fmt.erase(fmt.size() - 1);
        return ctx.end();
    }

    auto format(const DoubleLinkedList<T> &list, format_context &ctx) -> format_context::iterator
    {
        auto str = makeformat(innerfmt.c_str(), fmt);
        fmt::format_to(ctx.out(), "{}", str);
    }
};
