#ifndef SET_SCL_FREQ_H
#define SET_SCL_FREQ_H

#include <avr/io.h>

#define MAX(a, b)                (((a) > (b)) ? (a) : (b))
#define MIN(a, b)                (((a) < (b)) ? (a) : (b))
#define CONSTRAIN(val, min, max) (MIN((MAX((val), (min))), (max)))
#define SQUARE(a)                ((a) * (a))

// TWSR[1:0]�ATWBR[7:0]�̊e�l����AF_CPU�������������̂������߂�}�N���B
#define DIV_REG(twps, twbr)  (((twbr) * (1 << ((twps) * 2)) + 8) * 2)

const uint8_t TWPS_MAX = 3;
const uint8_t TWBR_MAX = 255;

const uint16_t DIV_MIN = DIV_REG(0, 0);               // �ŏ�16�����܂ŉ�
const uint16_t DIV_MAX = DIV_REG(TWPS_MAX, TWBR_MAX); // �ő�32656�����܂ŉ�

const uint16_t DIV_MAX0 = DIV_REG(0, TWBR_MAX); //  526�BTWPS = 0�̂Ƃ��̍ő啪����B
const uint16_t DIV_MAX1 = DIV_REG(1, TWBR_MAX); // 2056�BTWPS = 1�̂Ƃ��̍ő啪����B
const uint16_t DIV_MAX2 = DIV_REG(2, TWBR_MAX); // 8176�BTWPS = 2�̂Ƃ��̍ő啪����B

// F_CPU�Ə��]��SCL���g���Ƃ̔�����߂锟���B
uint16_t calc_div(uint32_t f_cpu, uint32_t scl_freq){
	uint32_t div = f_cpu / scl_freq;
	return (uint16_t)(CONSTRAIN(div, DIV_MIN, DIV_MAX));
}

// TWSR[1:0] (=TWPS)�̒l�����߂锟���B
uint8_t calc_twps(uint16_t div){
	uint8_t twps;
	if     (div <= DIV_MAX0){twps = 0;} // F_CPU��SCL���g���Ƃ̔䂪     �`  526�̏ꍇ
	else if(div <= DIV_MAX1){twps = 1;} // F_CPU��SCL���g���Ƃ̔䂪 528 �` 2056�̏ꍇ
	else if(div <= DIV_MAX2){twps = 2;} // F_CPU��SCL���g���Ƃ̔䂪2064 �` 8176�̏ꍇ
	else                    {twps = 3;} // F_CPU��SCL���g���Ƃ̔䂪8208 �`     �̏ꍇ
	return twps;
}

// TWBR[7:0]�̒l�����߂锟���B
uint8_t calc_twbr(uint32_t f_cpu, uint32_t scl_freq, uint8_t twps){
	scl_freq = CONSTRAIN(scl_freq, f_cpu / DIV_MAX, f_cpu / DIV_MIN);
	return (uint8_t)((f_cpu - (scl_freq * 16)) / ((scl_freq * SQUARE(1 << twps)) * 2));
}

// TWSR[1:0] (=TWPS)�ATWBR[7:0]�����������邽�߂̔����B
void set_SCL_freq(uint32_t f_cpu, uint32_t scl_freq){
	uint16_t div = calc_div(f_cpu, scl_freq); // ���]�̕���������߂āA
	uint8_t twps = calc_twps(div);            // TWSR[1:0] (=TWPS)�̒l�����߂āA
	TWSR = (TWSR & ~0b11) | twps;             // ���̒l��TWSR[1:0]�����������āA
	TWBR = calc_twbr(f_cpu, scl_freq, twps);  // TWBR���W�X�^������������B
}

#endif