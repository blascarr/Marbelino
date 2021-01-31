#include <SPI.h>  
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
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

#define OUT_OF_EDGES 45 // OUT OF RANGE
#define CENTER_SHOT 10 //IMPULSE DEF

#define IMPULSE_HELPER //IMPULSE DEF

#define WINDOUT true

#define HOLES 10
#define INC_HOLES (int)( (float)NUM_LEDS_PER_STRIP*(float)0.02)
#define NHOLES ( (int)HOLES + (int)INC_HOLES )
#define BRIGHTNESS 150

#define FAILHOLES 4
#define INC_FAILHOLES (int)( (float)NUM_LEDS_PER_STRIP*(float)0.01)
#define NFAILHOLES ( (int)FAILHOLES + (int)INC_FAILHOLES )

#define HOLE_POINTS 15
#define FAILHOLE_POINTS 5

//----- JOYSTICK Configuration -----//

#define JOYSTICK_DOWN

#ifdef JOYSTICK_DOWN
  #define MIN_JOYSTICK 0
  #define MAX_JOYSTICK 1023
#else
  #define MIN_JOYSTICK 1023
  #define MAX_JOYSTICK 0
#endif 

int joystick_X = A0;
int joystick_Y = A1;
int joystick_C = 4;

//#define TFT_DOWN

#ifdef TFT_DOWN
  #define TFT_ORIENTATION 1
#else
  #define TFT_ORIENTATION 3
#endif 

#include "JoystickController.h"
#include "TFTController.h"
#include "MarbleGame.h"

//Declaracion objeto TFT
TFTMarble tft = TFTMarble(cs, dc, rst);

JoystickController joy = JoystickController( joystick_X , joystick_Y ,  joystick_C );

marblegame game( stripe, tft, joy );


void setup() {
  Serial.begin(9600);
  Serial.println("Marble Game");
  
  tft.initR(INITR_BLACKTAB); // Tab Label for ST7735 TFT Screen (INITR_GREENTAB 0x0, INITR_REDTAB 0x1, INITR_BLACKTAB 0x2)
  game.setPlayername( "BLACKPEN", 0 );
  game.setPlayername( " REDPEN ", 1 );
  game.setPlayercolor( TFTBLUE, 0 );
  game.setPlayercolor( TFTMAGENTA, 1 );
  game.init();

}

void loop() {
  game.update();
  
}
