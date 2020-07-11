#include "./station.h"
#include "./model.h"
#include "./gsensor/GSensorStation.h"
#include "./gsolution/GSolutionStation.h"
#include "./solution/SolutionStation.h"
#include "./modbus/modbusTask.h"

#ifndef SG_MCU_STATION_CORE_H
#define SG_MCU_STATION_CORE_H

class StationCore {
public:
  static StationCore *instance() {
    if (!s_instance)
      s_instance = new StationCore;
    return s_instance;
  }

  std::vector<Station *> getStations() {
    return stations;
  }

private:
  static StationCore *s_instance;
  std::vector<Station *> stations{};

  StationCore() {
    StationModel model;
    StationSchema stationSchema = model.get();
    ModbusTask *modbusTask = ModbusTask::instance();
    for (int i = 0; i < stationSchema.gSensorStation.size; i++) {
      auto *gSensorStation = new GSensorStation(stationSchema.gSensorStation.addresses[i]);
      stations.push_back(gSensorStation);
      modbusTask->registerStation(gSensorStation);
      Log::trace("station", "add gsensor addr: " + String(gSensorStation->getAddress()));
    }

    for (int i = 0; i < stationSchema.solutionStation.size; i++) {
      auto *solutionStation = new SolutionStation(stationSchema.solutionStation.addresses[i]);
      stations.push_back(solutionStation);
      modbusTask->registerStation(solutionStation);
      Log::trace("station", "add gsensor addr: " + String(solutionStation->getAddress()));
    }

    for (int i = 0; i < stationSchema.gSolutionStation.size; i++) {
      auto *gSolutionStation = new GSolutionStation(stationSchema.gSolutionStation.addresses[i]);
      stations.push_back(gSolutionStation);
      modbusTask->registerStation(gSolutionStation);
      Log::trace("station", "add gsolution addr: " + String(gSolutionStation->getAddress()));
    }

#ifdef SG_COMMUNICATION_SIMPLEX // disable modbus when using lora
    Log::trace("station", "lora task started");
#else
    modbusTask->enable();
    Log::trace("station", "modbus task started!");
#endif
  };
};

StationCore *StationCore::s_instance = nullptr;
#endif
