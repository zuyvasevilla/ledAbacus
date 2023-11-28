/* 
 * Project Abacus Testing Program 
 * Author: Zuyva Sevilla
 * Date: NOV 27 2023
 */

#include "Particle.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_BME280.h"
#include "neopixel.h"
#include "IoTClassroom_CNM.h"
#include "Colors.h"

//OLED
int OLED_RESET=D4;
Adafruit_SSD1306 display(OLED_RESET);

//NeoPixels
int pixelCount=256;
int i;
int pixelArray[5][5]={
  {0,1,2,3,4},
  {9,8,7,6,5},
  {10,11,12,13,14},
  {19,18,17,16,15},
  {20,21,22,23,24}
};
Adafruit_NeoPixel strip(pixelCount, SPI1, WS2812B);

int theNumber;
int theDecimal;
String theQuinary;
String baseTest, base1, binary, base9, base11, base16;
String decimalToBase5(int number);
String decimalToBinary(int number);
String decimalToBaseX(int number, int base);
String decimalToBaseXBackwards(int number, int base);

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

void setup() {
    Serial.begin(9600);		
	waitFor(Serial.isConnected, 5000);
  	Serial.printf("SERIAL READY\n");  
	
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 	display.display();
	display.setRotation(0);
 	delay(500);
  	display.clearDisplay();

    strip.begin();  //Init Neopixels
	strip.setBrightness(1);
    strip.clear();
	strip.show();
}

void loop() {
    
    for (theNumber=0;theNumber<300;theNumber++){
    
        baseTest = decimalToBase5(theNumber);
        //base1 = decimalToBaseX(theNumber, 1); //Trying to do this broke everything, I think it might go against math
        binary = decimalToBinary(theNumber);
        base9 = decimalToBaseX(theNumber, 9);
        base11 = decimalToBaseX(theNumber, 11);
        base16 = decimalToBaseX(theNumber, 16);
        
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.printf("Decimal:%i\n\nBinary:%s\nQuinary(Base 5):%s\nBase 9:%s\nBase 11:%s\nHexadecimal:%s",theNumber,binary.c_str(), baseTest.c_str(), base9.c_str(),base11.c_str(),base16.c_str());
        display.display();

        Serial.printf("Decimal: %i\nBase Conversion:\n%s",theNumber, baseTest.c_str());
        
        delay(1000);
    }

/* 	for(i=0; i<=5; i++){
    	strip.setPixelColor(pixelArray[i][i], red);
        strip.show();
        delay(500);

  	}  */

}

String decimalToBase5(int number){
    String result;
    String remainder;

    if (number == 0){
        result = "0";
    }

    while(number > 0){
        remainder = number % 5;
        result = remainder + result;
        number = number / 5;
    }

    return result;
}

String decimalToBinary(int number){
    String result;
    String remainder;

    if (number == 0){
        result = "0";
    }

    while(number > 0){
        remainder = number % 2;
        result = remainder + result;
        number = number / 2;
    }

    return result;
}

String decimalToBaseX(int number, int base){
    String result;
    String remainder;

    if (number == 0){
        result = "0";
    }

    while(number > 0){
        remainder = number % base;

        result = remainder + result;
        number = number / base;
    }

    return result;
}

String decimalToBaseXBackwards(int number, int base){
    String result;
    String remainder;

    if (number == 0){
        result = "0";
    }

    while(number > 0){
        remainder = number % base;

        result = result + remainder;
        number = number / base;
    }

    return result;
}


