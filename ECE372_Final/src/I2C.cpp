// Initialization for I2C and a function sendatai2c that sends data using I2C
#include <avr/io.h>
#include "I2C.h"
#include "Arduino.h"

#define ADXL345_SLA      0x53   // use 0x1D if ALT ADDRESS is HIGH
// Setup registers
#define POWER_CTL        0x2D
#define DATA_FORMAT      0x31
#define BW_RATE          0x2C   // optional: data rate control
// X-axis
#define DATAX0           0x32   // X low byte
#define DATAX1           0x33   // X high byte
// Y-axis
#define DATAY0           0x34   // Y low byte
#define DATAY1           0x35   // Y high byte
// Z-axis
#define DATAZ0           0x36   // Z low byte
#define DATAZ1           0x37   // Z high byte



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
  TWBR = 0x12; // bit rate generator = 100k  (TWBR = 18)

  TWCR = (1 << TWINT | 1 << TWEN); // enable two wire interface
  
  // Set data format: full resolution
  startI2C_Trans(ADXL345_SLA);
  write(DATA_FORMAT);
  write(0x08);
  stopI2C_Trans();

  // Put ADXL345 into measurement mode
  startI2C_Trans(ADXL345_SLA);
  write(POWER_CTL);
  write(0x08);
  stopI2C_Trans();

  Serial.println("ADXL345 configured");
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

//send nack  
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));

//Recieve byte into TWDR
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));

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


void ReadIMU(int16_t* x,int16_t* y,int16_t* z){
  // X
read_From(ADXL345_SLA, DATAX1);
uint8_t xHigh = Read_data();

read_From(ADXL345_SLA, DATAX0);
uint8_t xLow = Read_data();

// Y
read_From(ADXL345_SLA, DATAY1);
uint8_t yHigh = Read_data();

read_From(ADXL345_SLA, DATAY0);
uint8_t yLow = Read_data();

// Z
read_From(ADXL345_SLA, DATAZ1);
uint8_t zHigh = Read_data();

read_From(ADXL345_SLA, DATAZ0);
uint8_t zLow = Read_data();

// combine
*x = (xHigh << 8) | xLow;
*y = (yHigh << 8) | yLow;
*z = (zHigh << 8) | zLow;
}