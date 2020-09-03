/*
MPR121��led[0]�[�q (ele[4]�[�q)����PWM�g���o�͂���v���O�����B
*/

#define F_CPU 8000000UL

#include <avr/io.h>
//#include <avr/interrupt.h>
#include <util/delay.h>
//#include "USARTClass.h"

#include "MPR121Class.h"
MPR121Class pad(0x5A); // (I2C�X���[�u�A�h���X)�Ŏ��̉�����B

int main(){
	// �\�t�g���Z�b�g��������B
	pad.soft_reset();
	
    pad.enable_gpio(0b00000001);        // led[0] (ele[4])��GPIO�ɂ���B
    pad.set_gpio_direction(0b00000001); // led[0] (ele[4])��IO��OUT�ɂ���B
    pad.set_gpio_circuit(0, SOURCE);    // led[0] (ele[4])�̏o�͉�H���\�[�X(�n�C�T�C�h�I�[�v���h���C��)�ɂ���B(PUSH_PULL�ł��悢)

    // led[0] (ele[4])����PWM�g���o�͂���B�ڂ�[����LED�𖾖ł�����B
    while(1){
        for(int i=0; i<=16; i++){
            pad.analog_write(0, i); // (led�̃s���ԍ�, duty=16���̂�����)
            _delay_ms(30);
        }
        for(int i=16; i>=0; i--){
            pad.analog_write(0, i);
            _delay_ms(30);
        }
    }

	return 0;
}
