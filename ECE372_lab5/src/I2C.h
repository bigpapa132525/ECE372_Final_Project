#ifndef I2C_H
#define I2C_H

void initI2C();
void startI2C_Trans(unsigned char SLA);
void stopI2C_Trans();
void write(unsigned char data);
void read_From(unsigned char SLA, unsigned char MEMADDRESS);
unsigned char Read_data();
void ReadIMU(int16_t* x,int16_t* y,int16_t* z);
// below from example
void sendDataI2C(unsigned char i2c_address, unsigned char data_to_send);
#endif
