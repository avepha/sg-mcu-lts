#include "mutation/m_date_save.h"
#include "mutation/m_sensor_order_save.h"

#include "query/q_date.h"
#include "query/q_sensor.h"
#include "query/q_sensor_order.h"
#include "query/q_precondition.h"


#include "util/date_time.h"


void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  delay(1000);

  UNITY_BEGIN();

  /* Query Test */
  q_date_RUN_TEST();
  q_sensor_RUN_TEST();
  q_sensor_order_RUN_TEST();
  q_precondition_RUN_TEST();

  /* Mutation Test */
  m_date_save_RUN_TEST();
  m_sensor_order_save_RUN_TEST();
  /* Util Test */
  util_date_time_RUN_TEST();

  UNITY_END();
}

void loop() {

}