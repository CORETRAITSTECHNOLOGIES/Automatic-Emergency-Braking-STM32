#include "Data_Config.h"
#include <unity.h>
void setUp(void) {}
void tearDown(void) {}
void test_collision() { TEST_ASSERT_EQUAL_INT32(true, detectCollision_Partial_brake(40,50)); }
int main() { UNITY_BEGIN(); RUN_TEST(test_collision); UNITY_END(); return 0; }
