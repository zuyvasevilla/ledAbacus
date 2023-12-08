/* 
 * Project Temperature Testing Program 
 * Author: Zuyva Sevilla
 * Date: NOV 27 2023
 */

#include "Particle.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_BME280.h"
#include "neopixel.h"
#include "IoTClassroom_CNM.h"
#include "Colors.h"

// OLED
int OLED_RESET = D4;
Adafruit_SSD1306 display(OLED_RESET);

// NeoPixels
int pixelCount = 128; // 8x8 matrix
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
int theDecimal;
int placeIndex, zeroIndex, pixel, pixelIndex, digitValue;
String theOctal;
String decimalToBaseX(int number, int base);

const int relayPin = 9;
bool relayBool;
//Temp
Adafruit_BME280 bme;
int BMEstatus;
int BMEhex = 0x76;
int tempC;
int tempK;
String tempK_8;
char pCent = 0x25;
char degree = 0xF8;
//Time
int previousMillis, currentMillis;
const int countingInterval = 500;

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
        
    BMEstatus = bme.begin(BMEhex);
 	  if (BMEstatus==false){
    	Serial.printf("BME280 at address 0x%02X failed to start", BMEhex);
 		}

    strip.begin();  // Init NeoPixels
    strip.setBrightness(255);
    strip.clear();
    strip.show();
    pinMode(relayPin, OUTPUT);
    relayBool = 0;
    
    previousMillis = millis();
    theDecimal = random(36985,97179);

    Serial.printf("READY\n");
}

void loop() {
    currentMillis = millis();
    strip.clear();
    digitalWrite(relayPin, relayBool);

    tempC = bme.readTemperature();
  	tempK = tempC+273;
    theOctal = decimalToBaseX(tempK, 8);

        //theOctal = decimalToBaseX(theDecimal, 8);


        for (placeIndex= 0; placeIndex < theOctal.length(); placeIndex++) {
             digitValue = theOctal.charAt(theOctal.length() - 1 - placeIndex) - '0'; 
                //grab the numberal from each place of the octal string, working backwards from least significant digit in, 
                //then convert to integer by subtraction ASCII value

            for (pixel = 0; pixel <= digitValue; pixel++) {
                pixelIndex = pixelArray[placeIndex][pixel];
                strip.setPixelColor(pixelIndex, 0,0,255); //the appropriate number of pixels ON in each place column 
            }

        }

        for(zeroIndex=0;zeroIndex<8;zeroIndex++){ // set the zero-th pixel a different color
        strip.setPixelColor(pixelArray[zeroIndex][0], 70,20,200); 
        }
        strip.show();


        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.printf("Kelvin Decimal:\n%i\nKelvin Octal:\n%s", tempK, theOctal.c_str());
        display.display();


}

String decimalToBaseX(int number, int base){
    String result;
    String remainder;

    if (number == 0) {
        result = "0";
    }

    while(number > 0) {
        remainder = String(number % base); // Convert remainder to String
        result = remainder + result;
        number = number/base;
    }

    return result;
}
