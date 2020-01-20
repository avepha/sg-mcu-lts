#include <functional>
#include "./station.h"
#include "./model.h"
#include "./gsensor/GSensorStation.h"
#include "./solution/SolutionStation.h"
#include "./modbus/modbusTask.h"

#ifndef SG_MCU_STATION_CORE_H
#define SG_MCU_STATION_CORE_H

class StationCore {
public:
  StationCore() {
    StationModel model;
    StationSchema stationSchema = model.get();
    ModbusTask *modbusTask = ModbusTask::instance();
    for (int i = 0; i < stationSchema.gSensorStation.GsensorSize; i++) {
      auto *gSensorStation = new GSensorStation(stationSchema.gSensorStation.GSensorAddresses[i]);
      stations.push_back(gSensorStation);
      modbusTask->registerStation(gSensorStation);
    }

    for (int i = 0 ; i < stationSchema.solutionStation.SolutionSize; i++) {
      auto *solutionStation = new SolutionStation(stationSchema.solutionStation.SolutionAddresses[i]);
      stations.push_back(solutionStation);
      modbusTask->registerStation(solutionStation);
    }

    modbusTask->enable();
  };

  std::vector<Station *> getStations() {
    return stations;
  }

private:
  std::vector<Station *> stations{};
};

#endif
