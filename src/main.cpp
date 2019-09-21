#include "header.h"

int CH_ON = HIGH;
int CH_OFF = LOW;
String MCU_STATE = "NRDY";

struct timer_s
{
  uint16_t st;
  uint16_t en; // end or working depend CH_ON timer mode
};

struct sensor_s
{
  float soil;
  float vpd;
  float ec;
  float ph;
  float water;
  float temp;
  float humi;
  float co2;
  float light;
  float soilTemp;
  float soilPoten;
  boolean floating;
  float par;
};

#include "modules/Control/Factory/Control.h"
Control *channel[CHANNEL_NUMBER];
//
int ChannelGpio[6] = {32, 33, 25, 26};
int ChannelStatus[6] = {LOW, LOW, LOW, LOW};

void DigitalWrite(int ch, int status)
{
  digitalWrite(ChannelGpio[ch], status);
  ChannelStatus[ch] = status;
}

String BoardInfo()
{
  String str = "INFOBOARD-VERSION" + String(VERSION) + "\r\n";
  str += "INFOPROJECT-NAME " + String(PROJECT) + "\r\n";
  str += "INFODATE-" + String(UPLOADDATE) + "\r\n";
  return str;
}

//General module
#include "./modules/Memory/eeprom_manager.h"
// #include "./modules/DateTime.h"
#include "./modules/RTC.h"

#include "./modules/Sensors/Sensor.h"
#include "./modules/Sensors/ParAcc.h"
#include "modules/Control/Factory/ControlFactory.h"
#include "./modules/ChannelHandler.h"
#include "./modules/Memory/MemoryCheck.h"

#include "./modules/Communication.h"

#include "./modules/Helper/Puppet.h"
#include "./modules/Button/ResetWifi.h"

#include "./modules/Helper/pingMessage.h"

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  mpuCom.begin(115200, SERIAL_8N1, 18, 19);
  sensorCom.begin(9600, SERIAL_8N1, 16, 17);

  EEPROM_Manager::InitEEPROM();

  taskManager.StartTask(RTC::instance());
  taskManager.StartTask(Sensor::instance());
  taskManager.StartTask(ParAcc::instance());
  taskManager.StartTask(Communication::instance());

  ChannelHanler::instance();

  MCU_STATE = "RDY";
  mpuCom.println(MCU_STATE);
}

void loop()
{
  taskManager.Loop();
}