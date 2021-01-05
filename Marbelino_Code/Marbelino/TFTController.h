//--------------------------------------------//
//------------- TFT CONTROLLER ---------------//
//--------------------------------------------//


// These definitions map display functions to the Adduino UNO display header pins
#define sclk 11
#define mosi 10
#define cs   7
#define dc   6
#define rst  5
    
// Assign human-readable names to some common 16-bit color values:

#define TFTBLACK 0x0000
#define TFTMAGENTA 0xF81F
#define TFTWHITE 0xFFFF
#define TFTBLUE 0x001F
#define TFTGREEN 0x07E0
#define TFTYELLOW 0xFFE0 
#define TFTRED     0xF800
#define TFTBROWN 0x99AA66
#define TFTCYAN    0x07FF
  

class TFTMarble : public Adafruit_ST7735 {
  public:
    int w, h;

    //--- Power Bar ---//
    uint8_t power_level = 6; //Number 1 - 10
    uint8_t power_x = 10;
    uint8_t power_y = 10;
    uint8_t power_w = 20;
    uint8_t power_h = 0;
    uint8_t power_height = 0;
    
    //--- Arrow ---//
    uint8_t arrow_length = 40;
    uint8_t step_arrow = 10;
    int current_angle = 0;
    
    int limit_angle = 90;
    int max_angle = limit_angle;
    int min_angle = -limit_angle;

    //--- Wind Arrow ---//
    uint8_t x_arrow = 140;
    uint8_t y_arrow = 20;
    uint8_t l_arrow = 20;
    
    int arrow_angle = 0;
    int current_arrow_angle = 0;
    uint8_t minl_arrow = 8;

    //--- Marble Drawing ---//
    uint8_t marble_h = 10;
    uint8_t marble_w = 25;
    uint8_t marble_e = 0;
    
    TFTMarble(int CS,int DC,int RST) : Adafruit_ST7735 ( CS, DC, RST) {

    }

    void init(){
      TFTMarble::setRotation( TFT_ORIENTATION ); 
      TFTMarble::fillScreen( TFTWHITE );
  
      w = TFTMarble::width();
      h = TFTMarble::height();
      
      power_height = h*power_level/10;

      TFTMarble::drawRect( power_x, power_y, power_w, power_height , TFTBLUE  );
  
    }

    void draw_arrow( int dif_angle ){
      
      TFTMarble::drawLine( w/2, h, w/2 + arrow_length*sin( current_angle*PI/180 ), h - arrow_length*cos( current_angle*PI/180 ), TFTWHITE ); 
      current_angle += dif_angle;
      if ( current_angle > max_angle ){  current_angle = max_angle; }
      if ( current_angle < min_angle ){  current_angle = min_angle; } 
      TFTMarble::drawLine( w/2, h, w/2 + arrow_length*sin( current_angle*PI/180 ), h - arrow_length*cos( current_angle*PI/180 ), TFTBLUE );
          
    }

    void draw_powerbar( int power_h){
        if( power_h > 0 ){
            int power_tft = (power_h*power_height )/100; // Convert to pixels 
          
            //Clean TFT 
            TFTMarble::fillRect( power_x+1, power_y+1, power_w-2, power_height - power_tft , TFTWHITE  );
            
            //Blue Bar
            TFTMarble::fillRect( power_x+1, power_y+1 + power_height - power_tft , power_w-2, power_tft-1 , TFTBLUE  );
    
            //Power Label
            TFTMarble::clearLabel( "888" , power_x + power_w + 5, power_y , TFTBLUE );
            TFTMarble::println( power_h );
        }
    }
    
    void draw_wind_arrow( ){
      
      current_arrow_angle += random( -step_arrow, step_arrow );
      
      if( current_arrow_angle != arrow_angle){
        //Cleaning Coordinates
        int x_i = x_arrow + l_arrow/2*sin( arrow_angle*PI/180 );
        int y_i = y_arrow - l_arrow/2*cos( arrow_angle*PI/180 );
        int x_f = x_arrow - l_arrow/2*sin( arrow_angle*PI/180 );
        int y_f = y_arrow + l_arrow/2*cos( arrow_angle*PI/180 );
        TFTMarble::drawLine( x_i , y_i , x_f , y_f ,  TFTWHITE );
        
        int xp1 = x_f + minl_arrow*cos( ( 60 - arrow_angle )*PI/180 );
        int yp1 = y_f - minl_arrow*sin( ( 60 - arrow_angle )*PI/180 );
        int xp2 = x_f + minl_arrow*sin( ( arrow_angle -30 )*PI/180 );
        int yp2 = y_f - minl_arrow*cos( ( arrow_angle -30 )*PI/180 );
        
        TFTMarble::drawLine( x_f , y_f , xp1 , yp1 ,  TFTWHITE );
        TFTMarble::drawLine( x_f , y_f , xp2 , yp2 ,  TFTWHITE );

        //Drawing Coordinates
        arrow_angle = current_arrow_angle;
        x_i = x_arrow + l_arrow/2*sin( arrow_angle*PI/180 );
        y_i = y_arrow - l_arrow/2*cos( arrow_angle*PI/180 );
        x_f = x_arrow - l_arrow/2*sin( arrow_angle*PI/180 );
        y_f = y_arrow + l_arrow/2*cos( arrow_angle*PI/180 );
        
        xp1 = x_f + minl_arrow*cos( ( 60 - arrow_angle )*PI/180 );
        yp1 = y_f - minl_arrow*sin( ( 60 - arrow_angle )*PI/180 );
        xp2 = x_f + minl_arrow*sin( ( arrow_angle -30 )*PI/180 );
        yp2 = y_f - minl_arrow*cos( ( arrow_angle -30 )*PI/180 );
        TFTMarble::drawLine(x_i , y_i , x_f , y_f ,  TFTMAGENTA );
        
        TFTMarble::drawLine( x_f , y_f , xp1 , yp1 ,  TFTBLUE );
        TFTMarble::drawLine( x_f , y_f , xp2 , yp2 ,  TFTBLUE );
            
      }
    }

    void draw_wind_force( int wind_value ){
        TFTMarble::clearLabel( String(wind_value) , x_arrow-l_arrow , y_arrow+l_arrow , TFTBLUE );
        TFTMarble::println( wind_value );
        TFTMarble::setCursor( x_arrow-6, y_arrow+l_arrow );
        TFTMarble::println( "Km/h" );
    }
    
    void draw_radar(){
      drawCircle( w/2, h, arrow_length+2, TFTGREEN);
      drawLine( w/2, h, w/2, h-arrow_length, TFTBLUE );
    }

    void draw_marble( uint32_t color, bool left ){
      int side = ( left == true )? -1: 1; 
      TFTMarble::fillRect( w/2 + side*marble_e/2 + side*marble_w , h/2 - marble_h/2, marble_w, marble_h, color );
    }

    void draw_score( uint32_t points ){
      String score = "Score: "+ String( points );
      TFTMarble::clearLabel( score+"00" ,  w/2 , power_y + 20 , TFTBLUE, true );
      TFTMarble::println( score );
    }


    void drawHeader( String header){
        TFTMarble::clearLabel( header ,  w/2 , power_y , TFTBLUE, true );
        TFTMarble::println( header );
    }

    void clearLabel( String label , int x, int y, uint32_t c, bool center = false ){
      int16_t x_label, y_label;
      uint16_t w_label, h_label;
      TFTMarble::getTextBounds( label  , x, y, &x_label, &y_label, &w_label, &h_label);
     
      TFTMarble::setTextColor( c ); 
      if( center ) {
         TFTMarble::setCursor( x-w_label/2, y-h_label/2 );
         TFTMarble::fillRect( x-w_label/2, y-h_label/2, w_label, h_label ,TFTWHITE);
      }else{
         TFTMarble::fillRect( x_label, y_label, w_label, h_label ,TFTWHITE);
         TFTMarble::setCursor( x, y );
      }
      
    }
};
