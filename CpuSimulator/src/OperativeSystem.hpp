/**
 * @file OperativeSystem.hpp
 * @author matteo.ielacqua
 * @brief classe che implementa lo scheduler per gli eventi del progetto
 * costruisce e memorizza anche le varie stazioni che compongono il sistema da simulare
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "Event.hpp"
#include "ISimulator.hpp"
#include "Scheduler.hpp"

class OS : public ISimulator, public Scheduler
{
  private:
    bool _end = false;

  protected:
    void ProcessProbe(Event &evt) override;

  public:
    void Execute() override;
    void Reset() override;
    OS();
    void Initialize() override;

    virtual void ProcessArrival(Event &evt) override;
};