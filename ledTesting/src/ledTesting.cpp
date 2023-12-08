/* 
 * Project Abacus Testing Program 
 * Author: Zuyva Sevilla
 * Date: DEC 1 2023
 */

#include "Particle.h"
#include "neopixel.h"
#include "IoTClassroom_CNM.h"
#include "Colors.h"

//NeoPixels
int pixelCount=70;
int i;

int pixelArray[5][5]={
  {0,1,2,3,4},
  {9,8,7,6,5},
  {10,11,12,13,14},
  {19,18,17,16,15},
  {20,21,22,23,24}
};

Adafruit_NeoPixel strip(pixelCount, SPI1, WS2812);
void pixelFill(int startPixel, int endPixel, int fillColor);

int pixelCounter, hueCounter, HSV2RGB;

SYSTEM_MODE(SEMI_AUTOMATIC);
//SYSTEM_THREAD(ENABLED);


void setup() {
    Serial.begin(9600);		
	waitFor(Serial.isConnected, 5000);
  	Serial.printf("SERIAL READY\n");  

    strip.begin();  //Init Neopixels
	strip.setBrightness(10);
    strip.clear();
	strip.show();

    Serial.printf("READY\n");

    hueCounter=0;

}

void loop() {
    
    pixelFill(0, pixelCount, orange);
    delay(1000);
    pixelFill(0, pixelCount, red);
    delay(1000);
    pixelFill(0, pixelCount, pink);
    delay(1000);
    pixelFill(0, pixelCount, white);

    delay(1000);

}


//Neopixel
void pixelFill(int startPixel, int endPixel, int fillColor){
 	int i;
	for(i=startPixel; i<=endPixel; i++){
    	strip.setPixelColor(i, fillColor);
  	} 
  	strip.show();
}