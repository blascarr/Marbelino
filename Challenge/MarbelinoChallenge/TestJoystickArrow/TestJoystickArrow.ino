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

int power_x = 20;
int power_y = 20;
int power_w = 30;
int power_h = 0;
int power_height = 0;

int arrow_length = 50;
int step_arrow = 10;

int current_angle = 0;

int last_value = 0;
int limit_angle = 90;
int max_angle = limit_angle;
int min_angle = -limit_angle;
int joystick_offset = 10;

// --- ARROW ----
int x_arrow = 120;
int y_arrow = 40;
int l_arrow = 40;
long arrow_time = 0;
int arrow_duration = 200;
long arrow_angle = 0;
long current_arrow_angle = 0;
int minl_arrow = 8;

void setup() {
  Serial.begin(9600);
  pinMode( joystick_X, INPUT );
  pinMode( joystick_Y, INPUT );
  pinMode( joystick_C, INPUT_PULLUP );
  randomSeed(analogRead(5));
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

  tft.drawCircle( width/2, height, arrow_length+2, GREEN);
  tft.drawLine( width/2, height, width/2, height-arrow_length, BLUE );
}



void loop() {
  int sample;
  
  if ( millis()-debug_time > duration) {
    
    
    debug_time = millis();
    int reading = analogRead(joystick_X);
    
    //Clean mechanical errors
    if ( ( reading > ( 1023/2 - joystick_offset ) )  &&  ( reading < ( 1023/2 + joystick_offset ) ) ){ reading = 1023/2 ;}
    
    sample = map ( reading , 0, 1023, step_arrow , -step_arrow );
    int estimated_value = simpleKalmanFilter.updateEstimate(sample);

    
    
    if ( ( estimated_value != 0 ) && ( estimated_value != last_value ) ){
      tft.drawLine( width/2, height, width/2 + arrow_length*sin( current_angle*PI/180 ), height - arrow_length*cos( current_angle*PI/180 ), WHITE );
      current_angle += estimated_value;
      if ( current_angle > max_angle ){  current_angle = max_angle; }
      if ( current_angle < min_angle ){  current_angle = min_angle; }
      last_value = estimated_value;
      
      
      tft.drawLine( width/2, height, width/2 + arrow_length*sin( current_angle*PI/180 ), height - arrow_length*cos( current_angle*PI/180 ), BLUE );
    }
    
    /*Serial.print( sample );
      Serial.print(",");
      Serial.print( estimated_value );
      Serial.print(",");
      Serial.print( current_angle );
      Serial.println();*/
    
    
  }

  if ( millis()- arrow_time > arrow_duration) {
    arrow_time = millis();
    
    current_arrow_angle += random( -10, 10);
    
    if( current_arrow_angle != arrow_angle){
      int x_i = x_arrow + l_arrow/2*sin( arrow_angle*PI/180 );
      int y_i = y_arrow - l_arrow/2*cos( arrow_angle*PI/180 );
      int x_f = x_arrow - l_arrow/2*sin( arrow_angle*PI/180 );
      int y_f = y_arrow + l_arrow/2*cos( arrow_angle*PI/180 );
      tft.drawLine( x_i , y_i , x_f , y_f ,  WHITE );
      int xp1 = x_f + minl_arrow*cos( ( 60 - arrow_angle )*PI/180 );
      int yp1 = y_f - minl_arrow*sin( ( 60 - arrow_angle )*PI/180 );
      int xp2 = x_f + minl_arrow*sin( ( arrow_angle -30 )*PI/180 );
      int yp2 = y_f - minl_arrow*cos( ( arrow_angle -30 )*PI/180 );
      tft.drawLine( x_f , y_f , xp1 , yp1 ,  WHITE );
      tft.drawLine( x_f , y_f , xp2 , yp2 ,  WHITE );
      arrow_angle = current_arrow_angle;
      x_i = x_arrow + l_arrow/2*sin( arrow_angle*PI/180 );
      y_i = y_arrow - l_arrow/2*cos( arrow_angle*PI/180 );
      x_f = x_arrow - l_arrow/2*sin( arrow_angle*PI/180 );
      y_f = y_arrow + l_arrow/2*cos( arrow_angle*PI/180 );
      xp1 = x_f + minl_arrow*cos( ( 60 - arrow_angle )*PI/180 );
      yp1 = y_f - minl_arrow*sin( ( 60 - arrow_angle )*PI/180 );
      xp2 = x_f + minl_arrow*sin( ( arrow_angle -30 )*PI/180 );
      yp2 = y_f - minl_arrow*cos( ( arrow_angle -30 )*PI/180 );
      tft.drawLine(x_i , y_i , x_f , y_f ,  MAGENTA );
      tft.drawLine( x_f , y_f , xp1 , yp1 ,  BLUE );
      tft.drawLine( x_f , y_f , xp2 , yp2 ,  BLUE );
    }
    
    
  }
  
}
