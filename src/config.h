#ifndef HOUR_IN_MINUTE
#define HOUR_IN_MINUTE 60
#endif

#ifndef SG_MCU_CONFIG_H
#define SG_MCU_CONFIG_H

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
#ifdef SG_MCU_V2
const int SG_MPU_RX = 19;
const int SG_MPU_TX = 18;
const String SG_MODEL = "v2";
#endif

#ifdef SG_MCU_V1
const int SG_MPU_RX = 18;
const int SG_MPU_TX = 19;
const String SG_MODEL = "v1";
#endif

const int SG_SENSOR_RX = 16;
const int SG_SENSOR_TX = 17;
const int SG_DIR_PIN = 4;

const unsigned long EXPECTED_ACTIVE_DIFF_TIME = 10 * 1000; // 10 seconds

#endif
