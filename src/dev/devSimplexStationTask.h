#include "../combineContext.h"
#ifndef SG_MCU_LTS_DEVSIMPLEXSTATIONTASK_H
#define SG_MCU_LTS_DEVSIMPLEXSTATIONTASK_H

class DevSimplexStationTask {
public:
  static DevSimplexStationTask *instance() {
    if (!s_instance)
      s_instance = new DevSimplexStationTask;
    return s_instance;
  }

  void runningTask(CombineContext *_context) {
    std::vector<Station *> stations = _context->station->core->getStations();
    std::vector<byte> vByte = GenerateModbusResponse::getGSolutionPacket(32);
    for (int i = 0; i < stations.size(); i++) {
      if (stations[i]->getAddress() == 32) {
        stations[i]->onPacketReceived(vByte);
        Log::trace("simplex", "got packet sta: " + String(32));
      }
    }
  }

private:
  static DevSimplexStationTask *s_instance;
  uint32_t timestamp = 0;

  DevSimplexStationTask() {}
};

DevSimplexStationTask *DevSimplexStationTask::s_instance = nullptr;
#endif //SG_MCU_LTS_DEVSIMPLEXSTATIONTASK_H
