#include <unity.h>
#include <mbed.h>
#include "Data_Config.h"
void test_1() { if(detectCollision_Partial_brake(57,50)) TEST_ASSERT_TRUE(true); }
int main() { UNITY_BEGIN(); RUN_TEST(test_1); UNITY_END(); return 0; }
