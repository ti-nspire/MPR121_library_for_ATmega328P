#include "I2CClass.h"
#include "set_SCL_freq.h"

/////////////
/* private */
/////////////
void I2CClass::waitForComplete(){
	loop_until_bit_is_set(TWCR, TWINT);
}
uint8_t I2CClass::readByteACK(){
	TWCR = (1 << TWINT | 1 << TWEN | 1 << TWEA);
	waitForComplete();
	return TWDR;
}
void I2CClass::readBytesACK(uint16_t n, uint8_t *buff){
	for(uint16_t i=0; i<n; i++){
		buff[i] = readByteACK();
	}
}
uint8_t I2CClass::readByteNACK(){
	TWCR = (1 << TWINT | 1 << TWEN);
	waitForComplete();
	return TWDR;
}
void I2CClass::readBytesNACK(uint16_t n, uint8_t *buff){
	readBytesACK(n-1, buff);
	buff[n-1] = readByteNACK();
}
void I2CClass::writeByte(uint8_t byte){
	TWDR = byte;
	TWCR = (1 << TWINT | 1 << TWEN);
	waitForComplete();
}
void I2CClass::writeBytes(uint16_t n, uint8_t *buff){
	for(uint16_t i=0; i<n; i++){
		writeByte(buff[i]);
	}
}


////////////
/* public */
////////////
void I2CClass::init(uint32_t f_cpu, uint32_t scl_freq){
	TWCR |=  (1 << TWEN); // 念のため真っ先にI2Cを有効化しておく。
	set_SCL_freq(f_cpu, scl_freq);
}
void I2CClass::quit(){
	TWCR &= ~(1 << TWEN);
}
void I2CClass::start(){
	TWCR = (1 << TWINT | 1 << TWEN | 1 << TWSTA);
	waitForComplete();
}
void I2CClass::stop(){
	TWCR = (1 << TWINT | 1 << TWEN | 1 << TWSTO);
}
void I2CClass::read(uint8_t slave_addr, uint8_t first_register, uint16_t n, uint8_t*buff){
	start();
	writeByte(SLA_W(slave_addr));
	writeByte(first_register);
	start();
	writeByte(SLA_R(slave_addr));
	readBytesNACK(n, buff);
	stop();
}
uint8_t I2CClass::read(uint8_t slave_addr, uint8_t which_register){
	start();
	writeByte(SLA_W(slave_addr));
	writeByte(which_register);
	start();
	writeByte(SLA_R(slave_addr));
	uint8_t val = readByteNACK();
	stop();
	return val;
}
void I2CClass::write(uint8_t slave_addr, uint8_t first_register, uint16_t n, uint8_t*buff){
	start();
	writeByte(SLA_W(slave_addr));
	writeByte(first_register);
	writeBytes(n, buff);
	stop();
}
void I2CClass::write(uint8_t slave_addr, uint8_t which_register, uint8_t byte){
	start();
	writeByte(SLA_W(slave_addr));
	writeByte(which_register);
	writeByte(byte);
	stop();
}


// コンストラクタ
I2CClass::I2CClass(){}

// 実装ファイルの中で先に実体化しておく。
I2CClass i2c;
