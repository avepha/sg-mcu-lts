#include "./station.h"
#include "./model.h"
#include "./modbus/modbusTask.h"

#ifndef SG_MCU_PSTATION_CORE_H
#define SG_MCU_PSTATION_CORE_H

class PStationCore {
public:
  static PStationCore *instance() {
    if (!s_instance)
      s_instance = new PStationCore;
    return s_instance;
  }

  std::vector<PStation *> getStations() {
    return stations;
  }

private:
  static PStationCore *s_instance;
  std::vector<PStation *> stations{};

  PStationCore() {
    PStationModel model;
    PStationSchema schema = model.get();
    PModbusTask *modbusTask = PModbusTask::instance();
    for (int i = 0; i < schema.stationSize; i++) {
      PStationSchema::Station _station = schema.stations[i];
      PStation *pstation = new PStation(_station.type, _station.address, _station.sensors, _station.sensorSize);
      if (!_station.enable) {
        pstation->setEnable(false);
      }
      // add station to station pool
      stations.push_back(pstation);
      // add station to modbus task
      modbusTask->registerStation(pstation);
      // create log on "station" topic
      Log::trace("station", "add gsensor addr: " + String(_station.address));
    }

#ifdef SG_COMMUNICATION_SIMPLEX // disable modbus when using lora
    Log::trace("station", "lora task started");
#else
    modbusTask->enable();
    Log::trace("station", "modbus task started!");
#endif
  };
};

PStationCore *PStationCore::s_instance = nullptr;
#endif
