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

        void operator-=(std::function<void(Args...)>& fnc)
        {
            for (int i = 0; i < _handlers.size(); i++)
            {
                void (*const* t1)(Args...) = _handlers.at(i).template target<void(*)(Args...)>();
                void (*const* t2)(Args...) = fnc.template target<void(*)(Args...)>();

                if (t1 && t2 && t1 == t2)
                    _handlers.erase(_handlers.begin() + i);
            }
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