/*
電極3:0のタッチ状態が変化するたびに、そのタッチ状態をATmega328PのPB3:0から出力するとともに、
さまざまなパラメーターを端末に表示するプログラム。 
*/

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include "USARTClass.h"

#include "MPR121Class.h"
MPR121Class pad(0x5A); // (I2Cスレーブアドレス)で実体化する。

const uint8_t n = 4; // 電極をn本使う(電極3:0をタッチセンサーにする)。

ISR(INT1_vect){                            // 電極の状態が変化したら、
	PORTB = pad.get_touch_status() & 0x0F; // 電極3:0の状態をATmega328PのPB3:0から出力し、 
	pad.print_touch_status();              // 全チャンネルのタッチ状態を端末に表示し、
	pad.print_filtered_data(n);            // (n)本の各チャンネルの電極データを端末に表示し、
	pad.print_baseline_val(n);             // (n)本の各チャンネルのベースライン値を端末に表示する。
}

int main(){
	usart.init(); // ボーレートは9600。改行記号は"\n"。
	
	DDRB  = 0x0F; // PB3:0のIOをOUTにしておく。

	EIMSK |= (1 << INT1);  // INT1 (外部割り込み要求1)を有効化する。
	EICRA |= (1 << ISC11); // falling edgeで割り込む。
	sei();                 // グローバル割り込みを有効化する。

	usart.printString("\n**********************************");
	usart.printString("\n********** TOUCH SENSOR **********");
	usart.printString("\n**********************************\n");
	
	// スレーブアドレスを端末に表示する。
	usart.printString("Slave Address: ");
	pad.print_slave_address();
	usart.printString("\n");
	
	// ソフトリセットをかける。
	pad.soft_reset();
	
	// ベースラインフィルターの各種パラメーターを設定する。(設定しなくてもよい)
	pad.set_mhd_r(10); // rising時のMHDを設定する。値の範囲は1～63。
	pad.set_nhd_r(5);  // rising時のNHDを設定する。値の範囲は1～63。
	pad.set_ncl_r(10); // rising時のNCLを設定する。値の範囲は0～255。一般にfalling時よりも速く(小さく)する。
	pad.set_fdl_r(0);  // rising時のFDLを設定する。値の範囲は0～255。
	pad.set_mhd_f(15); // falling時のMHDを設定する。値の範囲は1～63。
	pad.set_nhd_f(5);  // falling時のNHDを設定する。値の範囲は1～63。
	pad.set_ncl_f(10); // falling時のNCLを設定する。値の範囲は0～255。
	pad.set_fdl_f(90); // falling時のFDLを設定する。値の範囲は0～255。一般にrising時よりも遅く(大きく)する。
	pad.set_nhd_t(1);  // touched時のNHDを設定する。値の範囲は1～63。
	pad.set_ncl_t(1);  // touched時のNCLを設定する。値の範囲は0～255。
	pad.set_fdl_t(2);  // touched時のFDLを設定する。値の範囲は0～255。
	
	// ベースラインフィルターの各種パラメーターの設定値を端末に表示する。
	uint8_t buff[11];
	pad.print_touch_filter_parameters(buff);

	// 自動(再)コンフィギュレーションの設定をする。(設定しなくてもよい)
	pad.set_up_side_limit(202);      // 0～255
	pad.set_low_side_limit(117);     // 0～255
	pad.set_target_level(181);       // 0～255
	pad.set_auto_config(6, 4, 1, 1,  // (ffi=6,10,18 or 34回; retry=0,2,4 or 8回; are=1 or 0; ace=1 or 0
                        0, 1, 1, 1); //  scts=1 or 0; oorie=1 or 0; arfie=1 or 0; acfie=1 or 0); 

	// ベースライントラッキングの設定をする。(設定しなくてもよい)
	// 0: 現在のベースラインを初期値とする。(リセット後は0であるため、ベースラインが現在の電極データに追いつくまで短時間だけ反応しない)
	// 1: ベースライントラッキングを無効化する。
	// 2: 最初の測定値の上位5ビットをベースラインの初期値とする。
	// 3: 最初の測定値の上位10ビットをベースラインの初期値とする。
	pad.set_baseline_tracking(2);
	
	// タッチ時のディバウンス回数(0～7)を設定する。(設定しなくてもよい)
	pad.set_touch_debounce_num(4);
	
	 // リリース時のディバウンス回数(0～7)を設定する。(設定しなくてもよい)
	pad.set_release_debounce_num(3);
	
	// (チャンネル)にタッチ閾値(0～255)を設定する。(設定しなくてもよい)
	// 一般にリリース閾値よりも大きくしてヒステリシス特性を持たせる。
	for(int ch=0; ch<n; ch++) pad.set_touch_threshold(ch, 15);
	
	// (チャンネル)にリリース閾値(0～255)を設定する。(設定しなくてもよい)
	// 一般にタッチ閾値よりも小さくしてヒステリシス特性を持たせる。
	for(int ch=0; ch<n; ch++) pad.set_release_threshold(ch, 5);
	
	// (n)本の電極をタッチセンサーとして有効化する。(必ず有効化しなければならない)
	pad.enable_touch(n);

	// 自動(再)コンフィギュレーションの範囲外ステータスを端末に表示する。
	pad.print_out_of_range_status_status();
	
	// 自動コンフィギュレーションに失敗したかどうかを端末に表示する。
	pad.print_auto_config_failed();

	// (n)本のチャンネルのタッチ閾値、リリース閾値の両方を端末に表示する。
	pad.print_both_threshold(n);

	// タッチ、リリース両方のディバウンス回数を端末に表示する。
	pad.print_both_debounce_num();
	
	// フィルターのサンプリング回数、グローバルCDC値、グローバルCDT値を端末に表示する。
	pad.print_filter_global_cdc_cdt();

	// (n)本の各チャンネルの充放電電流値設定値を端末に表示する。
	pad.print_cdc(n);
	
	// (n)本の各チャンネルの各充放電時間設定値(10倍値)を端末に表示する。
	pad.print_cdt10(n);


	while(1);
	return 0;
}
