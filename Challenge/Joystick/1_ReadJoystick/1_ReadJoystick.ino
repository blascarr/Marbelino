#define MAX_JOYSTICK 1023
#define MIN_JOYSTICK 0

int joystick_X = A0;

int filteroffset( int value ){
  //Aplicar filtro dentro de esta funcion para que la lectura del joystick permanezca en el valor 512 cuando no detecte movimiento
  int filter = value;
  
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
