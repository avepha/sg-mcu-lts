#ifndef HOUR_IN_MINUTE
#define HOUR_IN_MINUTE 60
#endif

#ifndef SG_MCU_CONFIG_H
#define SG_MCU_CONFIG_H

const int CHANNEL_GPIO_MAP[8] = {
    22, //ch 1
    23, //ch 2
    24, //ch 3
    25, //ch 4
    26, //ch 5
    27, //ch 6
    28, //ch 7
    29, //ch 8
};

//const int SG_MPU_RX = 18;
//const int SG_MPU_TX = 19;

const int SG_MPU_RX = 19;
const int SG_MPU_TX = 18;

const int SG_SENSOR_RX = 16;
const int SG_SENSOR_TX = 17;

const unsigned long EXPECTED_ACTIVE_DIFF_TIME = 10 * 1000; // 10 seconds

#endif
