#ifndef MPR121CLASS_H_
#define MPR121CLASS_H_

#include <avr/io.h>
#include "USARTClass.h"
#include "I2CClass.h"

// Register Addresses and Others
const uint8_t OOR_STATUS           = 0x02;
const uint8_t FILTERED_DATA        = 0x04;
const uint8_t BASELINE_VALUE       = 0x1E;

const uint8_t TOUCH_THRESHOLD      = 0x41;
const uint8_t RELEASE_THRESHOLD    = 0x42;
const uint8_t DEBOUNCE             = 0x5B;

const uint8_t FILTER1_GLOBAL_CDC   = 0x5C;
const uint8_t FILTER2_GLOBAL_CDT   = 0x5D;

const uint8_t ECR                  = 0x5E;

const uint8_t ELECTRODE_CURRENT    = 0x5F;
const uint8_t CHARGE_TIME          = 0x6C;

const uint8_t GPIO_CONTROL_0       = 0x73;
const uint8_t GPIO_CONTROL_1       = 0x74;
const uint8_t GPIO_DATA            = 0x75;
const uint8_t GPIO_DIRECTION       = 0x76;
const uint8_t GPIO_ENABLE          = 0x77;
const uint8_t GPIO_DATA_SET        = 0x78;
const uint8_t GPIO_DATA_CLEAR      = 0x79;
const uint8_t GPIO_DATA_TOGGLE     = 0x7A;

const uint8_t AUTOCONFIG_CONTROL_0 = 0x7B;
const uint8_t UP_SIDE_LIMIT        = 0x7D;
const uint8_t LOW_SIDE_LIMIT       = 0x7E;
const uint8_t TARGET_LEVEL         = 0x7F;

const uint8_t SOFT_RESET           = 0x80;

const uint8_t PWM_0                = 0x81;

const uint8_t MHD_RISING           = 0x2B;
const uint8_t PROX_MHD_RISING      = 0x36;

const uint8_t SOFT_RESET_VAl = 0x63; // �\�t�g���Z�b�g���W�X�^�ɏ������ޒl

const uint8_t NORMAL_IN = 0, PULL_DOWN = 2, PULL_UP = 3; // IN���̉�H�\��
const uint8_t PUSH_PULL = 0, SINK      = 2, SOURCE  = 3; // OUT���̉�H�\��

 
class MPR121Class{
	private:
	uint8_t _SLA; // �X���[�u�A�h���X�B
	uint8_t _CL;  // calibration lock
	uint8_t  get_ECR(); // ECR���W�X�^��ǂݏo���B

	public:
	uint8_t get_slave_address(); // �X���[�u�A�h���X���擾����B
	
	void soft_reset(); // �\�t�g���Z�b�g��������B

	// �x�[�X���C���g���b�L���O��L�����A����������B
	// cl=0: �f�t�H���g�B���̂Ƃ��̃x�[�X���C���l�������x�[�X���C���l�Ƃ��ă��[�h����B
	// cl=1: �x�[�X���C���g���b�L���O�𖳌�������B
	// cl=2: ����d�Ƀf�[�^�̏��5�r�b�g�������x�[�X���C���l�Ƃ��ă��[�h����B
	// cl=3: ����d�Ƀf�[�^�̏��10�r�b�g�������x�[�X���C���l�Ƃ��ă��[�h����B
	void set_baseline_tracking(uint8_t cl); 

	uint16_t get_touch_status();      // 13ch�Ԃ�S���̃^�b�`���(ch13�͋ߐڏ��)���擾����B

	void     enable_touch(uint8_t n); // (n)�{�̓d�ɂ��^�b�`�Z���T�[�ɂ���Bn=0�`12�B
	void     enable_prox(uint8_t n);  // (n)�{�̓d�ɂ��ߐڃZ���T�[�ɂ���Bn=0, 2, 4, or 12�B
	void     enter_stop_mode();       // STOP���[�h�Ɉڍs����B

	uint16_t get_filtered_data(uint8_t ch);                    // (�e�`�����l��)�̓d�Ƀf�[�^(10�r�b�g)���擾����B
	void     get_filtered_data(uint8_t n, uint16_t*buff_word); // (n)�{�̃`�����l���̓d�Ƀf�[�^(10�r�b�g)���o�b�t�@�[�Ɋi�[����B

	uint16_t get_baseline_val(uint8_t ch);                    // (�e�`�����l��)�̃x�[�X���C���l(10�r�b�g�A��2�r�b�g�͖���)���擾����B
	void     get_baseline_val(uint8_t n, uint16_t*buff_word); // (n)�{�̃`�����l���̃x�[�X���C���l(10�r�b�g�A��2�r�b�g�͖���)���o�b�t�@�[�Ɋi�[����B

	uint8_t get_touch_threshold(uint8_t ch);              // (�e�`�����l��)�̃^�b�`臒l���擾����B

	void    get_touch_threshold(uint8_t n, uint8_t*buff); // (n)�{�̃`�����l���̃^�b�`臒l���o�b�t�@�[�Ɋi�[����B
	void    set_touch_threshold(uint8_t ch, uint8_t val); // (�e�`�����l��)�̃^�b�`臒l(0�`255)���Z�b�g����B

	uint8_t get_release_threshold(uint8_t ch);              // (�e�`�����l��)�̃����[�X臒l���擾����B
	void    get_release_threshold(uint8_t n, uint8_t*buff); // (n)�{�̃`�����l���̃����[�X臒l���o�b�t�@�[�Ɋi�[����B

	void    set_release_threshold(uint8_t ch, uint8_t val); // (�e�`�����l��)�̃����[�X臒l(0�`255)���Z�b�g����B

	void get_both_threshold(uint8_t n, uint8_t*touch_buff, uint8_t*release_buff); // (n)�{�̃`�����l���̃^�b�`臒l�A�����[�X臒l����x�Ƀo�b�t�@�[�Ɋi�[����B

	uint8_t get_touch_debounce_num();   // �^�b�`�f�B�o�E���X�񐔂��擾����B�S�`�����l�����ʁB
	uint8_t get_release_debounce_num(); // �����[�X�f�B�o�E���X�񐔂��擾����B�S�`�����l�����ʁB

	void set_touch_debounce_num(uint8_t num);   // �^�b�`�f�B�o�E���X��(0�`7)���Z�b�g����B�S�`�����l�����ʁB
	void set_release_debounce_num(uint8_t num); // �����[�X�f�B�o�E���X��(0�`7)���Z�b�g����B�S�`�����l�����ʁB

	uint8_t get_ffi();          // 1���t�B���^�[�̃T���v�����O�񐔁B�Ԓl:6,10,18,34��(�Ή����郌�W�X�^�l��0�`3)�B
	uint8_t get_gloval_cdc();   // ���ʂ̏[���d�d���ݒ�l�B�Ԓl:����0�`63 (uA)�B
	uint8_t get_gloval_cdt10(); // ���ʂ̏[���d���Ԑݒ�l�B�Ԓl:10 * 2^(n-2) (us) (�Ή����郌�W�X�^�l0�`7)�B�����̂܂܈��������̂�10�{�l��Ԃ��B
	uint8_t get_sfi();          // 2���t�B���^�[�̃T���v�����O�񐔁B�Ԓl:4,6,10,18��(�Ή����郌�W�X�^�l��0�`3)�B
	uint8_t get_esi();          // 2���t�B���^�[�ł̃T���v�����O�Ԋu�ݒ�l�B�Ԓl:2^n (ms) (�Ή����郌�W�X�^�l��0�`7)�B

	void get_cdc(uint8_t n, uint8_t*buff);   // (n)�{�̊e�`�����l���̏[���d�d���ݒ�l���o�b�t�@�[�Ɋi�[����B�Ԓl:����0�`63 uA�B
	void get_cdt10(uint8_t n, uint8_t*buff); // (n)�{�̊e�`�����l���̏[���d���Ԑݒ�l���o�b�t�@�[�Ɋi�[����B�Ԓl:10 * 2^(n-2) us (�Ή����郌�W�X�^�l��0�`7)�B�����̂܂܈��������̂�10�{�l��Ԃ��B

	
	// �����R���t�B�M�����[�V�������W�X�^�̐ݒ������B
	// ffi   = 1���t�B���^�[�̃T���v�����O�񐔁B6,10,18,34��̂����ꂩ�Ŏw�肷��(�Ή����郌�W�X�^�l��0�`3)�B
	// retry = ���g���C�񐔁B0,2,4,8��̂����ꂩ�Ŏw�肷��(�Ή����郌�W�X���l��0�`3)�B
	// are   = �����ăR���t�B�M�����[�V���������s����B1: �L���A0: �����B
	// ace   = STOP �� RUN���[�h�ڍs���Ɏ����R���t�B�M�����[�V���������s����B1: �L���A0: �����B
	// scts  = �[�d���Ԍ������ȗ�����B1: �ȗ�����A0: �ȗ����Ȃ��B
	// oorie = �͈͊O���荞�ݗL�����B1: �L���A0: �����B
	// arfie = �����ăR���t�B�M�����[�V�������s���荞�ݗL�����B1: �L���A0: �����B
	// acfie = �����R���t�B�M�����[�V�������s���荞�ݗL�����B1: �L���A0: �����B
	void set_auto_config(uint8_t ffi, uint8_t retry, uint8_t are, uint8_t ace,
                         uint8_t scts, uint8_t oorie, uint8_t arfie, uint8_t acfie);
	
	void set_up_side_limit (uint8_t usl); // ����(��)�R���t�B�M�����[�V�������̓d�Ƀf�[�^�̏���B0�`255�B
	void set_low_side_limit(uint8_t lsl); // ����(��)�R���t�B�M�����[�V�������̓d�Ƀf�[�^�̉����B0�`255�B
	void set_target_level  (uint8_t  tl); // ����(��)�R���t�B�M�����[�V�������̓d�Ƀf�[�^�̖ڕW�l�B0�`255�B
	
	uint16_t get_out_of_range_status(); // ����(��)�R���t�B�M�����[�V�����͈̔͊O�X�e�[�^�X�擾����B
	uint8_t auto_config_failed();       // �����R���t�B�M�����[�V�����Ɏ��s�������ǂ������擾����B
	uint8_t auto_reconfig_failed();     // �����ăR���t�B�M�����[�V�����Ɏ��s�������ǂ������擾����B

	// MHD Rising , NHD Rising , NCL Rising , FDL Rising, 
	// MHD Falling, NHD Falling, NCL Falling, FDL Falling,
	//              NHD Touched, NCL Touched, FDL Touched�̏��Ԃ�11�o�C�g���o�b�t�@�[�Ɋi�[����B
	void get_touch_filter_parameters(uint8_t*buff);
	
	// PROX MHD Rising , PROX NHD Rising , PROX NCL Rising , PROX FDL Rising,
	// PROX MHD Falling, PROX NHD Falling, PROX NCL Falling, PROX FDL Falling,
	//                   PROX NHD Touched, PROX NCL Touched, PROX FDL Touched�̏��Ԃ�11�o�C�g���o�b�t�@�[�Ɋi�[����B
	void get_prox_filter_parameters(uint8_t*buff);
	
	void set_mhd_r(uint8_t val); // rising����MHD��ݒ肷��B�l�͈̔͂�1�`63�B
	void set_nhd_r(uint8_t val); // rising����NHD��ݒ肷��B�l�͈̔͂�1�`63�B
	void set_ncl_r(uint8_t val); // rising����NCL��ݒ肷��B�l�͈̔͂�0�`255�B
	void set_fdl_r(uint8_t val); // rising����FDL��ݒ肷��B�l�͈̔͂�0�`255�B
	
	void set_mhd_f(uint8_t val); // falling����MHD��ݒ肷��B�l�͈̔͂�1�`63�B
	void set_nhd_f(uint8_t val); // falling����NHD��ݒ肷��B�l�͈̔͂�1�`63�B
	void set_ncl_f(uint8_t val); // falling����NCL��ݒ肷��B�l�͈̔͂�0�`255�B
	void set_fdl_f(uint8_t val); // falling����FDL��ݒ肷��B�l�͈̔͂�0�`255�B
	
	void set_nhd_t(uint8_t val); // touched����NHD��ݒ肷��B�l�͈̔͂�1�`63�B
	void set_ncl_t(uint8_t val); // touched����NCL��ݒ肷��B�l�͈̔͂�0�`255�B
	void set_fdl_t(uint8_t val); // touched����FDL��ݒ肷��B�l�͈̔͂�0�`255�B
	
	void set_mhd_prox_r(uint8_t val); // prox rising����MHD��ݒ肷��B�l�͈̔͂�1�`63�B
	void set_nhd_prox_r(uint8_t val); // prox rising����NHD��ݒ肷��B�l�͈̔͂�1�`63�B
	void set_ncl_prox_r(uint8_t val); // prox rising����NCL��ݒ肷��B�l�͈̔͂�0�`255�B
	void set_fdl_prox_r(uint8_t val); // prox rising����FDL��ݒ肷��B�l�͈̔͂�0�`255�B
	
	void set_mhd_prox_f(uint8_t val); // prox falling����MHD��ݒ肷��B�l�͈̔͂�1�`63�B
	void set_nhd_prox_f(uint8_t val); // prox falling����NHD��ݒ肷��B�l�͈̔͂�1�`63�B
	void set_ncl_prox_f(uint8_t val); // prox falling����NCL��ݒ肷��B�l�͈̔͂�0�`255�B
	void set_fdl_prox_f(uint8_t val); // prox falling����FDL��ݒ肷��B�l�͈̔͂�0�`255�B
	
	void set_nhd_proxr_t(uint8_t val); // prox touched����NHD��ݒ肷��B�l�͈̔͂�1�`63�B
	void set_ncl_proxr_t(uint8_t val); // prox touched����NCL��ݒ肷��B�l�͈̔͂�0�`255�B
	void set_fdl_proxr_t(uint8_t val); // prox touched����FDL��ݒ肷��B�l�͈̔͂�0�`255�B
	

	//////////////////////
	/* GPIO�֘A�̔����Q */
	//////////////////////
	void enable_gpio       (uint8_t led_7_0);   // GPIO��L��������B1: �L��, 0; �����B
	void set_gpio_direction(uint8_t led_7_0=0); // IO�̌������w�肷��B1:OUT, 0:IN�B
	void write_gpio_data   (uint8_t led_7_0);   // GPIO�f�[�^���W�X�^������������B
	void set_gpio_data     (uint8_t led_7_0);   // �w�肵���r�b�g��1�ɃZ�b�g����B
	void clear_gpio_data   (uint8_t led_7_0);   // �w�肵���r�b�g���N���A����B
	void toggle_gpio_data  (uint8_t led_7_0);   // �w�肵���r�b�g���g�O������B
	
	
	// GPIO IN�� : circuit = NORMAL_IN, PULL_DOWN, PULL_UP�̂����ꂩ�B
	// GPIO OUT��: circuit = PUSH_PULL, SINK     , SOURCE�̂����ꂩ�B
	void set_gpio_circuit(uint8_t led_n, uint8_t circuit);
	
	uint8_t read_gpio_data(); // GPIO�f�[�^���W�X�^��ǂݏo���B
	
	void analog_write(uint8_t led_n, uint8_t duty); // (led�s���ԍ�, duty=16���̂�����)


	///////////////////////////////////
	/* �[���ɉ�����\�����邽�߂̔����Q */
	///////////////////////////////////
	void print_slave_address();                       // �X���[�u�A�h���X��[���ɕ\�����锟���B
	void print_touch_status();                        // �S�`�����l���̃^�b�`�X�e�[�^�X��[���ɕ\�����锟���B
	void print_filtered_data(uint8_t n);              // (n)�{�̃`�����l���̓d�Ƀf�[�^��[���ɕ\�����锟���B
	void print_baseline_val(uint8_t n);               // (n)�{�̃`�����l���̃x�[�X���C���l��[���ɕ\�����锟���B
	void print_touch_threshold(uint8_t n);            // (n)�{�̃`�����l���̃^�b�`臒l��[���ɕ\�����锟���B
	void print_release_threshold(uint8_t n);          // (n)�{�̃`�����l���̃����[�X臒l��[���ɕ\�����锟���B
	void print_both_threshold(uint8_t n);             // (n)�{�̃`�����l���̃^�b�`臒l�A�����[�X臒l�̗�����[���ɕ\�����锟���B
	void print_both_debounce_num();                   // �^�b�`�A�����[�X�����̃f�B�o�E���X��(�S�`�����l������)��[���ɕ\�����锟���B
	void print_filter_global_cdc_cdt();               // �t�B���^�[�̃T���v�����O�񐔁A�O���[�o��CDC�l�A�O���[�o��CDT�l��[���ɕ\�����锟���B
	void print_cdc(uint8_t n);                        // (n)�{�̊e�`�����l���̏[���d�d���ݒ�l��[���ɕ\�����锟���B
	void print_cdt10(uint8_t n);                      // (n)�{�̊e�`�����l���̏[���d�����ݒ�l(10�{�l)��[���ɕ\�����锟���B
	void print_out_of_range_status_status();          // ����(��)�R���t�B�M�����[�V���������s�������ʁA�͈͊O�ɂȂ������ǂ�����[���ɕ\�����锟���B
	void print_auto_config_failed();                  // �����R���t�B�M�����[�V�����Ɏ��s�������ǂ�����[���ɕ\�����锟���B
	void print_auto_reconfig_failed();                // �����ăR���t�B�M�����[�V�����Ɏ��s�������ǂ�����[���ɕ\�����锟���B
	void print_touch_filter_parameters(uint8_t*buff); // �^�b�`�Z���T�[�̃t�B���^�����O�p�����[�^�[��\�����锟���B
	void print_prox_filter_parameters(uint8_t*buff);  // �ߐڃZ���T�[�̃t�B���^�����O�p�����[�^�[��\�����锟���B
	///////////////////////////////////
	

	// �R���X�g���N�^
	MPR121Class(uint8_t slave_addr);
};

#endif
