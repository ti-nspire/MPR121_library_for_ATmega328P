#include "MPR121Class.h"

uint8_t MPR121Class::get_slave_address(){
	return _SLA;
}
void MPR121Class::soft_reset(){
	i2c.write(_SLA, SOFT_RESET, SOFT_RESET_VAl);
}
void MPR121Class::set_baseline_tracking(uint8_t cl){
	_CL = cl;
	uint8_t ecr = (get_ECR() & ~(0b11 << 6)) | (_CL << 6); // ECRを読み出して上位2ビットを消してそこだけ書き換える。
	i2c.write(_SLA, ECR, ecr);
}
uint16_t MPR121Class::get_touch_status(){
	uint8_t buff[2];
	i2c.read(_SLA, 0x00, 2, buff);
	return ((uint16_t)(buff[1] & 0b11111) << 8) | buff[0]; // 上位5ビット + 下位8ビットに並べ換えて返す。
}
uint8_t MPR121Class::get_ECR(){
	return i2c.read(_SLA, ECR);
}
void MPR121Class::enable_touch(uint8_t n){
	uint8_t ecr = (get_ECR() & 0xF0) | n; // ECRを読み出して下4ビットを消してそこだけ書き換える。
	i2c.write(_SLA, ECR, ecr);
}
void MPR121Class::enable_prox(uint8_t n){
	uint8_t ele_prox_en;
	switch(n){
		case  0: ele_prox_en = 0; break;
		case  2: ele_prox_en = 1; break;
		case  4: ele_prox_en = 2; break;
		case 12: ele_prox_en = 3; break;
		default: ele_prox_en = 3; break;
	}
	uint8_t ecr = (get_ECR() & ~(0b11 << 4)) | (ele_prox_en << 4); // ECRを読み出してビット[5:4]を消してそこだけ書き換える。
	i2c.write(_SLA, ECR, ecr);
}
void MPR121Class::enter_stop_mode(){
	enable_touch(0);
	enable_prox(0);
}
uint16_t MPR121Class::get_filtered_data(uint8_t ch){
	uint8_t buff[2];
	i2c.read(_SLA, FILTERED_DATA + 2 * ch, 2, buff);
	return ((uint16_t)(buff[1] & 0b11) << 8) | buff[0]; // 上位2ビット + 下位8ビットに並べ換えて返す。
}
void MPR121Class::get_filtered_data(uint8_t n, uint16_t*buff_word){
	/*
	for(int i=0; i<n; i++){
		buff_word[i] = get_filtered_data(i);
	}
	*/
	uint8_t buff_byte[2*n];
	i2c.read(_SLA, FILTERED_DATA, 2*n, buff_byte);
	for(int i=0; i<n; i++){
		buff_word[i] = ((uint16_t)(buff_byte[i*2+1] & 0b11) << 8) | buff_byte[i*2];
	}
}
uint16_t MPR121Class::get_baseline_val(uint8_t ch){
	return (uint16_t)i2c.read(_SLA, BASELINE_VALUE + ch) << 2; // 電極データと比較しやすいよう10ビット値にして返す。
}
void MPR121Class::get_baseline_val(uint8_t n, uint16_t*buff_word){
	/*
	for(int i=0; i<n; i++){
		buff_word[i] = get_baseline_val(i);
	}
	*/
	uint8_t buff_byte[n];
	i2c.read(_SLA, BASELINE_VALUE, n, buff_byte);	
	for(int i=0; i<n; i++){
		buff_word[i] = (uint16_t)buff_byte[i] << 2; // 電極データと比較しやすいよう10ビット値にしてバッファーに格納する。
	}
}
uint8_t MPR121Class::get_touch_threshold(uint8_t ch){
	return i2c.read(_SLA, TOUCH_THRESHOLD + 2 * ch);	
}
void MPR121Class::get_touch_threshold(uint8_t n, uint8_t*buff){
	for(int i=0; i<n; i++){
		buff[i] = get_touch_threshold(i);
	}
}
void MPR121Class::set_touch_threshold(uint8_t ch, uint8_t val){
	uint8_t ecr = get_ECR();                        // ECRの内容を取っておいて、
	enter_stop_mode();                              // STOPモードにして、
	i2c.write(_SLA, TOUCH_THRESHOLD + 2 * ch, val); // タッチ閾値を書き換えて、
	i2c.write(_SLA, ECR, ecr);                      // ECRを書き戻してRUNモードに戻す。
}
uint8_t MPR121Class::get_release_threshold(uint8_t ch){
	return i2c.read(_SLA, RELEASE_THRESHOLD + 2 * ch);
}
void MPR121Class::get_release_threshold(uint8_t n, uint8_t*buff){
	for(int i=0; i<n; i++){
		buff[i] = get_release_threshold(i);
	}
}
void MPR121Class::set_release_threshold(uint8_t ch, uint8_t val){
	uint8_t ecr = get_ECR();                          // ECRの内容を取っておいて、
	enter_stop_mode();                                // STOPモードにして、
	i2c.write(_SLA, RELEASE_THRESHOLD + 2 * ch, val); // リリース閾値を書き換えて、
	i2c.write(_SLA, ECR, ecr);                        // ECRを書き戻してRUNモードに戻す。
}
void MPR121Class::get_both_threshold(uint8_t n, uint8_t*touch_buff, uint8_t*release_buff){
	uint8_t both_buff[n*2];
	i2c.read(_SLA, TOUCH_THRESHOLD, (uint16_t)n*2, both_buff);

	uint16_t t = 0;
	uint16_t r = 0;
	for(int i=0; i<(n*2); i++){
		if     (i%2==0){  touch_buff[t] = both_buff[i]; t++;}
		else if(i%2==1){release_buff[r] = both_buff[i]; r++;}
	}
}
uint8_t MPR121Class::get_touch_debounce_num(){
	return i2c.read(_SLA, DEBOUNCE) & 0b111;
}
uint8_t MPR121Class::get_release_debounce_num(){
	return (i2c.read(_SLA, DEBOUNCE) >> 4) & 0b111;
}
void MPR121Class::set_touch_debounce_num(uint8_t num){
	uint8_t ecr = get_ECR();                                // ECRの内容を取っておいて、
	enter_stop_mode();                                      // STOPモードにして、
	uint8_t dt = (i2c.read(_SLA, DEBOUNCE) & ~0b111) | num; // ディバウンスレジスタの下3ビットだけを
	i2c.write(_SLA, DEBOUNCE, dt);                          // 書き換えて、
	i2c.write(_SLA, ECR, ecr);                              // ECRを書き戻してRUNモードに戻す。
}
void MPR121Class::set_release_debounce_num(uint8_t num){
	uint8_t ecr = get_ECR();                                             // ECRの内容を取っておいて、
	enter_stop_mode();                                                   // STOPモードにして、
	uint8_t dr = (i2c.read(_SLA, DEBOUNCE) & ~(0b111 << 4)) | (num <<4); // ディバウンスレジスタのビット[6:4]だけを
	i2c.write(_SLA, DEBOUNCE, dr);                                       // 書き換えて、
	i2c.write(_SLA, ECR, ecr);                                           // ECRを書き戻してRUNモードに戻す。
}
uint8_t MPR121Class::get_ffi(){
	uint8_t n;
	switch((i2c.read(_SLA, FILTER1_GLOBAL_CDC) >> 6) & 0b11){
		case 0 : n =  6; break;
		case 1 : n = 10; break;
		case 2 : n = 18; break;
		case 3 : n = 34; break;
		default: n =  0; break;
	}
	return n;
}
uint8_t MPR121Class::get_gloval_cdc(){
	return i2c.read(_SLA, FILTER1_GLOBAL_CDC) & ~(0b11 << 6);
}
uint8_t  MPR121Class::get_gloval_cdt10(){
	uint8_t cdt   = (i2c.read(_SLA, FILTER2_GLOBAL_CDT) >> 5) & 0b111;
	uint8_t cdt10 = (5 * (1 << cdt)) / 2;
	return (cdt10 < 5) ? 0 : cdt10;
}
uint8_t MPR121Class::get_sfi(){
	uint8_t n;
	switch((i2c.read(_SLA, FILTER2_GLOBAL_CDT) >> 3) & 0b11){
		case 0 : n =  4; break;
		case 1 : n =  6; break;
		case 2 : n = 10; break;
		case 3 : n = 18; break;
		default: n =  0; break;
	}
	return n;
}
uint8_t MPR121Class::get_esi(){
	return 1 << (i2c.read(_SLA, FILTER2_GLOBAL_CDT) & 0b111);
}
void MPR121Class::get_cdc(uint8_t n, uint8_t*buff){
	i2c.read(_SLA, ELECTRODE_CURRENT, n, buff);
	for(int i=0; i<n; i++){
		buff[i] &= (~(0b11 << 6));
	}
}
void MPR121Class::get_cdt10(uint8_t n, uint8_t*buff){
	uint8_t temp_buff[(n+1)/2];                      // 読み出したいチャンネル本数の半分の假バッファーを用意しておいて、
	i2c.read(_SLA, CHARGE_TIME, (n+1)/2, temp_buff); // 同居している偶数、奇数両方のチャンネルを読み出して、
	uint8_t j = 0;
	for(int i=0; i<(n+1)/2; i++){
		// CDT[偶数ch]の値をバッファーに格納する。
		uint8_t cdt_even_10 = 5 * (1 << (temp_buff[i] & 0b111)) / 2;
		buff[j] = (cdt_even_10 < 5) ? 0 : cdt_even_10;
		if(j>n){break;}
		
		// CDT[奇数ch]の値をバッファーに格納する。
		uint8_t cdt_odd_10  = 5 * (1 << ((temp_buff[i] >> 4) & 0b111)) / 2;
		buff[j+1] = (cdt_odd_10 < 5) ? 0 : cdt_odd_10;

		j+=2;
	}
}
void MPR121Class::set_auto_config(uint8_t ffi, uint8_t retry, uint8_t are, uint8_t ace,
                                  uint8_t scts, uint8_t oorie, uint8_t arfie, uint8_t acfie){
	uint8_t ffi_n; // ffi回数を、レジスタに書き込む値に変換する。
	switch(ffi){
		case  6: ffi_n = 0; break;
		case 10: ffi_n = 1; break;
		case 18: ffi_n = 2; break;
		case 34: ffi_n = 3; break;
		default: ffi_n = 0; break;
	}
	uint8_t retry_n; // retry回数を、レジスタに書き込む値に変換する。
	switch(retry){
		case 0 : retry_n = 0; break;
		case 2 : retry_n = 1; break;
		case 4 : retry_n = 2; break;
		case 8 : retry_n = 3; break;
		default: retry_n = 0; break;
	}
	uint8_t auto_config[2];
	auto_config[0] = (ffi_n << 6) | (retry_n << 4) | (_CL << 2) | (are < 1) | ace;
	auto_config[1] = (scts << 7) | (oorie << 2) | (arfie <<1) | acfie;
	i2c.write(_SLA, AUTOCONFIG_CONTROL_0, 2, auto_config);
}

void MPR121Class::set_up_side_limit (uint8_t usl){i2c.write(_SLA, UP_SIDE_LIMIT , usl);}
void MPR121Class::set_low_side_limit(uint8_t lsl){i2c.write(_SLA, LOW_SIDE_LIMIT, lsl);}
void MPR121Class::set_target_level  (uint8_t  tl){i2c.write(_SLA, TARGET_LEVEL  ,  tl);}
	
uint16_t MPR121Class::get_out_of_range_status(){
	uint8_t buff[2];
	i2c.read(_SLA, OOR_STATUS, 2, buff);
	return ((uint16_t)(buff[1] & 0b11111) << 8) | buff[0];
}
uint8_t MPR121Class::auto_config_failed(){
	return (i2c.read(_SLA, OOR_STATUS + 1) >> 7) & 1;
}
uint8_t MPR121Class::auto_reconfig_failed(){
	return (i2c.read(_SLA, OOR_STATUS + 1) >> 6) & 1;
}

void MPR121Class::enable_gpio       (uint8_t led_7_0){i2c.write(_SLA, GPIO_ENABLE     , led_7_0);}
void MPR121Class::set_gpio_direction(uint8_t led_7_0){i2c.write(_SLA, GPIO_DIRECTION  , led_7_0);}
void MPR121Class::write_gpio_data   (uint8_t led_7_0){i2c.write(_SLA, GPIO_DATA       , led_7_0);}
void MPR121Class::set_gpio_data     (uint8_t led_7_0){i2c.write(_SLA, GPIO_DATA_SET   , led_7_0);}
void MPR121Class::clear_gpio_data   (uint8_t led_7_0){i2c.write(_SLA, GPIO_DATA_CLEAR , led_7_0);}
void MPR121Class::toggle_gpio_data  (uint8_t led_7_0){i2c.write(_SLA, GPIO_DATA_TOGGLE, led_7_0);}

uint8_t MPR121Class::read_gpio_data(){return i2c.read(_SLA, GPIO_DATA);}
		
void MPR121Class::set_gpio_circuit(uint8_t led_n, uint8_t circuit){
	uint8_t cont_reg[2];
	i2c.read(_SLA, GPIO_CONTROL_0, 2, cont_reg);                                   // 一旦制御レジスタを読み出して、
	cont_reg[0] = (cont_reg[0] & ~(1 << led_n)) | (((circuit >> 1) & 1) << led_n); // 目的のビットだけを書き換えて、
	cont_reg[1] = (cont_reg[1] & ~(1 << led_n)) | ( (circuit       & 1) << led_n); // 目的のビットだけを書き換えて、
	i2c.write(_SLA, GPIO_CONTROL_0, 2, cont_reg);                                  // 制御レジスタに書き戻す。
}
void MPR121Class::analog_write(uint8_t led_n, uint8_t duty){
	uint8_t data_reg = i2c.read(_SLA, GPIO_DATA);          // 一旦GPIOデータレジスタを読み出して、
	if     (duty==0){data_reg &= ~(1 << led_n);}           // 指定されたdutyが16分の0の場合は、目的のデータレジスタビットを0にして、
	else if(duty<16){data_reg |=  (1 << led_n);}           // dutyが16分の1～15の場合は、目的のデータレジスタビットを1にして、
	else            {data_reg |=  (1 << led_n); duty = 0;} // dutyが16分の16の場合は、目的のデータレジスタビットを1にして、常時点灯状態にして、
	i2c.write(_SLA, GPIO_DATA, data_reg);                  // レジスタに書き戻す。
	
	uint8_t pwm_reg = i2c.read(_SLA, PWM_0 + led_n/2);                     // 一旦目的のPWMレジスタを読み出して、
	if     (led_n%2==0){pwm_reg = (pwm_reg & 0xF0) | ( duty & 0x0F);}      // ledピン番号が偶数の場合は下4ビットを消して目的のdutyに書き換えて、
	else if(led_n%2==1){pwm_reg = (pwm_reg & 0x0F) | ((duty & 0x0F) << 4);}// ledピン番号が奇数の場合は上4ビットを消して目的のdutyに書き換えて、
	i2c.write(_SLA, PWM_0 + led_n/2, pwm_reg);                             // レジスタに書き戻す。
}


void MPR121Class::get_touch_filter_parameters(uint8_t*buff){i2c.read(_SLA, MHD_RISING     , 11, buff);}
void MPR121Class::get_prox_filter_parameters (uint8_t*buff){i2c.read(_SLA, PROX_MHD_RISING, 11, buff);}

void MPR121Class::set_mhd_r(uint8_t val){i2c.write(_SLA, MHD_RISING, val);}
void MPR121Class::set_nhd_r(uint8_t val){i2c.write(_SLA, MHD_RISING + 1, val);}
void MPR121Class::set_ncl_r(uint8_t val){i2c.write(_SLA, MHD_RISING + 2, val);}
void MPR121Class::set_fdl_r(uint8_t val){i2c.write(_SLA, MHD_RISING + 3, val);}

void MPR121Class::set_mhd_f(uint8_t val){i2c.write(_SLA, MHD_RISING + 4, val);}
void MPR121Class::set_nhd_f(uint8_t val){i2c.write(_SLA, MHD_RISING + 5, val);}
void MPR121Class::set_ncl_f(uint8_t val){i2c.write(_SLA, MHD_RISING + 6, val);}
void MPR121Class::set_fdl_f(uint8_t val){i2c.write(_SLA, MHD_RISING + 7, val);}
	
void MPR121Class::set_nhd_t(uint8_t val){i2c.write(_SLA, MHD_RISING + 8, val);}
void MPR121Class::set_ncl_t(uint8_t val){i2c.write(_SLA, MHD_RISING + 9, val);}
void MPR121Class::set_fdl_t(uint8_t val){i2c.write(_SLA, MHD_RISING + 10, val);}
	
void MPR121Class::set_mhd_prox_r(uint8_t val){i2c.write(_SLA, PROX_MHD_RISING, val);}
void MPR121Class::set_nhd_prox_r(uint8_t val){i2c.write(_SLA, PROX_MHD_RISING + 1, val);}
void MPR121Class::set_ncl_prox_r(uint8_t val){i2c.write(_SLA, PROX_MHD_RISING + 2, val);}
void MPR121Class::set_fdl_prox_r(uint8_t val){i2c.write(_SLA, PROX_MHD_RISING + 3, val);}

void MPR121Class::set_mhd_prox_f(uint8_t val){i2c.write(_SLA, PROX_MHD_RISING + 4, val);}
void MPR121Class::set_nhd_prox_f(uint8_t val){i2c.write(_SLA, PROX_MHD_RISING + 5, val);}
void MPR121Class::set_ncl_prox_f(uint8_t val){i2c.write(_SLA, PROX_MHD_RISING + 6, val);}
void MPR121Class::set_fdl_prox_f(uint8_t val){i2c.write(_SLA, PROX_MHD_RISING + 7, val);}
	
void MPR121Class::set_nhd_proxr_t(uint8_t val){i2c.write(_SLA, PROX_MHD_RISING + 8, val);}
void MPR121Class::set_ncl_proxr_t(uint8_t val){i2c.write(_SLA, PROX_MHD_RISING + 9, val);}
void MPR121Class::set_fdl_proxr_t(uint8_t val){i2c.write(_SLA, PROX_MHD_RISING + 10, val);}

///////////////////////////////////
/* 端末に何かを表示するための函数群 */
///////////////////////////////////

// スレーブアドレスを端末に表示する函数。
void MPR121Class::print_slave_address(){
	usart.printHexByte(get_slave_address());
}
// 全チャンネルのタッチステータスを端末に表示する函数。
void MPR121Class::print_touch_status(){
	usart.printString("Touch (Prox) Status[12:0]: ");
	usart.printBinaryWord(get_touch_status());
	usart.printString("\n");
}
// (n)本のチャンネルの電極データを端末に表示する函数。
void MPR121Class::print_filtered_data(uint8_t n){
	usart.printString("Filtered Data: ");
	uint16_t buff[n];
	get_filtered_data(n, buff);
	for(int i=0; i<n; i++){
		usart.printNibble(i);
		usart.printString("ch:");
		usart.printWord(buff[i]);
		usart.printString(", ");
	}
	usart.printString("\n");
}
// (n)本のチャンネルのベースライン値を端末に表示する函数。
void MPR121Class::print_baseline_val(uint8_t n){
	usart.printString("Baseline Val : ");
	uint16_t buff[n];
	get_baseline_val(n, buff);
	for(int i=0; i<n; i++){
		usart.printNibble(i);
		usart.printString("ch:");
		usart.printWord(buff[i]);
		usart.printString(", ");
	}
	usart.printString("\n");
}
// (n)本のチャンネルのタッチ閾値を端末に表示する函数。
void MPR121Class::print_touch_threshold(uint8_t n){
	usart.printString("Touch   Threshold: ");
	uint8_t buff[n];
	get_touch_threshold(n, buff);
	for(int i=0; i<n; i++){
		usart.printNibble(i);
		usart.printString("ch:");
		usart.printByte(buff[i]);
		usart.printString(", ");
	}
	usart.printString("\n");
}
// (n)本のチャンネルのリリース閾値を端末に表示する函数。
void MPR121Class::print_release_threshold(uint8_t n){
	usart.printString("Release Threshold: ");
	uint8_t buff[n];
	get_release_threshold(n, buff);
	for(int i=0; i<n; i++){
		usart.printNibble(i);
		usart.printString("ch:");
		usart.printByte(buff[i]);
		usart.printString(", ");
	}
	usart.printString("\n");
}
// (n)本のチャンネルのタッチ閾値、リリース閾値の両方を端末に表示する函数。
void MPR121Class::print_both_threshold(uint8_t n){
	usart.printString("Touch   Threshold: ");
	uint8_t   touch_buff[n];
	uint8_t release_buff[n];
	get_both_threshold(n, touch_buff, release_buff);
	for(int i=0; i<n; i++){
		usart.printNibble(i);
		usart.printString("ch:");
		usart.printByte(touch_buff[i]);
		usart.printString(", ");
	}
	usart.printString("\n");

	usart.printString("Release Threshold: ");
	for(int i=0; i<n; i++){
		usart.printNibble(i);
		usart.printString("ch:");
		usart.printByte(release_buff[i]);
		usart.printString(", ");
	}
	usart.printString("\n");
}
// タッチ、リリース両方のディバウンス回数(全チャンネル共通)を端末に表示する函数。
void MPR121Class::print_both_debounce_num(){
	usart.printString("Touch   Debounce#: ");
	usart.printNibble(get_touch_debounce_num());
	usart.printString("\n");
	usart.printString("Release Debounce#: ");
	usart.printNibble(get_release_debounce_num());
	usart.printString("\n");
}
// フィルターのサンプリング回数、グローバルCDC値、グローバルCDT値を端末に表示する函数。
void MPR121Class::print_filter_global_cdc_cdt(){
	usart.printString("First Filter#        : ");
	usart.printByte(get_ffi());
	usart.printString("\n");

	usart.printString("Global CDC (uA)      : ");
	usart.printByte(get_gloval_cdc());
	usart.printString("\n");

	usart.printString("Global CDT10 (0.1 us): ");
	usart.printByte(get_gloval_cdt10());
	usart.printString("\n");
	
	usart.printString("Second Filter#       : ");
	usart.printByte(get_sfi());
	usart.printString("\n");

	usart.printString("Sample Interval (ms) : ");
	usart.printByte(get_esi());

	usart.printString("\n");
}
// (n)本の各チャンネルの充放電電流設定値を端末に表示する函数。
void MPR121Class::print_cdc(uint8_t n){
	usart.printString("CDC (uA)      : ");
	uint8_t buff[n];
	get_cdc(n, buff);
	for(int i=0; i<n; i++){
		usart.printNibble(i);
		usart.printString("ch:");
		usart.printByte(buff[i]);
		usart.printString(", ");
	}
	usart.printString("\n");
}
// (n)本の各チャンネルの充放電時感設定値(10倍値)を端末に表示する函数。
void MPR121Class::print_cdt10(uint8_t n){
	usart.printString("CDT10 (0.1 us): ");
	uint8_t buff[n];
	get_cdt10(n, buff);
	for(int i=0; i<n; i++){
		usart.printNibble(i);
		usart.printString("ch:");
		usart.printByte(buff[i]);
		usart.printString(", ");
	}
	usart.printString("\n");
}
// 自動(再)コンフィギュレーションを実行した結果、範囲外になったかどうかを端末に表示する函数。
void MPR121Class::print_out_of_range_status_status(){
	usart.printString("Out-Of-Range Status[12:0]: ");
	usart.printBinaryWord(get_out_of_range_status());
	usart.printString("\n");
}
// 自動コンフィギュレーションに失敗したかどうかを端末に表示する函数。
void MPR121Class::print_auto_config_failed(){
	usart.printString("Auto Config   Failed?: ");
	usart.printNibble(auto_config_failed());
	usart.printString("\n");
}
// 自動再コンフィギュレーションに失敗したかどうかを端末に表示する函数。
void MPR121Class::print_auto_reconfig_failed(){
	usart.printString("Auto-ReConfig Failed?: ");
	usart.printNibble(auto_reconfig_failed());
	usart.printString("\n");
}
// タッチセンサーのフィルタリングパラメーターを表示する函数。
void MPR121Class::print_touch_filter_parameters(uint8_t*buff){
	usart.printString("Touch Filter Parameters: \n");
	get_touch_filter_parameters(buff);
	usart.printString("MHD Rising : "); usart.printByte(buff[0]); usart.printString(", ");
	usart.printString("NHD Rising : "); usart.printByte(buff[1]); usart.printString(", ");
	usart.printString("NCL Rising : "); usart.printByte(buff[2]); usart.printString(", ");
	usart.printString("FDL Rising : "); usart.printByte(buff[3]); usart.printString("\n");
	
	usart.printString("MHD Falling: "); usart.printByte(buff[4]); usart.printString(", ");
	usart.printString("NHD Falling: "); usart.printByte(buff[5]); usart.printString(", ");
	usart.printString("NCL Falling: "); usart.printByte(buff[6]); usart.printString(", ");
	usart.printString("FDL Falling: "); usart.printByte(buff[7]); usart.printString("\n");
	
	usart.printString("                  ");
	usart.printString("NHD Touched: "); usart.printByte(buff[8]); usart.printString(", ");
	usart.printString("NCL Touched: "); usart.printByte(buff[9]); usart.printString(", ");
	usart.printString("FDL Touched: ");	usart.printByte(buff[10]); usart.printString("\n");
}
// 近接センサーのフィルタリングパラメーターを表示する函数。
void MPR121Class::print_prox_filter_parameters(uint8_t*buff){
	usart.printString("Prox Filter Parameters: \n");
	get_prox_filter_parameters(buff);
	usart.printString("PROX MHD Rising : "); usart.printByte(buff[0]); usart.printString(", ");
	usart.printString("PROX NHD Rising : "); usart.printByte(buff[1]); usart.printString(", ");
	usart.printString("PROX NCL Rising : "); usart.printByte(buff[2]); usart.printString(", ");
	usart.printString("PROX FDL Rising : "); usart.printByte(buff[3]); usart.printString("\n");
	
	usart.printString("PROX MHD Falling: "); usart.printByte(buff[4]); usart.printString(", ");
	usart.printString("PROX NHD Falling: "); usart.printByte(buff[5]); usart.printString(", ");
	usart.printString("PROX NCL Falling: "); usart.printByte(buff[6]); usart.printString(", ");
	usart.printString("PROX FDL Falling: "); usart.printByte(buff[7]); usart.printString("\n");
	
	usart.printString("                       ");
	usart.printString("PROX NHD Touched: "); usart.printByte(buff[8]); usart.printString(", ");
	usart.printString("PROX NCL Touched: "); usart.printByte(buff[9]); usart.printString(", ");
	usart.printString("PROX FDL Touched: "); usart.printByte(buff[10]); usart.printString("\n");
}
///////////////////////////////////



MPR121Class::MPR121Class(uint8_t slave_addr){
	_SLA = slave_addr;
	_CL  = 0; // calibration lock
	i2c.init();
}