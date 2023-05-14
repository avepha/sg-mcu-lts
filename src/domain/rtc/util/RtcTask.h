#include <TaskScheduler.h>
#include <RTClib.h>

#ifndef SG_MCU_RTCTASK_H
#define SG_MCU_RTCTASK_H

class RtcTask : public Task
{
public:
  RtcTask(RTC_DS1307 *_hwRtc, RTC_Millis *_swRtc) : Task(TASK_SECOND, TASK_FOREVER, &backgroundScheduler, true)
  {
    hwRtc = _hwRtc;
    swRtc = _swRtc;
    this->update();
  }

  bool Callback() override
  {
    this->update();
    return true;
  }

  DateTime getNow()
  {
    return now;
  }

  bool isHwRunning()
  {
    return hwRtcStatus;
  }

private:
  RTC_DS1307 *hwRtc;
  RTC_Millis *swRtc;
  DateTime now;
  bool hwRtcStatus = false;

  void update()
  {
    hwRtcStatus = hwRtc->isrunning();

    if (hwRtcStatus)
    {
      now = hwRtc->now();
      swRtc->adjust(now);
    }
    else
    {
      now = swRtc->now();
    }
  }
};

#endif // SG_MCU_RTCTASK_H
