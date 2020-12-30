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

class hole{
  public:
    int position = 0;
    bool isgood = true;
    uint32_t color =  Color( 0,255,255 );
    bool taken = false;
    hole* next_hole;
    
    hole(){ }
    hole( int pos , bool glory ): position( pos ), isgood( glory ) {}

    void set_position( int pos ){
      position = pos;
    }
    
    void set_color( uint32_t c ){
      color = c;
    }

    void take(){
      taken = true;
      color = WHITE;
    }

    void setOverHole( hole* next ){
      next_hole = next;
    }

    int operator - ( const hole& obj ) {
        return ( position - obj.position );
    }
};

class marble{
  public:
    uint32_t color =  Color( 0,0,0 );
    int oldPosition = 0;
    int position = 0;
    marble* next_marble;
    marble* over_marble;
    hole* over_hole;
    bool first = false;
    bool firstinstrip = false;
    uint8_t inHole = 0;
        
    marble( ):color( Color( 0,0,255 ) ){}
  
    marble( uint32_t c ):color( c ){}

    void set_position( int pos ){
      position = pos;
    }
    
    void set_color( uint32_t c ){
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

    void setOverHole( hole* next_hole ){
      over_hole = next_hole;
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

int compareHolePointers_ASC (const void * a, const void * b)
{
  return ( *(hole*)a - *(hole*)b );
}


class marbleplayer{
  public:

    //----- marble Controller -----//
    marble* current_marble;
    uint8_t current_nmarble = 0;
    uint8_t points = 0;

    marble* marblequeue[ NUM_MARBLES ]; //Lista de punteros hacia las canicas para procesar el orden
    marble marbles[ NUM_MARBLES ];      //Vector de canicas donde se guardan las canicas
    
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
         marbles[i].set_color( colorList[ i ] );
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
      Serial.print( "Player : ");
      Serial.print( playername );
      Serial.print( " - Marble : ");
      Serial.print( current_nmarble );
      Serial.print( " - isTaken : ");
      Serial.println( current_marble->inHole );

      #ifndef LOOPGAME
        if( current_marble->inHole > 0 ){
          Serial.println( "JUMP TO NEXT MARBLE" );
          nextMarble();
        }
      #endif
    }

    void search_crash_marble(){
      // Define list of marbles in strip //Against my marbles or against enemy marbles
      int n = sizeof(marblequeue) / sizeof(marblequeue[0]); 
      qsort( marblequeue, n, sizeof(marblequeue[0]) , compareMarblePointers_DESC);

      //Guardar next marble pointer in object
      for ( int i= 0 ; i < NUM_MARBLES; i++ ){
        marblequeue[i]->setNextMarble( marblequeue[ (i+1) % NUM_MARBLES ] );
      }

      //Set first marble with greater position and set to false the second one
      marblequeue[ NUM_MARBLES - 1 ]->first = true;
      marblequeue[ NUM_MARBLES - 2 ]->first = false;
      
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
    int power = 50;
    bool game_over = false;
    
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

    long hole_timestamp;
    int hole_interval = 20;
    
    //----- Wind Controller -----//
    int wind_force = 0;
    uint16_t wind_angle = 180;
    //----- Power Controller -----//
    bool onback = false;
    int base_signal = 521;
    int signal_offset = 10;

    //----- Game Configuration -----//
    bool windout = WINDOUT;
    
    marbleplayer players[ NUM_PLAYERS ];
    
    marbleplayer& current_player ;
    uint8_t current_nplayer = 0;
    uint8_t num_players = 0;

    //----- Hole Controller -----//
    int nhole = 0;
    int nfailhole = 0;
    uint8_t hole_sat = 255;
    uint16_t hole_cos = 0;
    hole holes[ NHOLES + NFAILHOLES ];
    
    marblegame(){}

    marblegame( Adafruit_NeoPixel &marblestrip ): strip( marblestrip ){}

    marblegame( Adafruit_NeoPixel &marblestrip, TFTMarble &tft_screen, JoystickController &joystick ): strip( marblestrip ), tft( tft_screen ),joystick( joystick ) {
      for ( int i = 0; i < NUM_PLAYERS; i++ ){
          players[ num_players ].playername = String( "Player "+ String( num_players +1 ) ); // Apunta el puntero del array al objeto externo.

          Serial.print( players[ num_players ].playername );
          Serial.println( " added to the game" );
          num_players++;

          for ( int j = 0; j < NUM_MARBLES; j++ ){
              // Asociamos inicialmente puntero over a las canicas por orden de aparicion 
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
        wind_force = random( 100 );
        wind_angle = random( 360 );
        tft.draw_wind_force( wind_force );
        
        //Start with next player
        current_nplayer = NUM_PLAYERS-1;

        //---Holes in Strip---//
        uint8_t holeRange = NUM_LEDS_PER_STRIP/(NHOLES + NFAILHOLES);
        for ( int i = 0; i < NHOLES + NFAILHOLES; i++ ){
              holes[i].set_position( random( i*holeRange, (i+1)*holeRange ) );
              holes[i].set_color( 65536/3  );
        }

        for ( int i = 0; i < NFAILHOLES; i++ ){
          uint8_t fail_hole = random( NHOLES + NFAILHOLES );
          holes[ fail_hole ].isgood = false;
          holes[ fail_hole ].set_color( 65536  );
        }
        
        //Init order in holequeue
        marblegame::search_over_hole();
        
        //----Set Marble colors for each player
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
        
        int offset = 170;
        
        for ( int i = 0; i < NUM_PLAYERS; i++ ){
          
          uint16_t colorOffset = ( ( ((360+offset)%360)*65535/360) + 65535*i/NUM_PLAYERS );
          
          for ( int j = 0; j < NUM_MARBLES; j++ ){
            //Set Marble colors for each player
            uint32_t rgbcolor = strip.ColorHSV( ( j*marbleRange + colorOffset )% 65536  );
            players[ i ].marbles[j].set_color( rgbcolor );
            
            //Establecemos la configuracion inicial a la siguiente canica en la lista
            int index = ( (i*NUM_MARBLES+j-1) < 0 )? ( index = i + NUM_MARBLES *NUM_PLAYERS -1 ):( index = (i*NUM_MARBLES+j-1) );
            players[ i ].marbles[j].setOverMarble( overqueue[ index ] );

            //Relacionar todas las canicas con el primer hole de la lista.
            players[ i ].marbles[j].setOverHole( &holes[ 0 ] );
            
          }  
        }
        
        // Set first matble with true value, for control over other marbles
        players[ 0 ].marbles[ 0 ].firstinstrip = true;

        //Draw Players. 
        for ( int i = 0; i < NUM_PLAYERS; i++ ){
          tft.draw_marble( players[i].marbles[0].color, i );
        }
    }
    
    //------------------Players Manager---------------------------//
    void setPlayername( String name, int index = 0 ){
      players[ index ].playername = name;
    }

    void nextPlayer(){
      current_nplayer = ( current_nplayer == (int)(NUM_PLAYERS -1) ) ? 0: current_nplayer+1 ;
      
      
      //Draw name o Top
      tft.drawHeader( players [current_nplayer].playername );
      tft.draw_score ( players [current_nplayer].points );
      
      // Next Marble
      marblegame::set_next_marble();
      
      players [current_nplayer].search_crash_marble();
    }
    
    //----------------------Marble Manager---------------------------//
    void set_current_marble( int index  ){
      players[current_nplayer].current_marble = &players[current_nplayer].marbles[index];
    }

    void set_next_marble( ){
      players [current_nplayer].nextMarble();
      /*if( players [current_nplayer].current_marble->inHole != true ){
        
      }else{
          int n = 0;
          //players [current_nplayer].nextMarble();
          while ( players [current_nplayer].current_marble->inHole == true ){
            n++;
            players [current_nplayer].nextMarble();
            Serial.print(" Next" );
            Serial.print( n );
            if ( n > NUM_MARBLES ) {
              String winHeader = players [current_nplayer].playername + " WIN";
              Serial.println( winHeader );
              game_over = true;
              tft.drawHeader( winHeader );
              return;
            }
          }
       }*/
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
      
      /*
      #ifdef DEBUG
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
       }
      #endif
      */
    }

    void search_over_hole(){
      int n = sizeof(holes) / sizeof(holes[0]); 
      qsort( holes, n, sizeof(holes[0]) , compareHolePointers_ASC );

      // Define next holes for each element
      //Guardar over Hole pointer in object
      for ( int i= 0 ; i < n ; i++ ){
        int index = (i+1)%n;
        holes[i].setOverHole( &holes[ index ] );
      }
      
      /*Serial.println();
       for ( int i= 0 ; i < n ; i++ ){
          int index = (i+1)%( n );
          //int index = ( (i-1) < 0 )? ( index = i + n -1 ):( index = ( i-1 ) );
          Serial.print( i );         
          Serial.print( " - \t" );
          Serial.print( index );
          Serial.print( " - \t" );
          Serial.print( holes[i].position );
          Serial.print( " - \t" );
          Serial.print( holes[i].next_hole->position );  
          Serial.print( " - \t" );
          Serial.println( holes[i].isgood );       
       }*/
       
    }
    
    void shift_over_marble( ){
      #ifdef DEBUG
        Serial.println( "OVERMARBLE - ");
      #endif
      search_over_marble();
    }

    uint8_t move(){
          //dt =  (long) ( millis() - timestamp );
          
          // ----- Timer Control -----//
          //Same intervals between calculations for realistic effect
          dt = calc_interval;
          timeInterval += tg*dt/1000;
          ( timeInterval > T_MAX_VALUE)? T_MAX_VALUE : timeInterval;
          
          // ----- LED Physics -----//
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
            Serial.print( timeInterval );
            Serial.println();
          #endif
          if ( round(dx) < 0 ){ dx = 0 ;}
          
          return round( dx );
    }
    //------------------LED Movement Manager---------------------------//
    void update_strip(){
      if ( marbleOn ){
        
        if( millis() - timestamp > timeInterval ){
          
          uint8_t displacement = move();
          timestamp = millis();
          
          marble* current_marble = players [current_nplayer].current_marble;
          marble* next_marble = players [current_nplayer].current_marble->next_marble;
          
          // Marble return to the Beginning of the strip
          if(  current_marble->oldPosition > current_marble->position ){
            current_marble->first = false;
          }

          //-------- Detect Over Holes ---------//

          if ( ( current_marble->position  == current_marble->over_hole->position ) && ( displacement == 0 ) ) {
            Serial.println( "TAKEN");
            current_marble->inHole = current_marble->inHole + 1 ;
            marbleOn = false;
            
            //Detect if hole or failhole
            if( current_marble->over_hole->isgood ){
              players [current_nplayer].points += 5;
            }else{
              players [current_nplayer].points -= 3;
            }
            //Paint score
            tft.draw_score ( players [current_nplayer].points );
            
            current_marble->over_hole->take();
            
            //Define new Holes, quit the hole
            
          }

          if ( displacement == 0 ){
            marbleOn = false;
            return;
          }
          
          // ----- Crash Detection with other marbles -----//
          if( ( current_marble->position + displacement >= next_marble->position ) && ( current_marble->first != true) ){
              Serial.print(  players [current_nplayer].current_nmarble );Serial.println(" COLLISION");
            
              //LED ON position -1 for bounce
              current_marble->oldPosition = current_marble->position;
              current_marble->position = next_marble->position -1;
            
              //-------- Draw Marble ---------//
              if( current_marble->oldPosition !=  ( next_marble->position -1) ){
                  strip.setPixelColor( current_marble->oldPosition, strip.Color(0, 0, 0) );
                  strip.show();
                  strip.setPixelColor( current_marble->position , current_marble->color  );
                  strip.show();
              }
          
              //Change to the next marble . Exception for the first marble.
              bounce_to_marble( next_marble );
              players [current_nplayer].search_crash_marble();
            }else{
            
              // ----- Standard Marble Manager ----- //
  
              current_marble->oldPosition = current_marble->position;
              current_marble->position += displacement; //Este valor nunca llega a cero
              
              // Back to the origin
              if( current_marble->position > NUM_LEDS_PER_STRIP ){
                current_marble->position = ( current_marble->position -1 ) % NUM_LEDS_PER_STRIP ;
              }
  
              //-------- Refresh Over Marble ---------//
            
              // If position is greater than over_marble. Search and reconfigure.
              if ( ( current_marble->position >= current_marble->over_marble->position ) && current_marble->firstinstrip == false ){
              
                shift_over_marble(  );
                if ( current_marble->position == current_marble->over_marble->position ){
                    current_marble->position ++;
                }
              }
              
              // Si supera la posicion de un hole, se enlaza al siguiente
              if (  current_marble->position  > current_marble->over_hole->position  ) {
                current_marble->setOverHole( current_marble->over_hole->next_hole );
              }
              //-------- Draw Marble ---------//

                strip.setPixelColor( current_marble->oldPosition, strip.Color(0, 0, 0) );
                strip.show();
                strip.setPixelColor( current_marble->position , current_marble->color  );
                strip.show();
          }


              
          #ifdef MEMORY_DEBUG
            Serial.println( freeMemory()  );
          #endif
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
 
        tft.draw_wind_arrow( );
        
        wind_angle = tft.current_arrow_angle;
        wind_angle %= 360;
      }
    }

    void update_power(){
      if ( millis()- power_time > power_interval ) {
        power_time = millis();
        power = 0;
        int reading = joystick.readY();
        int raw = joystick.readYraw();
        
        int constantForce = 5;
        int medium_value = 512; // Mid value of 1023 / 512. To add constant force, you need to add amount on this variable
        
        int invert_sample = ( 1023 - raw );
        int invert_value = ( 1023 - reading );
        
        if ( invert_sample > 700 ){ onback = true; }
       
        //Fix Fire Once - Non negative values
        bool offset = ( invert_sample <= ( base_signal + signal_offset ) ) && ( invert_sample >=  base_signal - signal_offset  ) && ( invert_value > medium_value );
        
        if( offset && onback ){
          onback = false;
          power = ( (long)( invert_value - medium_value )*100)/medium_value + constantForce; // 0 - 100 Value
          power = (power > 100)? 100 : power; //Clean to 100 in case reading errors up to limit
        
          if( windout ){
            //---- Power Proportion + Wind Power ----//
            int wind_offset = (wind_angle + 180)%360;
            power = (power*MAX_POWER)/100 + ( ( wind_force*cos( wind_offset*PI/180 )*MAX_WIND_POWER)/100 ) ;
            
            /*Serial.print(" P ");
            Serial.print(power);
            Serial.print(" O ");
            Serial.print((power*MAX_POWER)/100);
            Serial.print(" W ");
            Serial.println(( wind_force*cos( wind_offset*PI/180 )*MAX_WIND_POWER)/100);*/
          }
          tft.draw_powerbar( power );
        }
        
        if( power > 0 ){
          
            marblegame::launch( power );
            //Change Player and Next Marble
            marblegame::nextPlayer();
            marblegame::search_over_marble();
        } 
      }
    }

    void update_holes(){
       if ( millis()- hole_timestamp > hole_interval ) {
          hole_timestamp = millis();

          //Draw Holes and Calculate Saturation
          nhole ++;
          if ( nhole == ( NHOLES +NFAILHOLES ) ){
            hole_cos += 20;
          }
          nhole = nhole % ( NHOLES +NFAILHOLES );

          strip.setPixelColor( holes[ nhole ].position, strip.gamma32( strip.ColorHSV( holes[ nhole ].color, hole_sat/2*(1+cos( (hole_cos%360)*PI/180 ) ), BRIGHTNESS )  )  );
          strip.show();

          //Draw FailHoles in color RED 
          
       }
    }
      
    update(){
      if( !game_over ){
        update_strip();
        update_readings();
        update_wind();
        update_power();
        update_holes();
      }
    }

    void impulse( int power ){
      v = power;      
    }

    void launch ( int power = 0 ){
      
      marbleOn = true;
      
      timestamp = millis();
      timeInterval = T_MIN_VALUE;
      
      wind_force = random(100);
      tft.draw_wind_force( wind_force );

      //-----  SIDE EFFECT - OUT OF EDGES     ------//
      if( windout ){
        int shot_angle = tft.current_angle;
        
        int wind_offset = ( (wind_angle - 90)%360 );
        if( wind_offset > 180){ wind_offset = 180 - wind_offset%180;}
        wind_offset = map( wind_offset, 0, 180, -90, 90) ;
        
        int power_angle =  ( shot_angle * POWER_ANGLE_SIDE/100) + (wind_offset*WIND_ANGLE_SIDE/100);
        
        String launch_head;
        uint32_t wind_color;
        
        /*Serial.print(shot_angle);
        Serial.print(" W-> ");
        Serial.print(wind_offset);
        Serial.print(" P-> ");
        Serial.println(power_angle);*/
        
        if ( power_angle < ( -90 + OUT_OF_EDGES ) || power_angle > ( 90 - OUT_OF_EDGES ) ){
           Serial.println( "OUT");
           launch_head = "OUT";
           wind_color = TFTRED;
        }else if( ( power_angle > (-OUT_OF_EDGES /2) ) && ( power_angle < (OUT_OF_EDGES /2) ) ){
          Serial.println("IMPULSE");
          launch_head = "IMPULSE";
          wind_color = TFTGREEN;
          power = 2*power;
        }else if( power_angle > 0 ){
          Serial.println("SLICE");
          launch_head = "RIGHT";
          wind_color = TFTBLUE;
        }else{
          Serial.println("DRAW");
          launch_head = "LEFT";
          wind_color = TFTBLACK;
        }
        
        tft.clearLabel( "IMPULSE" ,  tft.w/2+10, tft.h/4+15 , wind_color, true );
        tft.println( launch_head );
      }

      impulse( power );
      
      for ( int i = 0; i < NUM_PLAYERS; i++ ){
        tft.draw_marble( players[i].marbles[0].color, i );
      }
    }

    void set_wind_time ( int duration ){
      wind_interval = duration;
    }

    void set_joystick_time ( int duration ){
      joy_interval = duration;
    }

};
