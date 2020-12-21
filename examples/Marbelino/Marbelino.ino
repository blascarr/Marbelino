#include <SPI.h>  
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SimpleKalmanFilter.h>
#include <Adafruit_NeoPixel.h>

//#define DEBUG

//----- LED GAME Configuration -----//
#define NUM_LEDS_PER_STRIP 240
#define PIN 8  //Defines the Datapin for NeoPixel

Adafruit_NeoPixel stripe = Adafruit_NeoPixel(NUM_LEDS_PER_STRIP, PIN, NEO_GRB + NEO_KHZ800);

//----- MARBLE GAME Configuration -----//
#define NUM_PLAYERS 2
#define NUM_MARBLES 3 //Marbles each player

//----- JOYSTICK Configuration -----//
int joystick_X = A0;
int joystick_Y = A1;
int joystick_C = 4;


#include "JoystickController.h"
#include "TFTController.h"
#include "MarbleGame.h"

//Declaracion objeto TFT
TFTMarble tft = TFTMarble(cs, dc, rst);

JoystickController joy = JoystickController( joystick_X , joystick_Y ,  joystick_C );

marbleplayer blackplayer("BLACKPEN");
marbleplayer redplayer("REDPEN");

marblegame game( stripe, tft, joy );


void setup() {
  Serial.begin(9600);
  Serial.print("Marble Game");
  
  tft.initR(INITR_BLACKTAB); // Tab Label for ST7735 TFT Screen (INITR_GREENTAB 0x0, INITR_REDTAB 0x1, INITR_BLACKTAB 0x2)
  game.init();
  game.addPlayer( blackplayer );
  game.addPlayer( redplayer );

}

void loop() {
  game.update();
  
}
