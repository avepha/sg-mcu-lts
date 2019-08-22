#include "../init.h"

void util_IsoStringToDateTime() {
  DateTime dt = IsoStringToDateTime("2019-08-10T05:01:02");
  TEST_ASSERT_EQUAL( dt.year(), 2019);
  TEST_ASSERT_EQUAL( dt.month(), 8);
  TEST_ASSERT_EQUAL( dt.day(), 10);
  TEST_ASSERT_EQUAL( dt.hour(), 5);
  TEST_ASSERT_EQUAL( dt.minute(), 1);
  TEST_ASSERT_EQUAL( dt.second(), 02);
}

void util_date_time_RUN_TEST() {
  RUN_TEST(util_IsoStringToDateTime);
}