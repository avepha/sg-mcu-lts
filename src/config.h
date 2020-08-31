#ifndef HOUR_IN_MINUTE
#define HOUR_IN_MINUTE 60
#endif

#ifndef SG_MCU_CONFIG_H
#define SG_MCU_CONFIG_H

const unsigned int serialBaudRate = 115200;
const unsigned int rpiBaudRate = 57600;

// TODO: pin 2 is reserved pin for activating uploading mode
const int CHANNEL_GPIO_MAP[8] = {
    32, //ch 1
    33, //ch 2
    25, //ch 3
    26, //ch 4
    27, //ch 5
    13, //ch 6
    24, //ch 7 should be pin 2
    23, //ch 8
};

//const int SG_MPU_RX = 18;
//const int SG_MPU_TX = 19;
#ifdef SG_VERSION_V2
const int SG_MPU_RX = 19;
const int SG_MPU_TX = 18;

#ifdef SG_COMMUNICATION_SIMPLEX
const String SG_MODEL = "v2_lora";
#else
const String SG_MODEL = "v2";
#endif

#endif

#ifdef SG_VERSION_V1
const int SG_MPU_RX = 18;
const int SG_MPU_TX = 19;
const String SG_MODEL = "v1";
#endif


const int SG_STATION_RX = 16;
const int SG_STATION_TX = 17;
const int RS485_DIR_PIN = 4;
const int RS485_SEND_MODE = HIGH;
const int RS485_RECV_MODE = LOW;

const unsigned long EXPECTED_ACTIVE_DIFF_TIME = 10 * 1000; // 10 seconds

#endif

//1 4 0 0 C8 41 0 0 48 42 32 F1 C5 44 0 0 CC 41 0 0 72 42 0 80 BB 44 0 0 D2 42 0 0 C8 42 89 33
