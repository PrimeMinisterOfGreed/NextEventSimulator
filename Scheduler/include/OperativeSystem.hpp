#pragma once
#include "ISimulator.hpp"

class OS : public ISimulator
{
  public:
    void Execute() override;
    void Report() override;
};