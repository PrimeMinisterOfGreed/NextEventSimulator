//
// Created by drfaust on 03/02/23.
//

#pragma once

#include <string>
#include "EventHandler.hpp"


template<typename T, int Moments>
    class Measure
    {
    private:
        T _sum[Moments];
        size_t _count;

        T get(int moment) const
        {
            static_assert(moment < Moments);

        }

    public:


    };