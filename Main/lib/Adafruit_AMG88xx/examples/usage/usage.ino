// Example usage for Adafruit_AMG88xx library by Dean Miller.

#include "Adafruit_AMG88xx.h"

// Initialize objects from the lib
Adafruit_AMG88xx adafruit_AMG88xx;

void setup() {
    // Call functions on initialized library objects that require hardware
    adafruit_AMG88xx.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    adafruit_AMG88xx.process();
}
