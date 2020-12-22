//--------------------------------------------//
//---------- MARBEL GAME STRUCTURE -----------//
//--------------------------------------------//

#ifdef __arm__
  // should use uinstd.h to define sbrk but Due causes a conflict
  extern "C" char* sbrk(int incr);
#else  // __ARM__
  extern char *__brkval;
#endif  // __arm__

#ifdef MEMORY_DEBUG
  int freeMemory() {
    char top;
    #ifdef __arm__
      return &top - reinterpret_cast<char*>(sbrk(0));
    #elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
      return &top - __brkval;
    #else  // __arm__
      return __brkval ? &top - __brkval : &top - __malloc_heap_start;
    #endif  // __arm__
  }
#endif 

  
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
  //RED, //Color for Fail Hole
  GREEN,
  BLUE,
  YELLOW,
  MAGENTA,
  //WHITE, //Color for Hole
  CYAN
};

class marble{
  public:
    uint32_t color =  Color( 0,0,0 );
    uint16_t oldPosition = 0;
    uint16_t position = 0;
    marble* crash_marble;
    
    marble( ):color( Color( 0,0,255 ) ){
    
    }
  
    marble( uint32_t c ):color( c ){
     
    }

    void setColor( uint32_t c ){
      color = c;
    }

    int operator - ( const marble& obj ) {
        return ( position - obj.position );
    }
};

int compareMarbleOrder (const void * a, const void * b)
{
  return ( *(marble*)a - *(marble*)b );
}

class marbleplayer{
  public:

    //----- marble Controller -----//
    marble* current_marble;
    uint8_t current_nmarble = 0;
    uint8_t points = 0;

    marble* marblequeue[ NUM_MARBLES ];
    marble marbles[ NUM_MARBLES ];
    marble marblequeues[ NUM_MARBLES ];
    //----- player Info -----//
    
    uint8_t num_player;
    char* playername;

    marbleplayer( ){
      setMarbleColors( num_player );
      current_marble = &marbles[0];
    }
    
    marbleplayer( char* name ){
      playername = name;
      setMarbleColors( num_player );
      current_marble = &marbles[0];
    }

    //----- Marble Manager -----//
    void setMarbleColors( int index = 0 ){
      
      for ( int i = 0; i < NUM_MARBLES; i++ ){
         marbles[i].setColor( colorList[ i + index*NUM_MARBLES ] );
         marblequeue[i] = &marbles[i];
      }
    }

    void nextMarble(){
      current_nmarble = ( current_nmarble == NUM_MARBLES -1 ) ? 0: current_nmarble+1 ;
      set_current_marble( current_nmarble );
    }

    void set_current_marble( int nmarble ){
      current_nmarble = nmarble;
      current_marble = &marbles[ nmarble ];
    }
    
    //Devuelve la posicion con la canica con la que se ha chocado como puntero
    int crashmarble(){
      //Calcular la mas cercana de la canica activa y evaluar si se va a chocar contra ella.
      
      return -1;
    }

    void search_crash_marble(){
      //memcpy ( marbles, marblequeue, sizeof( marbles ) );
      //int n = sizeof(marblequeue) / sizeof(marblequeue[0]); 
      
      for ( int i= 0 ; i < NUM_MARBLES; i++ ){
          Serial.print( i );
          Serial.print( " - " );
          Serial.print( marbles[i].position );
          Serial.print( " - " );
          Serial.println( marblequeue[i]->position );
      }
    }
};


//-----------------------------------------------------
//----------- Marble Physics NeoPixels ----------------
//-----------------------------------------------------

class marblegame{
  public: 
    Adafruit_NeoPixel& strip;
    JoystickController& joystick;
    TFTMarble& tft;
    
    bool marbleOn = true;
    uint8_t power = 50;

    float friction = -40;

    float dx;
    float v;
    long dt = 0;

    //----- Timer Controller for Joystick and TFT Drawing Functions -----//
    long joy_time = 0;
    uint8_t joy_interval = 20;
    
    int power_time = 0;
    uint8_t power_interval = 20;
    
    long wind_time = 0;
    uint8_t wind_interval = 100;

    //----- Timer Controller for Marble -----//
    long timestamp;
    int timeInterval = 50;
    uint8_t calc_interval = 50;
    
    int tg = 100;
    int T_MIN_VALUE = 30;
    int T_MAX_VALUE = 300;

    marbleplayer players[ NUM_PLAYERS ];
    
    marbleplayer& current_player ;
    uint8_t current_nplayer = 0;
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

        //Draw Players
        //tft.drawHeader("Player 1");
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
      current_nplayer = ( current_nplayer == (int)(NUM_PLAYERS -1) ) ? 0: current_nplayer+1 ;
      Serial.print( "Player : ");
      Serial.print( players [current_nplayer].playername );
      //Draw name o Top
      tft.drawHeader( players [current_nplayer].playername );
      
      // Next Marble
      marblegame::set_next_marble();
      Serial.print( "Marble : ");
      Serial.println( players [current_nplayer].current_nmarble );
      players [current_nplayer].search_crash_marble();
    }
    //----------------------Marble Manager---------------------------//
    void set_current_marble( int index = 0){
      
    }

    void set_next_marble( ){
      players [current_nplayer].nextMarble();
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
          
          // ----- Crash Detection -----//


          
          // ----- Marble Manage ----- //

          players [current_nplayer].current_marble->oldPosition = players [current_nplayer].current_marble->position;
          players [current_nplayer].current_marble->position += round( dx );
          
          if( players [current_nplayer].current_marble->position > NUM_LEDS_PER_STRIP ){
            players [current_nplayer].current_marble->position = players [current_nplayer].current_marble->position - NUM_LEDS_PER_STRIP ;
          }

          //-------- Draw Marble ---------//
          strip.setPixelColor( players [current_nplayer].current_marble->oldPosition, strip.Color(0, 0, 0) );
          strip.show();
          strip.setPixelColor( players [current_nplayer].current_marble->position , players [current_nplayer].current_marble->color  );
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
        #ifdef MEMORY_DEBUG
          Serial.println( freeMemory()  );
        #endif
      }
    }

    void update_power(){
      if ( millis()- power_time > power_interval ) {
        power_time = millis();
        int reading = joystick.readY();
        
        int power = tft.draw_powerbar( reading, joystick.readYraw() );
        if( power > 0 ){
          
            marblegame::launch( power );
            //Change Player and Next Marble
            marblegame::nextPlayer();
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
