#include <Task.h>
extern TaskManager taskManager;
  
class Sensor : public Task
{
public:
  static Sensor *s_instance;
  Sensor() : Task(MsToTaskTime(200))
  {
    sensor.temp = 0;
    sensor.humi = 0;
    sensor.soil = 0;
    sensor.par = 0;
    sensor.vpd = 0;
    sensor.co2 = 0;
    sensor.light = 0;
    sensor.soilTemp = 0;
    sensor.soilPoten = 0;
  };
  static Sensor *instance()
  {
    if (!s_instance)
      s_instance = new Sensor;
    return s_instance;
  }

  sensor_s GetSensors()
  {
    return sensor;
  }

  float GetSensor(int num)
  {
    switch (num)
    {
    case 0:
      return sensor.temp;
    case 1:
      return sensor.humi;
    case 2:
      return sensor.vpd;
    case 3:
      return sensor.soilTemp;
    case 4:
      return sensor.soil;
    case 5:
      return sensor.soilPoten;
    case 6:
      return sensor.co2;
    case 7:
      return sensor.light;
    case 8:
      return sensor.par;
    default:
      return -1;
    }
  }

private:
  sensor_s sensor;
  String cmdStr;
  String sensorStr = "0.00,0.00,0.0,0,0";
  char res[100];
  int size, cmdNumber, cmdSize;
  virtual bool OnStart()
  {
    return true;
  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    
  }

  String PrintSensor()
  {
    String str = String(sensor.temp) + " " + String(sensor.humi) + " " + String(sensor.par) + " " + String(sensor.vpd) + " " + String(sensor.soil) + " " + String(sensor.co2);
    return str;
  }

  void ExtractDataFloat(float *data, int size, String res)
  {
    int i = 0, si = 0, ei, j = 0;
    while (j < size)
    {
      int index = res.indexOf(",");
      String a = res.substring(0, index);
      data[j] = a.toFloat();
      si = index;
      res = res.substring(index + 1);
      j++;
    }
  }
};
Sensor *Sensor::s_instance = 0;
