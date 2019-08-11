#include "mutation/m_date_save.h"

#include "query/q_date.h"
#include "query/q_sensor.h"


#include "util/date_time.h"


void setup() {
  delay(1000);

  UNITY_BEGIN();

  /* Query Test */
  q_date_RUN_TEST();
  q_sensor_RUN_TEST();

  /* Mutation Test */
  m_date_save_RUN_TEST();

  /* Util Test */
  util_date_time_RUN_TEST();

  UNITY_END();
}

void loop() {

}