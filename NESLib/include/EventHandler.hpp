//
// Created by drfaust on 28/01/23.
//
#pragma once

#include <cstdint>
#include <functional>
#include <vector>

template <typename... Args> struct EventHandler;

template <typename... Args> struct Slot
{
    friend class EventHandler<Args...>;

  private:
  public:
    ~Slot()
    {
    }
};

template <typename... Args> struct EventHandler
{
};
