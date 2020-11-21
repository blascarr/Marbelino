#include <Adafruit_NeoPixel.h>
#define PIN 2  //Defines the Datapin for NeoPixel
#define NUM_LEDS 200 //Define the amount of neopixels you are using. I use 60 neopixels for this code
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);


float Gravity = -9.81;
float friction = -9.81;
int StartHeight = 1;

float Height = StartHeight;
float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
float ImpactVelocity = ImpactVelocityStart;
float TimeSinceLastBounce = 0;
int   Position = 0;
long  ClockTimeSinceLastBounce;

//Cuanto menos dampening mas se frena
float Dampening= 0.80;
  
void setup() {
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    ClockTimeSinceLastBounce = millis();
}

void loop() {
  TimeSinceLastBounce =  millis() - ClockTimeSinceLastBounce;
  Height = 0.5 * Gravity * pow( TimeSinceLastBounce/1000 , 2.0 ) + ImpactVelocity * TimeSinceLastBounce/1000;

  if ( Height < 0 ) {                      
    Height = 0;
    ImpactVelocity = Dampening * ImpactVelocity;
    ClockTimeSinceLastBounce = millis();

    if ( ImpactVelocity < 0.01 ) {
      ImpactVelocity = ImpactVelocityStart;
    }
  }
  Position = round( Height * (NUM_LEDS - 1) / StartHeight);
  

   strip.setPixelColor(Position, strip.Color(0, 255, 0) );
    strip.show();

    for(int i = 0; i < NUM_LEDS; i++ ) {
      strip.setPixelColor(Position, strip.Color(0, 0, 0) );
    }
    strip.show();

  
}
