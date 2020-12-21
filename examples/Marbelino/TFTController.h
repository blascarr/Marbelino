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
    int power_level = 6; //Number 1 - 10
    int power_x = 10;
    int power_y = 10;
    int power_w = 20;
    int power_h = 0;
    int power_height = 0;
    
    //--- Arrow ---//
    int arrow_length = 50;
    int step_arrow = 10;

    int current_angle = 0;
    
    int limit_angle = 90;
    int max_angle = limit_angle;
    int min_angle = -limit_angle;

    //--- Wind Arrow ---//
    int x_arrow = 140;
    int y_arrow = 20;
    int l_arrow = 20;
    
    long arrow_angle = 0;
    long current_arrow_angle = 0;
    int minl_arrow = 8;

    TFTMarble(int CS,int DC,int RST) : Adafruit_ST7735 ( CS, DC, RST) {

    }

    void init(){
      TFTMarble::setRotation(1); 
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

    void draw_powerbar( int reading ){
      
    }
    
    void draw_wind_arrow(){
      current_arrow_angle += random( -10, 10);

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
    
    void draw_radar(){
      drawCircle( w/2, h, arrow_length+2, TFTGREEN);
      drawLine( w/2, h, w/2, h-arrow_length, TFTBLUE );
    }

    void draw_marbles(){
      
    }

    void update(){
      
    }
};
