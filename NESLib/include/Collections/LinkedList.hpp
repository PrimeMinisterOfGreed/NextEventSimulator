#pragma once

#include <concepts>
#include <cstddef>
#include <exception>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include "Node.hpp"



template <class T> requires std::equality_comparable<T> class DoubleLinkedList {
private:
  Node<T> *_begin;
  Node<T> *_end;
  size_t _count = 0;

public:
  NodeIterator<T> begin() const { return NodeIterator(_begin); }

  NodeIterator<T> end() const { return NodeIterator(_end); }

  inline size_t Count() const { return _count; }

  void Push(T val);

  T &Pull();

  void Enqueue(T val);


  T &Dequeue();

  void Insert(T val, int index);


  void Insert(T val, std::function<bool(const T &, const T &)> comparer);


  void Clear();

  int IndexOf(T val);

  bool Contains(T val);

  std::string ToString();
};

template <class T> bool DoubleLinkedList<T>::Contains(T *val) {
  return IndexOf(val) != -1;
}

template <class T> int DoubleLinkedList<T>::IndexOf(T *val) {
  auto itr = begin();
  int p = 0;
  while (itr != end()) {
    if (itr() == val)
      return p;
    itr++;
  }
  return -1;
}

template <class T> void DoubleLinkedList<T>::Push(T *val) {
  Node<T> *new_node = new Node<T>(val);
  if (_begin != nullptr)
    *_begin << *new_node;
  if (_end == nullptr)
    _end = new_node;
  _begin = new_node;
  _count++;
}

template <class T> T &DoubleLinkedList<T>::Pull() {
  if (_begin == nullptr)
    throw new std::invalid_argument(
        "Trying to pull from the list but is empty");
  Node<T> *res = _begin;
  if (_begin != _end) {
    _begin = _begin->Next();
    if (_begin != nullptr)
      _begin->_previous = nullptr;
  } else {
    _begin = nullptr;
    _end = nullptr;
  }
  auto val = res->Value();
  _count--;
  res->_next = nullptr;
  res->_previous = nullptr;
  res->_val = nullptr;
  return val;
}

template <class T> inline void DoubleLinkedList<T>::Enqueue(T *val) {
  Node<T> *newNode = new Node<T>(val);
  if (_begin == nullptr) {
    _begin = newNode;
    _end = newNode;
  } else {
    auto ptr = _end;
    *ptr >> *newNode;
    *newNode << *ptr;
    _end = newNode;
  }
  _count++;
}

template <class T> inline T &DoubleLinkedList<T>::Dequeue() { return Pull(); }

template <class T> void DoubleLinkedList<T>::Insert(T *val, int index) {
  if (index > _count)
    throw new std::invalid_argument("Index out of bounds");
  if (index == 0)
    Push(val);
  else if (index == _count)
    Enqueue(val);
  else {
    Node<T> &newNode = *new Node<T>(val);
    auto itr = (begin() + index);
    auto ptr = itr();
    Node<T> &prev = (itr--)();
    prev >> newNode;
    ptr << newNode;
    _count++;
  }
}

template <class T>
void DoubleLinkedList<T>::Insert(
    T *val, std::function<bool(const T &, const T &)> comparer) {
  Node<T> &newNode = *new Node<T>(val);
  auto itr = begin();
  if (_begin == nullptr || comparer(*itr, *newNode))
    Push(val);
  else {
    while (itr != end() && !comparer(*itr, *newNode))
      itr++;

    if (itr == end() && !comparer(*itr, *newNode))
      Enqueue(val);
    else {
      Node<T> &prev = *itr().Previous();
      prev >> newNode;
      itr() << newNode;
      _count++;
    }
  }
}

template <class T> inline void DoubleLinkedList<T>::Clear() {
  if (_count == 0)
    return;
  auto itr = begin();
  while (itr != end()) {
    auto prev = &itr();
    itr++;
    if (prev->Value() != nullptr) {
      delete prev;
    }
  }
  _begin = nullptr;
  _end = nullptr;
  _count = 0;
}
