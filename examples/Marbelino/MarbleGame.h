//--------------------------------------------//
//---------- MARBEL GAME STRUCTURE -----------//
//--------------------------------------------//

static uint32_t   Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

uint32_t RED = Color( 255,0,0 );
uint32_t GREEN = Color( 0,255,0 );
uint32_t BLUE = Color( 0,0,255 );
uint32_t YELLOW = Color( 0,255,255 );
uint32_t MAGENTA = Color( 200,50,200 );
uint32_t WHITE = Color( 255,255,255 );
uint32_t CYAN = Color( 0,200,200 );

uint32_t colorList[] = {
  RED,
  GREEN,
  BLUE,
  YELLOW,
  MAGENTA,
  WHITE,
  CYAN
};

class marble{
  public:
    uint32_t color =  Color( 0,0,0 );
    uint32_t oldPosition = 0;
    uint32_t position = 0;

    marble( ):color( Color( 0,0,255 ) ){
    
    }
  
    marble( uint32_t c ):color( c ){
     
    }

    void setColor( uint32_t c ){
      color = c;
    }
  
};



class marbleplayer{
  public:

    //----- marble Controller -----//
    marble* current_marble;
    uint8_t current_nmarble = 0;
    
    marble marbles[ NUM_MARBLES ];

    //----- player Info -----//
    
    uint8_t num_player;
    char* playername;

    marbleplayer( ){
      setMarbleColors( num_player );
      current_marble = &marbles[0];
    }
    
    marbleplayer( char* name ){

      setMarbleColors( num_player );
      current_marble = &marbles[0];
    }

    //----- Marble Manager -----//
    void setMarbleColors( int index = 0 ){
      
      for ( int i = 0; i < NUM_MARBLES; i++ ){
         marbles[i].setColor( colorList[ i + index*NUM_MARBLES ] );
      }
    }

    void nextMarble(){
      ( current_nmarble == NUM_MARBLES -1 ) ? 0: current_nmarble++ ;
      set_current_marble( current_nmarble );
    }

    void launchmarble( ){
      
    }

    void set_current_marble( int nmarble ){
      current_nmarble = nmarble;
      current_marble = &marbles[ nmarble ];
    }
    
    //Devuelve la posicion con la canica con la que se ha chocado
    int crashmarble(){
      //Calcular la mas cercana de la canica activa y evaluar si se va a chocar contra ella.
      
      return -1;
    }
    
};


//-----------------------------------------------------
//----------- Marble Physics NeoPixels ----------------
//-----------------------------------------------------

int   Position = 0;
int   oldPosition = 0;

class marblegame{
  public: 
    Adafruit_NeoPixel& strip;
    JoystickController& joystick;
    TFTMarble& tft;
    
    bool marbleOn = true;
    int power = 50;

    float friction = -40;

    float dx;
    float v;
    long dt = 0;

    //----- Timer Controller for Joystick and TFT Drawing Functions -----//
    long joy_time = 0;
    int joy_interval = 20;
    
    int power_time = 0;
    int power_interval = 20;
    
    long wind_time = 0;
    int wind_interval = 100;

    //----- Timer Controller for Marble -----//
    long timestamp;
    int timeInterval = 50;
    int calc_interval = 50;
    
    int tg = 100;
    int T_MIN_VALUE = 30;
    int T_MAX_VALUE = 300;

    marbleplayer players[ NUM_PLAYERS ];
    uint8_t current_player = 0;
    uint8_t num_players = 0;

    marblegame(){
    
    }

    marblegame( Adafruit_NeoPixel &marblestrip ): strip( marblestrip ){
      
    }

    marblegame( Adafruit_NeoPixel &marblestrip, TFTMarble &tft_screen, JoystickController &joystick ): strip( marblestrip ), tft( tft_screen ),joystick( joystick ) {
      
    }

    void init(){
        strip.begin();
        tft.init();
        joystick.init();
        
        // Draw Radar Interface
        tft.draw_radar();
    }
    
    //------------------Players Manager---------------------------//
    void addPlayer( marbleplayer player ){
      players[0] = player;
      Serial.println("Player added");
      num_players++;
    }
    
    void setPlayer(){
      
    }

    void changePlayer( int index = 0 ){
      
    }

    void nextPlayer(){
      
    }
    //----------------------Marble Manager---------------------------//
    void set_current_marble( int index = 0){
      
    }

    void set_next_marble( ){
      players [current_player].nextMarble();
    }


    //------------------LED Movement Manager---------------------------//
    void update_strip(){
      if ( marbleOn ){
        
        if( millis() - timestamp > timeInterval ){
          
          //dt =  (long) ( millis() - timestamp );
          
          //Same intervals between calculations for realistic effect
          dt = calc_interval;
          timeInterval += tg*dt/1000;
          ( timeInterval > T_MAX_VALUE)? T_MAX_VALUE : timeInterval;
          
          timestamp = millis();
    
          dx = v * dt/(float)1000 + 0.5 * friction * pow( dt/(float)1000 , 2.0 );
          v = v + friction*dt/(float)1000;

          
          #ifdef DEBUG
            Serial.print( dt );
            Serial.print( "," );
            Serial.print( dx );
            Serial.print( "," );
            Serial.print( v * dt/(float)1000 );
            Serial.print( "," );
            Serial.print(0.5 * friction * pow( dt/(float)1000 , 2.0 ) );
            Serial.print( "," );
            Serial.print( round( dx ) );
            Serial.print( "," );
            Serial.print( Position );
            Serial.print( "," );
            Serial.print( timeInterval );
            Serial.println();
          #endif
          
          
          if ( round(dx) <= 0 ){
            marbleOn = false;
            return;
          }

          // ----- Marble Manage -----//

          players [current_player].current_marble->oldPosition = players [current_player].current_marble->position;
          players [current_player].current_marble->position += round( dx );
          
          if( players [current_player].current_marble->position > NUM_LEDS_PER_STRIP ){
            players [current_player].current_marble->position = players [current_player].current_marble->position - NUM_LEDS_PER_STRIP ;
          }

          //-------- Draw Marble ---------//
          strip.setPixelColor( players [current_player].current_marble->oldPosition, strip.Color(0, 0, 0) );
          strip.show();
          strip.setPixelColor( players [current_player].current_marble->position , players [current_player].current_marble->color  );
          strip.show();
          
        }
      }
    }

    // ----- Reading Joystick ------ //
    void update_readings(){
      if ( millis()- joy_time > joy_interval ) {
        joy_time = millis();
        int reading = joystick.readX();
        if ( ( reading != 0 ) && ( reading != joystick.last_reading ) ){
          
          tft.draw_arrow( reading );
          
        }
        
        joystick.last_reading = reading;
        
      }
    }

    void update_wind(){
      if ( millis()- wind_time > wind_interval ) {
        wind_time = millis();

        tft.draw_wind_arrow();
      }
    }

    void update_power(){
      if ( millis()- power_time > power_interval ) {
        power_time = millis();
        int reading = joystick.readY();
        
        int power = tft.draw_powerbar( reading, joystick.readYraw() );
        if( power > 0 ){
          Serial.print("Launch");
            marblegame::launch( power );
        } 
      }
    }
      
    update(){
      update_strip();
      update_readings();
      update_wind();
      update_power();
    }

    void impulse( int power ){
      v = power;      
    }

    void launch ( int power = 0 ){
      marbleOn = true;
      impulse(power);
      timestamp = millis();
      timeInterval = T_MIN_VALUE;
    }

    void set_wind_time ( int duration ){
      wind_interval = duration;
    }

    void set_joystick_time ( int duration ){
      joy_interval = duration;
    }

};
