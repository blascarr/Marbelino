#include "config.h"  
#include "Debug_Controller.h" 
#include <SimpleKalmanFilter.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel stripe = Adafruit_NeoPixel(NUM_LEDS_PER_STRIP, PIN, NEO_GRB + NEO_KHZ800);

#include "JoystickController.h"
#include "TFTController.h"
#include "MarbleGame.h"

//Declaracion objeto TFT
TFTMarble tft = TFTMarble(cs, dc, rst);

JoystickController joy = JoystickController( joystick_X , joystick_Y ,  joystick_C );

marblegame game( stripe, tft, joy );

void setup() {
  SERIALDEBUG.begin(9600);
  DUMPSLN("Marble Game");
  
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
