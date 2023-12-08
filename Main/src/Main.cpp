/* 
 * Project The Counter
 * Author: Zuyva Sevilla
 * Date: DEC  4 2023
 */

#include "Particle.h"
#include <Wire.h>
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "credentials.h"
#include "Adafruit_AMG88xx.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_BME280.h"
#include "neopixel.h"
#include "Encoder.h"
#include "IoTClassroom_CNM.h"
#include "Colors.h"

/************ Global State (you don't need to change this!) ***   ***************/ 
TCPClient TheClient; 

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

/****************************** Feeds ***************************************/ 
// Setup Feeds to publish or subscribe 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
//Adafruit_MQTT_Subscribe buttonFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/buttononoff"); 
//Adafruit_MQTT_Subscribe ledSliderFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ledslider"); 

Adafruit_MQTT_Publish decimalFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/counterdecimal");
Adafruit_MQTT_Publish octalFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/counteroctal");
Adafruit_MQTT_Publish tempFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/countertemp");

void MQTT_connect();
bool MQTT_ping();

//Counting 
String decimalToBaseX(int number, int base);
void bubbleVisualizer(String nonDecimal, int r, int g, int b);
static uint32_t theDecimal;
String theOctal;
const int switchLED = 2;
bool switchLEDstate;

//Time
int currentMillis, previousMillis, countingMillis, MQTTmillis;
const int countingInterval = 500; ////////////////////////////////////////

//Neopixels
int bubbleCount = 128; 
Adafruit_NeoPixel bubbles(bubbleCount, SPI1, WS2811);
int bubbleArray[8][8] = {
{0, 2, 4, 6, 8, 10, 12, 14},
    {30, 28, 26, 24, 22, 20, 18, 16},
    {32, 34, 36, 38, 40, 42, 44, 46},
    {62, 60, 58, 56, 54, 52, 50, 48},
    {64, 66, 68, 70, 72, 74, 76, 78},
    {94, 92, 90, 88, 86, 84, 82, 80},
    {96, 98, 100, 102, 104, 106, 108, 110},
    {126, 124, 122, 120, 118, 116, 114, 112}
};

unsigned int placeIndex, zeroIndex, bubble, bubbleIndex, digitValue;


// OLED
int OLED_RESET = D4;
Adafruit_SSD1306 display(OLED_RESET);
//Temp
Adafruit_BME280 bme;
int BMEstatus;
int BMEhex = 0x76;
int tempC;
int tempK;
String tempK_8;
char pCent = 0x25;
char degree = 0xF8;
//Encoder
Encoder controlEnc(D9,D8);
Button encButton(D19);
const int encRed(D12);
const int encGreen(D13);
const int encBlue(D14);
bool encClick;
//Thermal
Adafruit_AMG88xx amg;
float thermalPixels[AMG88xx_PIXEL_ARRAY_SIZE];
int tempThreshold = 30;
int thermalPixelCounter, thermalPixelInt;
int thermalRow, thermalCol;
int thermalRed, thermalGreen, thermalBlue;
void thermalVisualizer();

bool amgStatus;

static int activeMode;
int modeMillis;
int modeInterval = 30000;



SYSTEM_MODE(AUTOMATIC);
//SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler(LOG_LEVEL_INFO);

void setup() {
    Serial.begin(9600);        
    waitFor(Serial.isConnected, 5000);
    Serial.printf("SERIAL READY\n");  

    WiFi.on();
  	WiFi.clearCredentials();
  	WiFi.setCredentials("DDCIOT","ddcIOT2020");
  	WiFi.connect();
  	while(WiFi.connecting()){
    	Serial.printf("."); 
  	}
  	delay(1000);
  	Serial.printf("\n\n"); 

    amgStatus = amg.begin();
    if (!amgStatus) {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
    }
    
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.display();
    display.setRotation(0);
    delay(500);
    display.clearDisplay();
    
    BMEstatus = bme.begin(BMEhex);
 	  if (BMEstatus==false){
    	Serial.printf("BME280 at address 0x%02X failed to start", BMEhex);
 		}

    bubbles.begin();  // Init NeoPixels
    bubbles.setBrightness(255);
    bubbles.clear();
    bubbles.show();

    pinMode(switchLED, OUTPUT);
    switchLEDstate = HIGH;

    pinMode(encRed, OUTPUT);
    pinMode(encGreen, OUTPUT);
    pinMode(encBlue, OUTPUT);

    countingMillis = millis();
    modeMillis = millis();
    theDecimal = 0;
    activeMode = 0;

    Serial.printf("READY\n");

}

void loop() {
    MQTT_connect();
    MQTT_ping();
    
    if((millis()-MQTTmillis > 6000)) {
    if(mqtt.Update()) {
      decimalFeed.publish(theDecimal);
      octalFeed.publish(theOctal);
      tempFeed.publish(tempK_8);
      } 
    MQTTmillis = millis();
  }

    //bubbles.clear();
    currentMillis = millis();

    tempC = bme.readTemperature();
  	tempK = tempC+273;
    tempK_8 = decimalToBaseX(tempK, 8);

    amg.readPixels(thermalPixels);


    if(currentMillis - countingMillis >= countingInterval){
        countingMillis = millis();
        theDecimal++;
    }
    if(theDecimal > 16777215){  //77777777 in decimal
        theDecimal = 0; 
    }
    theOctal = decimalToBaseX(theDecimal, 8);

    if(currentMillis - modeMillis > modeInterval){
        modeMillis = millis();
        activeMode++;
    }

    if(activeMode>3){
        activeMode = 0; 
    }
    switch(activeMode){
        case 0:
            bubbles.clear();
            for (placeIndex= 0; placeIndex < theOctal.length(); placeIndex++) { //work through each chracter in string 
                digitValue = theOctal.charAt(theOctal.length() - 1 - placeIndex) - '0'; 
                    //grab the numberal from each place of the octal string, working backwards from least significant digit in, 
                    //then convert to proper integer by subtraction of 0 ASCII value (48)

                for (bubble = 0; bubble <= digitValue; bubble++) {
                    bubbleIndex = bubbleArray[placeIndex][bubble];
                    bubbles.setPixelColor(bubbleIndex, 0,255,0); //the appropriate number of pixels ON in each place column 
                }

            }   

            for(zeroIndex=0;zeroIndex<8;zeroIndex++){ // set the zero-th pixel a different color
                bubbles.setPixelColor(bubbleArray[zeroIndex][0], 70, 200, 0); 
            }
            bubbles.show();
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(WHITE);
            display.setCursor(0,0);
            display.printf("Decimal: %i\nOctal: %s\nMode: %i", theDecimal, theOctal.c_str(),activeMode);
            display.display();
            break;
        case 1:
            bubbleVisualizer(tempK_8,10,140,220);
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(WHITE);
            display.setCursor(0,0);
            display.printf("Temp C: %i\n Temp K: %i\nMode: %i", tempC, tempK,activeMode);
            display.display();
            break;
        case 2:
            thermalVisualizer();
            break;
    }


/*     display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.printf("Decimal: %i\nOctal: %s\nMode: %i", theDecimal, theOctal.c_str(),activeMode);
    display.display(); */


}

String decimalToBaseX(int number, int base){
    String result;
    String remainder;

    if (number == 0) {               //shout-out to 0 for being the only exception
        result = "0";
    }

    while(number > 0) {              //Keep cycling through until quotient reaches 0
        remainder = number % base;   // divide decimal by base number, keep remainder
        result = remainder + result; //combine remainders together in one string
        number = number / base;      //divide decimal by base number, keep quotient
    }

    return result;
}
void bubbleVisualizer(String nonDecimal, int r, int g, int b){
     
     
    for (placeIndex= 0; placeIndex < nonDecimal.length(); placeIndex++) { //work through each chracter in string 
        digitValue = nonDecimal.charAt(nonDecimal.length() - 1 - placeIndex) - '0'; 
            //grab the numberal from each place of the octal string, working backwards from least significant digit in, 
            //then convert to proper integer by subtraction of 0 ASCII value (48)

        for (bubble = 0; bubble <= digitValue; bubble++) {
             bubbleIndex = bubbleArray[placeIndex][bubble];
            bubbles.setPixelColor(bubbleIndex, r, g, b); //the appropriate number of pixels ON in each place column 
        }

    }

    for(zeroIndex=0;zeroIndex<8;zeroIndex++){ // set the zero-th pixel a different color
        bubbles.setPixelColor(bubbleArray[zeroIndex][0], 70, 200, 0); 
    }
    bubbles.show();
}

void thermalVisualizer(){


    for(thermalPixelCounter = 0; thermalPixelCounter < AMG88xx_PIXEL_ARRAY_SIZE; thermalPixelCounter++){ //go through thermalPixel array
        thermalRow = thermalPixelCounter / 8; //figure out neopixel array position
        thermalCol = thermalPixelCounter % 8; 

        bubbleIndex = bubbleArray[thermalRow][thermalCol]; //designated with neopixel is being edited
        thermalPixelInt = thermalPixels[thermalPixelCounter]*1; //convert thermalPixel float to int
        
        thermalRed = map(thermalPixelInt,13,35,0,255);
        thermalBlue = map(thermalPixelInt,10,18,0,35);
        thermalGreen = map(thermalPixelInt,25,35,0,50);
        bubbles.setPixelColor(bubbleIndex, bubbles.Color(0, thermalRed, 0)); 
        
    }

    bubbles.show();

    Serial.print("[");    //not in style guide, just from examples
    for(int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++){
      Serial.print(thermalPixels[i-1]);
      Serial.print(", ");
      if(i % 8 == 0) Serial.println();
    }
    Serial.println("]");
    Serial.println();

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
 
  // Return if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
}

bool MQTT_ping() {
  static unsigned int last;
  bool pingStatus;

  if ((millis()-last)>120000) {
      Serial.printf("Pinging MQTT \n");
      pingStatus = mqtt.ping();
      if(!pingStatus) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
  return pingStatus;
}