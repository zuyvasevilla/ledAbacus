/* 
 * Project AMG8833 and MLX90640
 * Author: Zuyva Sevilla
 * Date: NOV 30 2023
 */
#include <Particle.h>
#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include <neopixel.h>

//Thermal
Adafruit_AMG88xx amg;
float thermalPixels[AMG88xx_PIXEL_ARRAY_SIZE];
int tempThreshold = 30;
int thermalPixelCounter, thermalPixelInt;
int thermalRow, thermalCol;
int thermalRed, thermalGreen, thermalBlue;

bool amgStatus;


// NeoPixels
int pixelCount = 128; 
int pixelIndex;
Adafruit_NeoPixel strip(pixelCount, SPI1, WS2812B);

int pixelArray[8][8] = {
    {0, 2, 4, 6, 8, 10, 12, 14},
    {30, 28, 26, 24, 22, 20, 18, 16},
    {32, 34, 36, 38, 40, 42, 44, 46},
    {62, 60, 58, 56, 54, 52, 50, 48},
    {64, 66, 68, 70, 72, 74, 76, 78},
    {94, 92, 90, 88, 86, 84, 82, 80},
    {96, 98, 100, 102, 104, 106, 108, 110},
    {126, 124, 122, 120, 118, 116, 114, 112}
};


SYSTEM_MODE(SEMI_AUTOMATIC);
//SYSTEM_THREAD(ENABLED);

void setup() {
    Serial.begin(9600);

    amgStatus = amg.begin();
    if (!amgStatus) {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
    }
    
    strip.begin();  // Init NeoPixels
    strip.setBrightness(255);
    strip.clear();
    strip.show();

    delay(1000); // let sensor boot up
}


void loop() { 

    amg.readPixels(thermalPixels);

    for( thermalPixelCounter = 0; thermalPixelCounter < AMG88xx_PIXEL_ARRAY_SIZE; thermalPixelCounter++){ //go through thermalPixel array
        thermalRow = thermalPixelCounter / 8; //figure out neopixel array position
        thermalCol = thermalPixelCounter % 8; 

        pixelIndex = pixelArray[thermalRow][thermalCol]; //designated with neopixel is being edited
        thermalPixelInt = thermalPixels[thermalPixelCounter]*1; //convert thermalPixel float to int
        
        thermalRed = map(thermalPixelInt,15,40,0,255);
        thermalBlue = map(thermalPixelInt,10,18,0,35);
        thermalGreen = map(thermalPixelInt,25,35,0,50);
        strip.setPixelColor(pixelIndex, strip.Color(0, thermalRed, 0)); 

      /*  //2 Color thermal map based on 1 temp threshold
        if(thermalPixels[thermalPixelCounter] > tempThreshold) {
            strip.setPixelColor(pixelIndex, strip.Color(255, 0, 0)); 
        } else {
            strip.setPixelColor(pixelIndex, strip.Color(0, 0, 255));
        }
         */

    }

    strip.show();

    Serial.print("[");
    for(int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++){
      Serial.print(thermalPixels[i-1]);
      Serial.print(", ");
      if(i % 8 == 0) Serial.println();
    }
    Serial.println("]");
    Serial.println();

    delay(200);


}
