
void serial_loop(){
  if( SERIALDEBUG.available()){
    String data = SERIALDEBUG.readString();
    
    if( data.indexOf("S") > 0 ){
      int value = parseInt(SERIALDEBUG.readString());
      
    }
    
  }
}
