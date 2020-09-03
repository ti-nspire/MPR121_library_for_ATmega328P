#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#ifndef I2CClass_H
#define I2CClass_H

#include <avr/io.h>

#define SLA_W(slave_addr)  ((slave_addr) << 1)
#define SLA_R(slave_addr)  (((slave_addr) << 1) | 1)

class I2CClass{
private:
	void waitForComplete(); // ����M�����̊�����҂B

	uint8_t readByteACK();                           // 1�o�C�g����M����ACK�ŉ�������B
	void    readBytesACK(uint16_t n, uint8_t *buff); // n�o�C�g����M���čŌ��ACK�ŉ�������B

	uint8_t readByteNACK();                           // 1�o�C�g����M����NACK�ŉ�������B
	void    readBytesNACK(uint16_t n, uint8_t *buff); // n�o�C�g����M���čŌゾ��NACK�ŉ�������B

	void writeByte(uint8_t byte);               // 1�o�C�g�𑗐M����B
	void writeBytes(uint16_t n, uint8_t *buff); // n�o�C�g�𑗐M����B

public:
	void init(uint32_t f_cpu=F_CPU, uint32_t scl_freq=100000UL); //I2C�y���t�F������L��������B
	void quit();                                                 //I2C�y���t�F�����𖳌�������B

	void start(); // START�R���f�B�V�����𐶐�����B
	void stop();  // STOP�R���f�B�V�����𐶐�����B

	// (�ǂ̃X���[�u��, �ǂ�����, ���o�C�g��, �ǂ���)�ǂݏo���B
	void read(uint8_t slave_addr, uint8_t first_register, uint16_t n, uint8_t*buff);

	// (�ǂ̃X���[�u��, �ǂ�����) 1�o�C�g��ǂݏo���B
	uint8_t read(uint8_t slave_addr, uint8_t which_register);

	// (�ǂ̃X���[�u��, �ǂ���, ���o�C�g��, �ǂ�����)�������ށB
	void write(uint8_t slave_addr, uint8_t first_register, uint16_t n, uint8_t*buff);

	// (�ǂ̃X���[�u��, �ǂ���, 1�o�C�g��)�������ށB
	void write(uint8_t slave_addr, uint8_t which_register, uint8_t byte);


	// �R���X�g���N�^
	I2CClass();
};


// �C���X�^���X�̊O���Q�Ɛ錾
extern I2CClass i2c;

#endif
