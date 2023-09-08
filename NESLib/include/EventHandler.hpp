//
// Created by drfaust on 28/01/23.
//
#pragma once

#include <cstdint>
#include <functional>

template <typename... Args> class EventHandler;

template <typename... Args> struct Slot
{
    using EvtHandler = EventHandler<Args...>;
    std::function<void(Args...)> _fncPtr;
    EvtHandler *_handler;
    int *slotPtr;
    Slot(EvtHandler *handler, std::function<void(Args...)> fncptr)
        : _handler(handler), _fncPtr(fncptr), slotPtr((int *)&_fncPtr)
    {
    }
    ~Slot()
    {
    }
};

/**
 * EventHandler in C# style, use FunctionHandler to implement lambda type handlers
 * @tparam Args
 * @example with lambda EventHandler handler += new FunctionHandler<>([](){});
 */
template <typename... Args> class EventHandler
{
  private:
    std::vector<Slot<Args...>> _handlers;

  public:
    void Invoke(Args... args)
    {
        for (auto handler : _handlers)
        {
            handler._fncPtr(args...);
        }
    }

    Slot<Args...> Attach(std::function<void(Args...)> function)
    {
        auto slot = Slot<Args...>(this, function);
        _handlers.push_back(slot);
        return std::move(slot);
    }

    void Detach(Slot<Args...> slot)
    {
        for (int i = 0; i < _handlers.size(); i++)
        {
            if (_handlers[i].slotPtr == slot.slotPtr)
            {
                _handlers.erase(_handlers.begin() + i);
            }
        }
    }

    virtual ~EventHandler()
    {
        _handlers.clear();
    }
};