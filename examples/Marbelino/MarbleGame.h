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
uint32_t YELLOW = Color( 255,255,0 );
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
    int oldPosition = 0;
    int position = 0;
    marble* next_marble;
    marble* over_marble;
    bool first = false;
    bool firstinstrip = false;
        
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

    void setNextMarble( marble* marble ){
      next_marble = marble;
    }

    void setOverMarble( marble* marble ){
      over_marble = marble;
    }
};

int compareMarbleOrder (const void * a, const void * b)
{
  return ( *(marble*)a - *(marble*)b );
}

int compareMarblePointers_DESC(const void * a, const void * b)
{
  const marble* pa = *( const marble ** )a;
  const marble* pb = *( const marble ** )b;
  return  ( pa->position > pb->position );
}

int compareMarblePointers_ASC(const void * a, const void * b)
{
  const marble* pa = *( const marble ** )a;
  const marble* pb = *( const marble ** )b;
  return  ( pa->position < pb->position );
}

class marbleplayer{
  public:

    //----- marble Controller -----//
    marble* current_marble;
    uint8_t current_nmarble = 0;
    uint8_t points = 0;

    marble* marblequeue[ NUM_MARBLES ];
    marble marbles[ NUM_MARBLES ];
    
    //----- player Info -----//
    
    uint8_t num_player;
    String playername;

    marbleplayer( ){
      init_marbles( num_player );
    }
    
    marbleplayer( String name ){
      playername = name;
      init_marbles( num_player );
    }

    //----- Marble Manager -----//
    void init_marbles( int index = 0 ){
      
      marbles[0].first = true;
      
      for ( int i = 0; i < NUM_MARBLES; i++ ){
         marbles[i].setColor( colorList[ i ] );
         marblequeue[i] = &marbles[i];

         //Set order position for each marble under cero for first launch
         marbles[i].position = i*(-1);
      }
      
      current_nmarble = NUM_MARBLES -1;
      current_marble = &marbles[ NUM_MARBLES -1 ];
    }

    void nextMarble(){
      current_nmarble = ( current_nmarble == NUM_MARBLES -1 ) ? 0: current_nmarble+1 ;
      set_current_marble( current_nmarble );
    }

    void set_current_marble( int nmarble ){
      current_nmarble = nmarble;
      current_marble = &marbles[ nmarble ];
    }

    void search_crash_marble(){
      // Define list of marbles in stripe //Against my marbles or against enemy marbles
      
      int n = sizeof(marblequeue) / sizeof(marblequeue[0]); 
      qsort( marblequeue, n, sizeof(marblequeue[0]) , compareMarblePointers_DESC);

      //Guardar next marble pointer in object
      for ( int i= 0 ; i < NUM_MARBLES; i++ ){
        marblequeue[i]->setNextMarble( marblequeue[ (i+1) % NUM_MARBLES ] );
      }

      //Problem with first marble
      /*for ( int i= 0 ; i < NUM_MARBLES; i++ ){
          marbles[ i ].first = false;
      }*/
      //Set first marble with greater position and set to false the second one
      marblequeue[ NUM_MARBLES -1 ]->first = true;
      marblequeue[ NUM_MARBLES -2 ]->first = false;
      /*for ( int i= 0 ; i < NUM_MARBLES; i++ ){
          Serial.print( i );
          Serial.print( " - " );
          Serial.print( marbles[i].position );
          //Serial.print( " - " );
          //Serial.print( marblequeue[i]->position );

          //Serial.print( " - " );
          //Serial.print( marblequeue[(i+1) % NUM_MARBLES]->position );
          
          Serial.print( " - " );
          Serial.print( marbles[i].next_marble->position );
          Serial.print( " - " );
          Serial.println( marbles[i].first );

      }*/
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

    marble* overqueue[ NUM_MARBLES*NUM_PLAYERS ];
    
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
      for ( int i = 0; i < NUM_PLAYERS; i++ ){
          players[ num_players ].playername = String( "Player "+ String( num_players +1 ) ); // Apunta el puntero del array al objeto externo.

          Serial.print( players[ num_players ].playername );
          Serial.println( " added to the game" );
          num_players++;

          for ( int j = 0; j < NUM_MARBLES; j++ ){
              //Set order position for each marble under cero for first launch
              
              //No ejecuta bien la suma de numeros negativos en el segundo lanzamiento
              // players[ i ].marbles[j].position = (i*NUM_MARBLES+j)*(-1);
              
              // Asociamos inicialmente puntero a las canicas por orden de aparicion 
              overqueue[i*NUM_MARBLES+j] = &(players[ i ].marbles[j]); 
          }  
      }
    }

    void init(){
        strip.begin();
        tft.init();
        joystick.init();
        
        // Draw Radar Interface
        tft.draw_radar();

        //Draw Players. Start with next player
        current_nplayer = NUM_PLAYERS-1;

        //Set Marble colors for each player
        //For thinner range subdivide in more parts. Default 6 for RED - YELLOW - GREEN - CYAN - BLUE - MAGENTA
        uint8_t HSV_divisor = 6;
        
        uint16_t rangeColor = 65535/HSV_divisor;
        uint16_t marbleRange = rangeColor/NUM_MARBLES;
        
        // Fitst Color Offset for range in HSV Values
        /*
        // RED  -10
        // YELLOW  50
        // GREEN  110
        // CYAN  170
        // BLUE  230
        // MAGENTA  290
        */
        
        int offset = 110;
        
        for ( int i = 0; i < NUM_PLAYERS; i++ ){
          
          uint16_t colorOffset = ( ( ((360+offset)%360)*65535/360) + 65535*i/NUM_PLAYERS );
          
          for ( int j = 0; j < NUM_MARBLES; j++ ){
            //Set Marble colors for each player
            uint32_t rgbcolor = strip.ColorHSV( ( j*marbleRange + colorOffset )% 65536  );
            players[ i ].marbles[j].setColor( rgbcolor );
            
            //Set order position for each marble under cero for first launch
            //players[ i ].marbles[j].position = (i*NUM_MARBLES+j)*(-1);
            
            //Establecemos la configuracion inicial a la siguiente canica en la lista
            
            int index = ( (i*NUM_MARBLES+j-1) < 0 )? ( index = i + NUM_MARBLES *NUM_PLAYERS -1 ):( index = (i*NUM_MARBLES+j-1) );
            players[ i ].marbles[j].setOverMarble( overqueue[ index ] );
            
          }  
        }
        
        // Set first matble with true value, for control over other marbles
        players[ 0 ].marbles[ 0 ].firstinstrip = true;
        
    }
    
    //------------------Players Manager---------------------------//
    void addPlayer( marbleplayer player, int index = 0 ){
      //Set num_players to zero to overwrite names
      
      players[ index ].playername = player.playername; // Solo cambia el nombre, el objeto esta creado dentro de la clase

      Serial.print( players[ index ].playername );
      Serial.println( " added to the game" );
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
      Serial.print( " - Marble : ");
      Serial.println( players [current_nplayer].current_nmarble );
      players [current_nplayer].search_crash_marble();
    }
    
    //----------------------Marble Manager---------------------------//
    void set_current_marble( int index = 0){
      //players [ index ].nextMarble();
    }

    void set_next_marble( ){
      players [current_nplayer].nextMarble();
    }

    void bounce_to_marble( marble* next ){
      players [current_nplayer].current_marble = next;
    }

    void search_over_marble(){
      
      int n = sizeof(overqueue) / sizeof(overqueue[0]); 
      qsort( overqueue, n, sizeof(overqueue[0]) , compareMarblePointers_ASC);

      //Guardar over marble pointer in object
      for ( int i= 0 ; i < NUM_MARBLES*NUM_PLAYERS; i++ ){
        int index = ( (i-1) < 0 )? ( index = i + NUM_MARBLES *NUM_PLAYERS -1 ):( index = ( i-1 ) );
        overqueue[i]->setOverMarble( overqueue[ index ] );
        
      }

      // Set first marble in strip and set false the second one
      overqueue[0]-> firstinstrip = true;
      overqueue[1]-> firstinstrip = false;
      
      /*for ( int i= 0 ; i < NUM_MARBLES*NUM_PLAYERS; i++ ){
          int index = ( (i-1) < 0 )? ( index = i + NUM_MARBLES *NUM_PLAYERS -1 ):( index = ( i-1 ) );
          Serial.print( i );
          Serial.print( " - \t" );
          Serial.print( players[i/NUM_MARBLES].marbles[i%NUM_MARBLES].position );
          Serial.print( " - \t" );
          Serial.print( players[i/NUM_MARBLES].marbles[i%NUM_MARBLES].over_marble->position );
          
          Serial.print( " - \t" );
          Serial.print( overqueue[i]->position );
          Serial.print( " - \t" );
          Serial.println( overqueue[index]->position );        
      }*/
    }

    void is_over_marble( int playerindex, int marbleindex ){
      search_over_marble();
      int i = playerindex*NUM_MARBLES+marbleindex;
      /*Serial.println("SEARCH");
      Serial.print( i );
      Serial.print( " - " );
      Serial.print( overqueue[i]->position );
      Serial.print( " - " );
      Serial.println( overqueue[i]->over_marble->position );*/
          
    }

    void shift_over_marble( int playerindex, int marbleindex ){
      Serial.println( "OVERMARBLE - ");
      search_over_marble();
      /*Serial.print( playerindex );
      Serial.print( " - \t" );
      Serial.print( marbleindex);
      Serial.print( " - \t" );
      Serial.println( ( playerindex*NUM_MARBLES + marbleindex ) );*/
      /*marble* next_over = players[ playerindex ].marbles[ marbleindex ].over_marble->over_marble;
      
      players[ playerindex ].marbles[ marbleindex ].over_marble->setOverMarble( overqueue[ ( playerindex*NUM_MARBLES + marbleindex ) ] );
      players[ playerindex ].marbles[ marbleindex ].setOverMarble(  next_over  );
      
      for ( int i= 0 ; i < NUM_MARBLES*NUM_PLAYERS; i++ ){
          int index = ( (i-1) < 0 )? ( index = i + NUM_MARBLES *NUM_PLAYERS -1 ):( index = ( i-1 ) );
          Serial.print( i );
          Serial.print( " - \t" );
          Serial.print( players[i/NUM_MARBLES].marbles[i%NUM_MARBLES].position );
          Serial.print( " - \t" );
          Serial.print( players[i/NUM_MARBLES].marbles[i%NUM_MARBLES].over_marble->position );
          
          Serial.print( " - \t" );
          Serial.print( overqueue[i]->position );
          Serial.print( " - \t" );
          Serial.println( overqueue[index]->position );       
      }*/
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
            //Serial.print( "," );
            //Serial.print( position );
            Serial.print( "," );
            Serial.print( timeInterval );
            Serial.println();
          #endif
          
          if ( round(dx) <= 0 ){
            marbleOn = false;
            return;
          }

          // Marble return to the Beginning of the strip
          if(  players [current_nplayer].current_marble->oldPosition > players [current_nplayer].current_marble->position ){
            players [current_nplayer].current_marble->first = false;
            //players [current_nplayer].current_marble->next_marble->first = true;
          }
          
          // ----- Crash Detection -----//
          if( ( players [current_nplayer].current_marble->position + round( dx ) >= players [current_nplayer].current_marble->next_marble->position) && ( players [current_nplayer].current_marble->first != true) ){
            Serial.print(  players [current_nplayer].current_nmarble );Serial.println(" COLLISION");
            
            //LED ON position -1 for bounce
            players [current_nplayer].current_marble->oldPosition = players [current_nplayer].current_marble->position;
            players [current_nplayer].current_marble->position = players [current_nplayer].current_marble->next_marble->position -1;
            
            //-------- Draw Marble ---------//
            if( players [current_nplayer].current_marble->oldPosition !=  ( players [current_nplayer].current_marble->next_marble->position -1) ){
              strip.setPixelColor( players [current_nplayer].current_marble->oldPosition, strip.Color(0, 0, 0) );
              strip.show();
              strip.setPixelColor( players [current_nplayer].current_marble->position , players [current_nplayer].current_marble->color  );
              strip.show();
            }
          
            //Change to the next marble . Exception for the first marble.
            bounce_to_marble( players [current_nplayer].current_marble->next_marble );
            players [current_nplayer].search_crash_marble();
          }else{
            
          }
          
          // ----- Marble Manage ----- //

          players [current_nplayer].current_marble->oldPosition = players [current_nplayer].current_marble->position;
          players [current_nplayer].current_marble->position += round( dx );
          // Back to the origin
          if( players [current_nplayer].current_marble->position > NUM_LEDS_PER_STRIP ){
            players [current_nplayer].current_marble->position = players [current_nplayer].current_marble->position - NUM_LEDS_PER_STRIP ;
          }

          //-------- Refresh Over Marble ---------//
          // If position is over other marble... No paint or next position.
          
          // If position is greater than over_marble. Search and reconfigure.
          if ( ( players [current_nplayer].current_marble->position > players [current_nplayer].current_marble->over_marble->position) && players [current_nplayer].current_marble->firstinstrip != true ){
            
            shift_over_marble( current_nplayer , players [current_nplayer].current_nmarble );
          }
          
          //search_over_marble( players [current_nplayer].current_marble );
          if ( players [current_nplayer].current_marble->position == players [current_nplayer].current_marble->over_marble->position ){
            
            // Increase +1 for position over the last marble
            players [current_nplayer].current_marble->position ++;

            shift_over_marble( current_nplayer , players [current_nplayer].current_nmarble );
            //is_over_marble( current_nplayer, players [current_nplayer].current_nmarble );
          }
          
          //-------- Draw Marble ---------//
          
          // Parece que rebota, pero simplemente es que al ser del mismo color y pasar por encima, da ese efecto. Cambio de colores para cada jugador
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
            marblegame::is_over_marble( current_nplayer, players [current_nplayer].current_nmarble );
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
