/*
�d��3:0�̃^�b�`��Ԃ��ω����邽�тɁA���̃^�b�`��Ԃ�ATmega328P��PB3:0����o�͂���ƂƂ��ɁA
���܂��܂ȃp�����[�^�[��[���ɕ\������v���O�����B 
*/

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include "USARTClass.h"

#include "MPR121Class.h"
MPR121Class pad(0x5A); // (I2C�X���[�u�A�h���X)�Ŏ��̉�����B

const uint8_t n = 4; // �d�ɂ�n�{�g��(�d��3:0���^�b�`�Z���T�[�ɂ���)�B

ISR(INT1_vect){                            // �d�ɂ̏�Ԃ��ω�������A
	PORTB = pad.get_touch_status() & 0x0F; // �d��3:0�̏�Ԃ�ATmega238P��PB3:0����o�͂��A 
	pad.print_touch_status();              // �S�`�����l���̃^�b�`��Ԃ�[���ɕ\�����A
	pad.print_filtered_data(n);            // (n)�{�̊e�`�����l���̓d�Ƀf�[�^��[���ɕ\�����A
	pad.print_baseline_val(n);             // (n)�{�̊e�`�����l���̃x�[�X���C���l��[���ɕ\������B
}

int main(){
	usart.init(); // �{�[���[�g��9600�B���s�L����"\n"�B
	
	DDRB  = 0x0F; // PB3:0��IO��OUT�ɂ��Ă����B

	EIMSK |= (1 << INT1);  // INT1 (�O�����荞�ݗv��1)��L��������B
	EICRA |= (1 << ISC11); // falling edge�Ŋ��荞�ށB
	sei();                 // �O���[�o�����荞�݂�L��������B

	usart.printString("\n**********************************");
	usart.printString("\n********** TOUCH SENSOR **********");
	usart.printString("\n**********************************\n");
	
	// �X���[�u�A�h���X��[���ɕ\������B
	usart.printString("Slave Address: ");
	pad.print_slave_address();
	usart.printString("\n");
	
	// �\�t�g���Z�b�g��������B
	pad.soft_reset();
	
	// �x�[�X���C���t�B���^�[�̊e��p�����[�^�[��ݒ肷��B(�ݒ肵�Ȃ��Ă��悢)
	pad.set_mhd_r(10); // rising����MHD��ݒ肷��B�l�͈̔͂�1�`63�B
	pad.set_nhd_r(5);  // rising����NHD��ݒ肷��B�l�͈̔͂�1�`63�B
	pad.set_ncl_r(10); // rising����NCL��ݒ肷��B�l�͈̔͂�0�`255�B��ʂ�falling����������(������)����B
	pad.set_fdl_r(0);  // rising����FDL��ݒ肷��B�l�͈̔͂�0�`255�B
	pad.set_mhd_f(15); // falling����MHD��ݒ肷��B�l�͈̔͂�1�`63�B
	pad.set_nhd_f(5);  // falling����NHD��ݒ肷��B�l�͈̔͂�1�`63�B
	pad.set_ncl_f(10); // falling����NCL��ݒ肷��B�l�͈̔͂�0�`255�B
	pad.set_fdl_f(90); // falling����FDL��ݒ肷��B�l�͈̔͂�0�`255�B��ʂ�rising�������x��(�傫��)����B
	pad.set_nhd_t(1);  // touched����NHD��ݒ肷��B�l�͈̔͂�1�`63�B
	pad.set_ncl_t(1);  // touched����NCL��ݒ肷��B�l�͈̔͂�0�`255�B
	pad.set_fdl_t(2);  // touched����FDL��ݒ肷��B�l�͈̔͂�0�`255�B
	
	// �x�[�X���C���t�B���^�[�̊e��p�����[�^�[�̐ݒ�l��[���ɕ\������B
	uint8_t buff[11];
	pad.print_touch_filter_parameters(buff);

	// ����(��)�R���t�B�M�����[�V�����̐ݒ������B(�ݒ肵�Ȃ��Ă��悢)
	pad.set_up_side_limit(202);      // 0�`255
	pad.set_low_side_limit(117);     // 0�`255
	pad.set_target_level(181);       // 0�`255
	pad.set_auto_config(6, 4, 1, 1,  // (ffi=6,10,18 or 34��; retry=0,2,4 or 8��; are=1 or 0; ace=1 or 0
                        0, 1, 1, 1); //  scts=1 or 0; oorie=1 or 0; arfie=1 or 0; acfie=1 or 0); 

	// �x�[�X���C���g���b�L���O�̐ݒ������B(�ݒ肵�Ȃ��Ă��悢)
	// 0: ���݂̃x�[�X���C���������l�Ƃ���B(���Z�b�g���0�ł��邽�߁A�x�[�X���C�������݂̓d�Ƀf�[�^�ɒǂ����܂ŒZ���Ԃ����������Ȃ�)
	// 1: �x�[�X���C���g���b�L���O�𖳌�������B
	// 2: �ŏ��̑���l�̏��5�r�b�g���x�[�X���C���̏����l�Ƃ���B
	// 3: �ŏ��̑���l�̏��10�r�b�g���x�[�X���C���̏����l�Ƃ���B
	pad.set_baseline_tracking(2);
	
	// �^�b�`���̃f�B�o�E���X��(0�`7)��ݒ肷��B(�ݒ肵�Ȃ��Ă��悢)
	pad.set_touch_debounce_num(4);
	
	 // �����[�X���̃f�B�o�E���X��(0�`7)��ݒ肷��B(�ݒ肵�Ȃ��Ă��悢)
	pad.set_release_debounce_num(3);
	
	// (�`�����l��)�Ƀ^�b�`臒l(0�`255)��ݒ肷��B(�ݒ肵�Ȃ��Ă��悢)
	// ��ʂɃ����[�X臒l�����傫�����ăq�X�e���V�X��������������B
	for(int ch=0; ch<n; ch++) pad.set_touch_threshold(ch, 15);
	
	// (�`�����l��)�Ƀ����[�X臒l(0�`255)��ݒ肷��B(�ݒ肵�Ȃ��Ă��悢)
	// ��ʂɃ^�b�`臒l�������������ăq�X�e���V�X��������������B
	for(int ch=0; ch<n; ch++) pad.set_release_threshold(ch, 5);
	
	// (n)�{�̓d�ɂ��^�b�`�Z���T�[�Ƃ��ėL��������B(�K���L�������Ȃ���΂Ȃ�Ȃ�)
	pad.enable_touch(n);

	// ����(��)�R���t�B�M�����[�V�����͈̔͊O�X�e�[�^�X��[���ɕ\������B
	pad.print_out_of_range_status_status();
	
	// �����R���t�B�M�����[�V�����Ɏ��s�������ǂ�����[���ɕ\������B
	pad.print_auto_config_failed();

	// (n)�{�̃`�����l���̃^�b�`臒l�A�����[�X臒l�̗�����[���ɕ\������B
	pad.print_both_threshold(n);

	// �^�b�`�A�����[�X�����̃f�B�o�E���X�񐔂�[���ɕ\������B
	pad.print_both_debounce_num();
	
	// �t�B���^�[�̃T���v�����O�񐔁A�O���[�o��CDC�l�A�O���[�o��CDT�l��[���ɕ\������B
	pad.print_filter_global_cdc_cdt();

	// (n)�{�̊e�`�����l���̏[���d�d���l�ݒ�l��[���ɕ\������B
	pad.print_cdc(n);
	
	// (n)�{�̊e�`�����l���̊e�[���d���Ԑݒ�l(10�{�l)��[���ɕ\������B
	pad.print_cdt10(n);


	while(1);
	return 0;
}
