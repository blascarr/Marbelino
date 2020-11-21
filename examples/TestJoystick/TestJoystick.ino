#include <SPI.h>  
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SimpleKalmanFilter.h>

// Kalman Filter /////////////////////////////////////
//Measurement Uncertainty - How much do we expect to our measurement vary
#define kalman_measure 300
  
//Estimation Uncertainty - Can be initilized with the same value as e_mea since the kalman filter will adjust its value
#define kalman_estimation 500
  
//Process Variance - usually a small number between 0.001 and 1 - how fast your measurement moves. Recommended 0.01. Should be tunned to your needs.
#define kalman_variance 1
  
SimpleKalmanFilter simpleKalmanFilter(kalman_measure, kalman_estimation, kalman_variance);

#define basesignal_Y 517

// These definitions map display functions to the Adduino UNO display header pins
#define sclk 15
#define mosi 16
#define cs   7
#define dc   6
#define rst  5
    
// Assign human-readable names to some common 16-bit color values:
  #define BLACK 0x0000
  #define WHITE 0xFFFF
  #define BLUE 0x001F
  #define GREEN   0x07E0
  #define YELLOW 0xFFE0 
  #define RED     0xF800
  #define BROWN 0x99AA66
  #define CYAN    0x07FF
  #define MAGENTA 0xF81F

//Declaracion objeto TFT
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

int joystick_X = A0;
int joystick_Y = A1;
int joystick_C = 4;

int width, height;

long debug_time = 0;
int duration = 20;
bool onback = false;
int power_x = 20;
int power_y = 20;
int power_w = 30;
int power_h = 0;
int power_height = 0;

void setup() {
  Serial.begin(9600);
  pinMode( joystick_X, INPUT );
  pinMode( joystick_Y, INPUT );
  pinMode( joystick_C, INPUT_PULLUP );
  
  tft.initR(INITR_BLACKTAB); // Tab Label for ST7735 TFT Screen (INITR_GREENTAB 0x0, INITR_REDTAB 0x1, INITR_BLACKTAB 0x2)

  tft.setRotation(1); 
  tft.fillScreen(WHITE);

  width = tft.width();
  height = tft.height();

  int power_level = 8;  //Number 1 - 10
  power_y = height*( 10 - power_level )/10/2;
  power_height = height*power_level/10;
  
  Serial.print("height Power");
  Serial.println(power_height);
  int length = 100;
  
  tft.drawRect( power_x, power_y, power_w, power_height , BLUE  );
}



void loop() {
  int sample;
  
  if ( millis()-debug_time > duration) {
    
    
    debug_time = millis();
    sample = analogRead(joystick_Y);
    float estimated_value = simpleKalmanFilter.updateEstimate(sample);

    int invert_sample = ( 1023 -sample );
    int invert_value = ( 1023 - estimated_value );
    if ( invert_sample > 1000 ){ onback = true; }
    

    if( ( invert_sample == basesignal_Y ) && onback ){
      onback = false;
      Serial.print("Event: ");
      Serial.print( invert_value );
      Serial.print("\t ");
      int medium_value = 512; // Mid value of 1023 / 512. For some force, you need to add amount on this variable
      long power_h = ( (long)( invert_value - medium_value )*100)/medium_value ;
      Serial.println( power_h );

      long power_tft = (long)((100- power_h)*power_height)/100;

      //Clean TFT 
      tft.fillRect( power_x, power_y, power_w, power_height , WHITE  );
      tft.drawRect( power_x, power_y, power_w, power_height , BLUE  );
      //tft.fillRect( power_x, power_y , power_w, power_h , BLUE  );
      tft.fillRect( power_x, power_y + power_tft , power_w, power_h , BLUE  );
    }
    
    //Serial.print( invert_sample );
    //Serial.print(",");
    //Serial.println( invert_value );
    //Serial.print(",");
    //Serial.print( digitalRead( joystick_C )*1000 );
    //Serial.println();
  }
  
}
