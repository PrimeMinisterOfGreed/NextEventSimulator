//
// Created by drfaust on 28/01/23.
//
#pragma once

#include <functional>

template<typename ...Args>
    class EventHandler
    {
    private:
        std::vector<std::function<void(Args...)>> _handlers;

    public:

        void operator+=(void(*fncPtr)(Args...))
        {
            _handlers.push_back(fncPtr);
        }

        void operator+=(std::function<void(Args...)> fnc)
        {
            _handlers.push_back(fnc);
        }

        void operator-=(void(*fncPtr)(Args...))
        {
            *this -= std::function<void(Args...)>(fncPtr);
        }

        void operator-=(std::function<void(Args...)> fnc)
        {
            std::remove_if(_handlers.begin(), _handlers.end(), [](auto a, auto b)
            { return a == b; });
        }


        void Invoke(Args... args)
        {
            for (auto handler: _handlers)
            {
                handler(args...);
            }
        }

        virtual ~EventHandler() = default;
    };