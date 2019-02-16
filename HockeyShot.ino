/*
 *  Project: Hockey Shot Electronic Net Target
 *  Author : Tony Fountaine
 *  Year   : 2019
 *  License: MIT  
 *  Desc   : My son was watching the NHL Skills competition and wanted 
 *           the light up targets used in the sharp shooter competition
 *           I wanted to buy some for him but the price was ridiculous.
 *           I won't name the company but the price was in the thousands.
 *           
 *           The design is really simple.  Just an accelerometer and an
 *           STM32 Blue Pill with Arduino firmware.  There is a WS2812
 *           LED strip that allows the target to change colors when hit.
 *           The current design uses an MPU6050 3Axis Accel and 3 Axis
 *           Gyro. I use the 3Axis accel readings to compute the mag.
 *           This is currently saturating so I'm planning on adding
 *           a version with the ST H3LIS331DL 400g. The original design was  
 *           chosen to create the cheapest possible version.  The blue
 *           pill is about $2-3 dollars and the MPU6050 is about $2.
 *           The LED Strips cost about $30 for 5meters maybe less. I cut
 *           up the strips and soldered them to the back of a 3/8" HDPE
 *           puck board.  The HDPE board is white and does a nice job
 *           of diffusing the LED lights.  You could go cheap and just use
 *           a single LED.....or something.  All in all you can make 4 targets
 *           for less than $100.  They work great and are very durable.  I
 *           believe HDPE is the same material that hockey rinks are made of.
 *           
 *           This project is kind of interesting in the sense that some
 *           young hockey players may take an interest in electronics and
 *           coding in an attempt to create something fun for themselves.
 *           That's my hope anyway.  There may be some old guys who want 
 *           to give it a shot as well.  It's a cool little project that
 *           I think anyone could put together.  You need a soldering iron
 *           to put the electronics together and a saw to cut the HDPE.   
 *           I actually repurposed the old foam hockey target as the casing
 *           for the HDPE.  You can buy them for about $15-$20.  I used some
 *           L brackets to hold the HDPE in.  Just drilled a hole in the side
 *           and screwed it in.
 *           
 *           The targets will currently work independently.  I am working 
 *           on a Raspberry Pi 3+ main controller to sequence the targets.
 *           Each target will connect to the Pi over USB.  This will allow 
 *           all sorts of game modes to be created.  
 *           I was originally going to write this in C++ but I'm leaning 
 *           towards using Node.js and Javascript so that it is easier for 
 *           someone to learn and modify.  I am testing it to see if it 
 *           performs well enough.  It will have a webpage to read the stats
 *           and start/restart the game modes from a Phone/Tablet.  
 *           All of the data will be saved in an SQLite database.
 *           
 *           Have fun!  I'm sure there are lots of improvements and ideas
 *           to take this project in many directions.  I added a lot of 
 *           code comments that I wouldn't normally do just in case someone
 *           is learning how to code.
 */

/**********This is for the setup of the RGB LED Strip*****************/
/**********Copied from an example in the FastLed library**************/ 
#include <FastLED.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    3
//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    24
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  120
/********************************************************************/


/******This for the setup of the Accelerometer MPU6050 board*********/
/********************************************************************/
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;
/**********************************************************************/

//STM32 Blue Pill on board LED triggers when target is hit
#define LED_PIN 13  

//Variable to track target hits
bool targetHit = false;
unsigned long hitTime =0;
int   hitCount = 0;
float result =0;
float threshold = 9500;
float maxResult =0;
bool printed = false;


//Serial data buffer
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;


//Default Arduino setup function that runs at startup
void setup() {

    //FastLED LED strip configuration
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);
  
    // configure STM32 Blue Pill LED for 
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, true);
    
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize the MPU6050 
    accelgyro.initialize();
    //Set the range to 16g (Maximum)
    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);    
}

//This function gets called repeatedly forever
void loop() {

    // read raw accel/gyro measurements from device
    // I haven't done any calibration or remove gravity from the sensor
    // should do that for completeness 
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    //Calculate the resultant magnitude of the 3 acceleration readings
    result = sqrt(ax*ax + ay*ay + az*az);
          
    //We got a hit. The threshold his the minimum value that the target
    //will detect a hit.  It's a trade off between detecting a hit and
    //getting a false trigger from hitting a post.
    if(!targetHit && result >threshold){
      hitTime = millis();
      targetHit = true;
      //Turn on the STM32 blue pill on board LED
      digitalWrite(LED_BUILTIN, false);
      //Turn the LED light strip RED
      FastLED.showColor(CRGB::Red); 
      //Increment the hit counter
      hitCount++;
    }

    //Save the greatest result value 
    if(targetHit && result >maxResult){
      maxResult = result;
    }

    //In a normal impact the force will reach a peak and then
    //drop in value.  When the current result is less than the
    //maximum result recorded we print out the result
    //This will eventually be read by the Raspberry Pi
    if(targetHit && result <maxResult && !printed){
      // display tab-separated accel/gyro x/y/z values and the time
        Serial.print("h/t/r:\t");
        Serial.print(hitCount); Serial.print("\t");
        Serial.print(millis()); Serial.print("\t");
        Serial.println(maxResult);
        printed = true;
    }

    //Turn off hit light after 750 ms
    if(targetHit && (millis() - hitTime > 750)){
      //Turn off the STM32 onboard LED
      digitalWrite(LED_BUILTIN, true);
      //Reset the target hit variable
      targetHit = false;
      //Reset the maximum result to 0
      maxResult = 0;
      //Reset the printed variable
      printed = false;
      //Turn the LED strip back to the default BLUE color
      FastLED.showColor(CRGB::Blue);
    }

    
    //Serial communication with Raspberry PI - Currently just looping back
    //Need to setup protocol to control the target
    //Reference http://forum.arduino.cc/index.php?topic=288234.0
    recvWithStartEndMarkers();
    showNewData();

}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
 // if (Serial.available() > 0) {
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
}
