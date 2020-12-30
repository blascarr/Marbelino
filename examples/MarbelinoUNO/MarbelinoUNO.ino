#include <SPI.h>  

#include <SimpleKalmanFilter.h>
#include <Adafruit_NeoPixel.h>

//#define DEBUG
//#define MEMORY_DEBUG
#define LOOPGAME

//----- LED GAME Configuration -----//
#define NUM_LEDS_PER_STRIP 240
#define PIN 8  //Defines the Datapin for NeoPixel

Adafruit_NeoPixel stripe = Adafruit_NeoPixel(NUM_LEDS_PER_STRIP, PIN, NEO_GRB + NEO_KHZ800);

//----- MARBLE GAME Configuration -----//
#define NUM_PLAYERS 2 //Fail for Players >= 3
#define NUM_MARBLES 3 //Marbles each player

// Sum of MAX_POWER and MAX_WIND_POWER must be 100
#define MAX_WIND_POWER 30
#define MAX_POWER 70

// Sum of POWER_SIDE and WIND_SIDE must be 100
#define POWER_SIDE 70
#define WIND_SIDE 30

// Intervals of outer Sides for Draw and Slice Shot
#define POWER_ANGLE_SIDE 70
#define WIND_ANGLE_SIDE 30

#define OUT_OF_EDGES 45
#define CENTER_SHOT 10

#define WINDOUT true

#define HOLES 10
#define INC_HOLES (int)( (float)NUM_LEDS_PER_STRIP*(float)0.02)
#define NHOLES ( (int)HOLES + (int)INC_HOLES )
#define BRIGHTNESS 150

#define FAILHOLES 4
#define INC_FAILHOLES (int)( (float)NUM_LEDS_PER_STRIP*(float)0.01)
#define NFAILHOLES ( (int)FAILHOLES + (int)INC_FAILHOLES )

//----- JOYSTICK Configuration -----//
int joystick_X = A0;
int joystick_Y = A1;
int joystick_C = 4;

#include "JoystickController.h"
#include "MarbleGame.h"

//Declaracion objeto TFT

JoystickController joy = JoystickController( joystick_X , joystick_Y ,  joystick_C );

marblegame game( stripe, joy );


void setup() {
  Serial.begin(9600);
  Serial.print("Marble Game");
  
  game.setPlayername( "BLACKPEN", 0 );
  game.setPlayername( " REDPEN ", 1 );
  game.init();

}

void loop() {
  game.update();
  
}
