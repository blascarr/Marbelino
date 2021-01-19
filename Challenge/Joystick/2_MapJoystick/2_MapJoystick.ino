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
  
  //Devolver el valor del sensor mapeado entre los valores definidos
  return filter;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  int valueX = analogRead( joystick_X );
  int valueX_Filtered = filteroffset( valueX );
  
  Serial.print( valueX );
  Serial.print( "," );
  Serial.println( valueX_Filtered );
}
