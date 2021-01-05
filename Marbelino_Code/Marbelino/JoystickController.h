//--------------------------------------------//
//------- INPUT Joystick Configuration -------//
//--------------------------------------------//




// Kalman Filter /////////////////////////////////////
//Measurement Uncertainty - How much do we expect to our measurement vary
#define kalman_measure 300
  
//Estimation Uncertainty - Can be initilized with the same value as e_mea since the kalman filter will adjust its value
#define kalman_estimation 500
  
//Process Variance - usually a small number between 0.001 and 1 - how fast your measurement moves. Recommended 0.01. Should be tunned to your needs.
#define kalman_variance 1
  
SimpleKalmanFilter YFilter(kalman_measure, kalman_estimation, kalman_variance);

SimpleKalmanFilter XFilter(kalman_measure, kalman_estimation, kalman_variance);

class JoystickController{
  public:
    int joystick_X, joystick_Y, joystick_C; //INPUT PINS
    int joystick_offset = 10; // Cleaning Limits
    int step_arrow = 10;
    int last_reading = 0;
    
    JoystickController( int pinX, int pinY , int clickpin ): joystick_X( pinX ), joystick_Y( pinY ), joystick_C( clickpin ){
    
    }

    void init(){
      pinMode( joystick_X, INPUT );
      pinMode( joystick_Y, INPUT );
      pinMode( joystick_C, INPUT_PULLUP );
      pinMode( A5, INPUT );
      randomSeed( analogRead( A5) );
    }

    int readX(){
      int sample;
      //int reading = analogRead( joystick_X );
      int reading = map ( analogRead( joystick_X ), 0, 1023, MAX_JOYSTICK, MIN_JOYSTICK );
      /*#ifdef JOYSTICK_DOWN
        int reading = ( 1023 - analogRead( joystick_X ) );
      #else
        int reading =analogRead( joystick_X );
      #endif
      */
      if ( ( reading > ( 1023/2 - joystick_offset ) )  &&  ( reading < ( 1023/2 + joystick_offset ) ) ){ reading = 1023/2 ;}

      sample = map ( reading , 0, 1023, step_arrow , -step_arrow );
      int estimated_value = XFilter.updateEstimate(sample);
      
      // Change value instead of estimated value
      return sample;
    }

    int readY(){
      int sample =analogRead( joystick_Y );
      float estimated_value = YFilter.updateEstimate(sample);
      return estimated_value;
    }

    int readYraw(){
        return  analogRead( joystick_Y );
    }

    bool readClick(){
      return digitalRead( joystick_C );
    }
    
};
