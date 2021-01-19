#include <SimpleKalmanFilter.h>

// ------- Kalman Filter -------//
//Calibra los parámetros de la señal para obtener una respuesta rapida y suave

//Measurement Uncertainty - How much do we expect to our measurement vary
#define kalman_measure 10
  
//Estimation Uncertainty - Can be initilized with the same value as e_mea since the kalman filter will adjust its value
#define kalman_estimation 10
  
//Process Variance - usually a small number between 0.001 and 1 - how fast your measurement moves. Recommended 0.01. Should be tunned to your needs.
#define kalman_variance 0.01

SimpleKalmanFilter XFilter(kalman_measure, kalman_estimation, kalman_variance);

#define MAX_JOYSTICK 1023
#define MIN_JOYSTICK 0

#define MAPVALUE 100
  
int joystick_X = A0;

int filteroffset( int value ){
  int sensor_offset = 20;
  int filter = value;
  if ( value > ( MAX_JOYSTICK/2 - sensor_offset) && value < ( MAX_JOYSTICK/2 + sensor_offset) ){
    filter = MAX_JOYSTICK/2;
  }
  
  return filter;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  int valueX = analogRead( joystick_X );
  int valueX_Filtered = filteroffset( valueX );

  int estimated_value = XFilter.updateEstimate( valueX_Filtered );
  
  Serial.print( valueX );
  Serial.print( "," );
  Serial.println( estimated_value );
}
