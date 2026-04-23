// Initialization for I2C and a function sendatai2c that sends data using I2C
#include <avr/io.h>
#include "I2C.h"
#include "Arduino.h"

unsigned char I2C_Adress = 0x68;

void initI2C() 
{
  // wake up from power reduction:   if the I2C is in power reduction mode (PRTWI bit is 1) 
  //then it needs to be restored by writing a zero to PRTWI.
  
PRR0 &= ~(1 << PRTWI);  //On pg. 236 of ATmega2560 Datasheet   
  
  // generate a 100kHz clock rate
  // SCL freq = 16Mhz/(16 + 2(TWBR)*4)
  // Table 24-7 Prescaler value is 1 so TWPS0 = 0 and TWPS1 = 0.
  TWSR &= ~(1 << TWPS0);
  TWSR &= ~(1 << TWPS1);
  TWBR = 0x12; // bit rate generator = 100k  (TWBR = 74)

  TWCR = (1 << TWINT | 1 << TWEN); // enable two wire interface
 

  
}

void startI2C_Trans(unsigned char SLA){
    // reset flag and intiate a start condition
    // Clear TWINT, send START, enable TWI
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

    // Wait for START to complete
  while (!(TWCR & (1 << TWINT)));
  
  // Load SLA + write bit into data register
  TWDR = (SLA << 1) | 0;//& (0xFE); //load i2c address and write bit (0)
    // Clear TWINT and enable TWI to start transmitting address
  TWCR = (1 << TWINT) | (1 << TWEN);
    // Wait for address transmission to complete
  while (!(TWCR & (1 << TWINT)));
}


void stopI2C_Trans(){
    //send stop condition
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<< TWSTO); 
}

void write(unsigned char data){
    // load the data register with data
  TWDR = data; 
    // clear flag and enable TWI
  TWCR = (1<<TWINT)|(1<<TWEN);
  while(!(TWCR & (1 << TWINT)));
}

unsigned char Read_data(){
  return TWDR;
}

void read_From(unsigned char SLA, unsigned char MEMADDRESS){
  //Start initial message
  startI2C_Trans(SLA);

  //set the register you want to read
  write(MEMADDRESS);

  //create repeated start
  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
  while(!(TWCR & (1 << TWINT)));

  //set the address to read
  TWDR = (SLA << 1) | 0x01;
  // =============--------- MIGHT NEED ACK
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));

  //Recieve byte into TWDR
  TWCR = (1<<TWINT) | (1<<TWEN);
Serial.println("found the bastard");
  // while(!(TWCR & (1<<TWINT)));
Serial.println(" bro its choped");
  // stops transaction
  stopI2C_Trans();
}


/* ------------------------ example -----------------------*/
void sendDataI2C(unsigned char i2c_address, unsigned char data_to_send) {
 
 TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // reset flag and intiate a start condition

 while (!(TWCR & (1<<TWINT)));  // wait while flag is low
 TWDR = (i2c_address << 1) & (0xFE); //load i2c address and write bit (0)
 TWCR = (1<<TWINT)|(1<<TWEN);  // clear flag and enable TWI

 while (!(TWCR & (1<<TWINT))); // wait for flag to set
 TWDR = data_to_send; // load the data register with data
 TWCR = (1<<TWINT)|(1<<TWEN);  // clear flag and enable TWI

 while (!(TWCR & (1<<TWINT))); // wait for flag to set
 TWCR = (1<<TWINT)|(1<<TWEN)|(1<< TWSTO); //send stop condition
}
