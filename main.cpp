// OpenCM 9.04 sketch to init servo and test it

#include <wirish/wirish.h>
#include "libraries/Dynamixel/dxl.h"

__attribute__((constructor)) void premain() {
    init();
}

/* Control table defines */
#define ID_Change_Address 3
#define Goal_Postion_Address 30

#define BUFF_MAX 16
char buff[ BUFF_MAX ];

int newServoID;

char *readFromUSB( char *buff, uint16 nToRead ) {
	while( SerialUSB.available() < nToRead )
		delay( 100 );
     
	for( uint16 i=0; i<nToRead && i<BUFF_MAX; i++ )
		buff[ i ] = SerialUSB.read();

  buff[ nToRead ] = 0;
  return buff;
} 

void panic(void) {

  while( 1 ) {
    toggleLED();
    delay( 50 );
  }
}



void setup() {
  unsigned int i;
  
  // Set up the LED to blink
  pinMode(BOARD_LED_PIN, OUTPUT);
  pinMode(BOARD_BUTTON_PIN, INPUT_PULLDOWN );

  // init usb
  SerialUSB.begin();
  
//  // small delay at the begining
//  while( !isButtonPressed() );
  
//  // blink couple of times
//  for( i=0; i<30; i++ ) {
//    toggleLED();
//    delay( 30 );
//  }

  // wait for user
  SerialUSB.println( "Dynamixel AX-12A set up tool" );
  SerialUSB.println( "Press any key to continue" );
  SerialUSB.read();

  dxl_init( 1000000 );
  delay( 1000 );

  SerialUSB.println( "resetting servo (y/n): " );
  char c = readFromUSB( buff, 1 )[0];
  SerialUSB.println( c );
  if( c == 'y' ) {
    dxl_reset( DXL_BROADCAST );
    SerialUSB.println( "servo reset" );
    delay( 1000 );
  }
  
//  SerialUSB.println( "light servo led" );
//  dxl_write_byte( DXL_BROADCAST, DXL_LED, 1);

  // check current servo id
  ui8 servoID = 0xff;
  bool flag = false;

  for( i=1; i<254; i++ )  {
    if( dxl_ping( i ) ) {
      flag = true;
      servoID = i;
      break;
    }
  } 
 
  SerialUSB.print( "Found Servo: " );
  SerialUSB.println( servoID );
    
  // change servo id
  SerialUSB.println( "Changing servo ID.  Enter new number: " );
  newServoID = readFromUSB( buff, 1 )[0] - '0';
  SerialUSB.print( "Setting servo with ID: " );
  SerialUSB.println( newServoID );

  SerialUSB.print( "is this correct (y/n): " );
  c = readFromUSB( buff, 1 )[0];
  SerialUSB.println( c );
  
  if( c == 'n' ) {
    SerialUSB.println( "stopping" );
    panic();
  }

  if( !dxl_read( servoID, ID_Change_Address, buff, 1 ) ) {
    SerialUSB.println( "Unable to read from dynamixel. PANIC!" );
    panic();
  }
  SerialUSB.print( "Current servo ID: " );
  SerialUSB.println( (int)buff[0], DEC );
  
  SerialUSB.println( "writing new ID" );
  dxl_write_byte( DXL_BROADCAST, ID_Change_Address, newServoID );    //Change current id to new id
  
  delay( 1000 );

  if( !dxl_read( newServoID, ID_Change_Address, buff, 1 ) ) {
    SerialUSB.println( "Unable to read from dynamixel. PANIC!" );
    panic();
  }
  newServoID = (int)buff[0];
  SerialUSB.print( "Current servo ID: " );
  SerialUSB.println( newServoID, DEC );

//  SerialUSB.println( "resetting servo" );
//  dx_reset( newServoID );

  delay( 2000 );
  if( !dxl_read( newServoID, ID_Change_Address, buff, 1 ) ) {
    SerialUSB.println( "Unable to read from dynamixel. PANIC!" );
    panic();
  }
  newServoID = (int)buff[0];
  SerialUSB.print( "Current servo ID: " );
  SerialUSB.println( newServoID, DEC );

  dxl_disable( newServoID );
  dxl_enable( newServoID );
}

void loop() {
  int cmd;
  
  SerialUSB.print( "enter command: " );
  cmd = readFromUSB( buff, 1 )[0];
  switch( cmd ) {
    case 'L':
    case 'l':
      /*Turn dynamixel left */
      SerialUSB.println( "left" );
      dxl_write_word( newServoID, Goal_Postion_Address, 300 );
      break;
      
    case 'R':
    case 'r':
      /*Turn dynamixel right */
      SerialUSB.println( "right" );
      dxl_write_word( newServoID, Goal_Postion_Address, 800 );
      break;
      
    case 'C':
    case 'c':
      /*Turn dynamixel to center*/
      SerialUSB.println( "center" );
      dxl_write_word( newServoID, Goal_Postion_Address, 512 );
      break;
  }
    
  delay(100);              
}

int main()
{
  setup();
	
  while( 1 )
	loop();

  return 1;
}
