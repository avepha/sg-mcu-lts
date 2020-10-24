#include "./station.h"
#include "./model.h"
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
    StationSchema schema = model.get();
    ModbusTask *modbusTask = ModbusTask::instance();
    for (int i = 0; i < schema.stationSize; i++) {
      StationSchema::Station _station = schema.stations[i];
      Station *station = new Station(_station.type, _station.address, _station.sensors, _station.sensorSize);
      if (!_station.enable) {
        station->setEnable(false);
      }
      // add station to station pool
      stations.push_back(station);
      // add station to modbus task
      modbusTask->registerStation(station);
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

StationCore *StationCore::s_instance = nullptr;
#endif
