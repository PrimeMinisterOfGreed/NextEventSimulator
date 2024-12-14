/**
 * @file Event.hpp
 * @author matteo.ielacqua
 * @brief Header per la classe evento, descrive tutti i possibili eventi accolti nel sistema
 *
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "fmt/format.h"
#include <cstddef>
#include <iostream>

/**
 * @brief Tipi di evento supportati, per questo simulatore il tipo maintenance è ignorato
 * 
 */
enum EventType : char {
  NO_EVENT = 'N',
  ARRIVAL = 'A',
  DEPARTURE = 'D',
  END = 'E',
  PROBE = 'P',
  MAINTENANCE = 'M',
  RESET = 'R'
};

/**
 * @brief Classe evento, gestisce i suoi parametri di creazione e occorrenza
 * inoltre tiene traccia anche di quante copie esistono dello stesso evento.
 * Questa funzionalità è stata usata per il debugging, in particolare 
 * per osservare se la gestione nella lista era corretta senza ricorrere all'uso degli smart pointers
 */
struct Event {
  int Station = 0;
  int *copyCounter = nullptr;
  static int GeneratedNodes;
  static int DeletedNodes;
  double CreateTime = 0.0;
  double OccurTime = 0.0;
  double ServiceTime = 0.0;
  double ArrivalTime = 0.0;
  std::string Name{};
  char Type = EventType::NO_EVENT;
  char SubType = EventType::NO_EVENT;
  Event() { copyCounter = new int(); }
  Event(std::string name, char type, double createTime, double occurTime,
        double serviceTime, double arrivalTime, int stationTarget = 0)
      : Name{name}, Type{type}, CreateTime{createTime}, OccurTime{occurTime},
        ServiceTime{serviceTime}, ArrivalTime{arrivalTime},
        Station(stationTarget) {
    Event::GeneratedNodes++;
    copyCounter = new int();
  }
  ~Event() {
    if (*copyCounter > 0)
      (*copyCounter)--;
    else
      DeletedNodes++;
    Name.clear();
  }

  Event(const Event &evt)
      : Name(evt.Name), Type(evt.Type), CreateTime(evt.CreateTime),
        OccurTime(evt.OccurTime), ServiceTime(evt.ServiceTime),
        ArrivalTime(evt.ArrivalTime), Station(evt.Station),
        SubType(evt.SubType), copyCounter(evt.copyCounter) {
    (*copyCounter)++;
  }

  bool operator==(Event &oth);
};

/**
 * @brief formattatore per la libreria fmt 
 * 
 * @param evt 
 * @return std::string 
 */
static std::string format_as(const Event &evt) {
  return fmt::format("J:{},OC:{:2f},Tp:{},Station:{}", evt.Name, evt.OccurTime,
                     evt.Type, evt.Station);
}
