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
  
SimpleKalmanFilter simpleKalmanFilter(kalman_measure, kalman_estimation, kalman_variance);

#define basesignal_Y 517



class JoystickController{
  public:
    int joystick_X, joystick_Y, joystick_C;
    int joystick_offset = 10;
    int step_arrow = 10;
    int last_reading = 0;
    
    JoystickController( int pinX, int pinY , int clickpin ): joystick_X( pinX ), joystick_Y( pinY ), joystick_C( clickpin ){
    
    }

    void init(){
      pinMode( joystick_X, INPUT );
      pinMode( joystick_Y, INPUT );
      pinMode( joystick_C, INPUT_PULLUP );
      randomSeed( analogRead( A5) );
    }

    int readX(){
      int sample;
      int reading = analogRead( joystick_X );
      if ( ( reading > ( 1023/2 - joystick_offset ) )  &&  ( reading < ( 1023/2 + joystick_offset ) ) ){ reading = 1023/2 ;}

      sample = map ( reading , 0, 1023, step_arrow , -step_arrow );
      int estimated_value = simpleKalmanFilter.updateEstimate(sample);
    
      return sample;
    }

    int readY(){
      int sample = analogRead( joystick_Y );
      float estimated_value = simpleKalmanFilter.updateEstimate(sample);
      return estimated_value;
    }

    bool readClick(){
      return digitalRead( joystick_C );
    }
    
};
