#pragma once
#include "LinkedList.hpp"
#include "Event.hpp"
#include <memory>

template <typename T> using sptr = std::shared_ptr<T>;

using EventList = DoubleLinkedList<Event>;