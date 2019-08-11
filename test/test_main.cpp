#include "mutation/m_date_save.h"
#include "query/q_date.h"
#include "util/date_time.h"


void setup() {
  delay(1000);

  UNITY_BEGIN();
  q_date_RUN_TEST();
  m_date_save_RUN_TEST();
  util_date_time_RUN_TEST();
  UNITY_END();
}

void loop() {

}