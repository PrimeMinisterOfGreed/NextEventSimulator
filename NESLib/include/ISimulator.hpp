#pragma once

class ISimulator
{
  public:
    virtual void Execute() = 0;
    virtual void Report() = 0;
};