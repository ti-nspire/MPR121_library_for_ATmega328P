#ifndef SET_SCL_FREQ_H
#define SET_SCL_FREQ_H

#include <avr/io.h>

#define MAX(a, b)                (((a) > (b)) ? (a) : (b))
#define MIN(a, b)                (((a) < (b)) ? (a) : (b))
#define CONSTRAIN(val, min, max) (MIN((MAX((val), (min))), (max)))
#define SQUARE(a)                ((a) * (a))

// TWSR[1:0]、TWBR[7:0]の各値から、F_CPUが何分周されるのかを求めるマクロ。
#define DIV_REG(twps, twbr)  (((twbr) * (1 << ((twps) * 2)) + 8) * 2)

const uint8_t TWPS_MAX = 3;
const uint8_t TWBR_MAX = 255;

const uint16_t DIV_MIN = DIV_REG(0, 0);               // 最小16分周まで可
const uint16_t DIV_MAX = DIV_REG(TWPS_MAX, TWBR_MAX); // 最大32656分周まで可

const uint16_t DIV_MAX0 = DIV_REG(0, TWBR_MAX); //  526。TWPS = 0のときの最大分周比。
const uint16_t DIV_MAX1 = DIV_REG(1, TWBR_MAX); // 2056。TWPS = 1のときの最大分周比。
const uint16_t DIV_MAX2 = DIV_REG(2, TWBR_MAX); // 8176。TWPS = 2のときの最大分周比。

// F_CPUと所望のSCL周波数との比を求める函数。
uint16_t calc_div(uint32_t f_cpu, uint32_t scl_freq){
	uint32_t div = f_cpu / scl_freq;
	return (uint16_t)(CONSTRAIN(div, DIV_MIN, DIV_MAX));
}

// TWSR[1:0] (=TWPS)の値を求める函数。
uint8_t calc_twps(uint16_t div){
	uint8_t twps;
	if     (div <= DIV_MAX0){twps = 0;} // F_CPUとSCL周波数との比が     ～  526の場合
	else if(div <= DIV_MAX1){twps = 1;} // F_CPUとSCL周波数との比が 528 ～ 2056の場合
	else if(div <= DIV_MAX2){twps = 2;} // F_CPUとSCL周波数との比が2064 ～ 8176の場合
	else                    {twps = 3;} // F_CPUとSCL周波数との比が8208 ～     の場合
	return twps;
}

// TWBR[7:0]の値を求める函数。
uint8_t calc_twbr(uint32_t f_cpu, uint32_t scl_freq, uint8_t twps){
	scl_freq = CONSTRAIN(scl_freq, f_cpu / DIV_MAX, f_cpu / DIV_MIN);
	return (uint8_t)((f_cpu - (scl_freq * 16)) / ((scl_freq * SQUARE(1 << twps)) * 2));
}

// TWSR[1:0] (=TWPS)、TWBR[7:0]を書き換えるための函数。
void set_SCL_freq(uint32_t f_cpu, uint32_t scl_freq){
	uint16_t div = calc_div(f_cpu, scl_freq); // 所望の分周比を求めて、
	uint8_t twps = calc_twps(div);            // TWSR[1:0] (=TWPS)の値を求めて、
	TWSR = (TWSR & ~0b11) | twps;             // その値でTWSR[1:0]を書き換えて、
	TWBR = calc_twbr(f_cpu, scl_freq, twps);  // TWBRレジスタも書き換える。
}

#endif