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
	void waitForComplete(); // 送受信処理の完了を待つ。

	uint8_t readByteACK();                           // 1バイトを受信してACKで応答する。
	void    readBytesACK(uint16_t n, uint8_t *buff); // nバイトを受信して最後もACKで応答する。

	uint8_t readByteNACK();                           // 1バイトを受信してNACKで応答する。
	void    readBytesNACK(uint16_t n, uint8_t *buff); // nバイトを受信して最後だけNACKで応答する。

	void writeByte(uint8_t byte);               // 1バイトを送信する。
	void writeBytes(uint16_t n, uint8_t *buff); // nバイトを送信する。

public:
	void init(uint32_t f_cpu=F_CPU, uint32_t scl_freq=100000UL); //I2Cペリフェラルを有効化する。
	void quit();                                                 //I2Cペリフェラルを無効化する。

	void start(); // STARTコンディションを生成する。
	void stop();  // STOPコンディションを生成する。

	// (どのスレーブの, どこから, 何バイトを, どこへ)読み出す。
	void read(uint8_t slave_addr, uint8_t first_register, uint16_t n, uint8_t*buff);

	// (どのスレーブの, どこから) 1バイトを読み出す。
	uint8_t read(uint8_t slave_addr, uint8_t which_register);

	// (どのスレーブの, どこへ, 何バイトを, どこから)書き込む。
	void write(uint8_t slave_addr, uint8_t first_register, uint16_t n, uint8_t*buff);

	// (どのスレーブの, どこへ, 1バイトを)書き込む。
	void write(uint8_t slave_addr, uint8_t which_register, uint8_t byte);


	// コンストラクタ
	I2CClass();
};


// インスタンスの外部参照宣言
extern I2CClass i2c;

#endif
