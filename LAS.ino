#include "LAS.h"
using namespace LAS;

void testFunc(){
  Serial.println("test");
  scheduleIn(testFunc, 500);
}

void setup() {
  Serial.println("Serial test");
  scheduleAt(testFunc, true);
  runScheduler();
}

void loop() {
  
}
