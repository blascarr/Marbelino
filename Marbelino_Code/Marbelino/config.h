
#define LOOPGAME

//----- LED GAME Configuration -----//
#define NUM_LEDS_PER_STRIP 240
#define PIN 8  //Defines the Datapin for NeoPixel
#define BRIGHTNESS 150

//----- MARBLE GAME Configuration -----//
#define NUM_PLAYERS 2 //Fail for Players >= 3
#define NUM_MARBLES 4 //Marbles each player

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

#define HOLES 3
#define INCREMENT_VALUE 0
#define INC_HOLES (int)( (float)NUM_LEDS_PER_STRIP*(float)INCREMENT_VALUE)
#define NHOLES ( (int)HOLES + (int)INC_HOLES )


#define FAILHOLES 3
#define INCREMENT_FAIL_VALUE 0
#define INC_FAILHOLES (int)( (float)NUM_LEDS_PER_STRIP*(float)INCREMENT_FAIL_VALUE)
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


//----- STREAM CONFIG -----//

//#define DEBUG
//#define MEMORY_DEBUG

#define STREAM_CHARLENGTH 4
#define SERIAL_CONTROL true
long serial_latency = 200;
long serial_millis;
#define SERIAL_DEBUG true
#if SERIAL_DEBUG
    #define SERIALDEBUG Serial
#endif

    
