/* 
 * Project Stringy
 * Author: Brian Rashap
 * Date: 29-NOV-2023
 */

#include "Particle.h"
SYSTEM_MODE(SEMI_AUTOMATIC);

int newNumber;
String printThis;

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);
  printThis = "";
}

void loop() {
  newNumber = random(16);
  printThis += String(newNumber,HEX);
  Serial.printf("The number string is %s\n",printThis.c_str());
  delay(1000);
}
