/*
MPR121のled[0]端子 (ele[4]端子)からPWM波を出力するプログラム。
*/

#define F_CPU 8000000UL

#include <avr/io.h>
//#include <avr/interrupt.h>
#include <util/delay.h>
//#include "USARTClass.h"

#include "MPR121Class.h"
MPR121Class pad(0x5A); // (I2Cスレーブアドレス)で実体化する。

int main(){
	// ソフトリセットをかける。
	pad.soft_reset();
	
    pad.enable_gpio(0b00000001);        // led[0] (ele[4])をGPIOにする。
    pad.set_gpio_direction(0b00000001); // led[0] (ele[4])のIOをOUTにする。
    pad.set_gpio_circuit(0, SOURCE);    // led[0] (ele[4])の出力回路をソース(ハイサイドオープンドレイン)にする。(PUSH_PULLでもよい)

    // led[0] (ele[4])からPWM波を出力する。ぼわーっとLEDを明滅させる。
    while(1){
        for(int i=0; i<=16; i++){
            pad.analog_write(0, i); // (ledのピン番号, duty=16分のいくつ)
            _delay_ms(30);
        }
        for(int i=16; i>=0; i--){
            pad.analog_write(0, i);
            _delay_ms(30);
        }
    }

	return 0;
}
