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

const uint8_t SOFT_RESET_VAl = 0x63; // ソフトリセットレジスタに書き込む値

const uint8_t NORMAL_IN = 0, PULL_DOWN = 2, PULL_UP = 3; // IN時の回路構成
const uint8_t PUSH_PULL = 0, SINK      = 2, SOURCE  = 3; // OUT時の回路構成

 
class MPR121Class{
	private:
	uint8_t _SLA; // スレーブアドレス。
	uint8_t _CL;  // calibration lock
	uint8_t  get_ECR(); // ECRレジスタを読み出す。

	public:
	uint8_t get_slave_address(); // スレーブアドレスを取得する。
	
	void soft_reset(); // ソフトリセットをかける。

	// ベースライントラッキングを有効化、無効化する。
	// cl=0: デフォルト。そのときのベースライン値を初期ベースライン値としてロードする。
	// cl=1: ベースライントラッキングを無効化する。
	// cl=2: 初回電極データの上位5ビットを初期ベースライン値としてロードする。
	// cl=3: 初回電極データの上位10ビットを初期ベースライン値としてロードする。
	void set_baseline_tracking(uint8_t cl); 

	uint16_t get_touch_status();      // 13chぶん全部のタッチ状態(ch13は近接状態)を取得する。

	void     enable_touch(uint8_t n); // (n)本の電極をタッチセンサーにする。n=0～12。
	void     enable_prox(uint8_t n);  // (n)本の電極を近接センサーにする。n=0, 2, 4, or 12。
	void     enter_stop_mode();       // STOPモードに移行する。

	uint16_t get_filtered_data(uint8_t ch);                    // (各チャンネル)の電極データ(10ビット)を取得する。
	void     get_filtered_data(uint8_t n, uint16_t*buff_word); // (n)本のチャンネルの電極データ(10ビット)をバッファーに格納する。

	uint16_t get_baseline_val(uint8_t ch);                    // (各チャンネル)のベースライン値(10ビット、下2ビットは無効)を取得する。
	void     get_baseline_val(uint8_t n, uint16_t*buff_word); // (n)本のチャンネルのベースライン値(10ビット、下2ビットは無効)をバッファーに格納する。

	uint8_t get_touch_threshold(uint8_t ch);              // (各チャンネル)のタッチ閾値を取得する。

	void    get_touch_threshold(uint8_t n, uint8_t*buff); // (n)本のチャンネルのタッチ閾値をバッファーに格納する。
	void    set_touch_threshold(uint8_t ch, uint8_t val); // (各チャンネル)のタッチ閾値(0～255)をセットする。

	uint8_t get_release_threshold(uint8_t ch);              // (各チャンネル)のリリース閾値を取得する。
	void    get_release_threshold(uint8_t n, uint8_t*buff); // (n)本のチャンネルのリリース閾値をバッファーに格納する。

	void    set_release_threshold(uint8_t ch, uint8_t val); // (各チャンネル)のリリース閾値(0～255)をセットする。

	void get_both_threshold(uint8_t n, uint8_t*touch_buff, uint8_t*release_buff); // (n)本のチャンネルのタッチ閾値、リリース閾値を一度にバッファーに格納する。

	uint8_t get_touch_debounce_num();   // タッチディバウンス回数を取得する。全チャンネル共通。
	uint8_t get_release_debounce_num(); // リリースディバウンス回数を取得する。全チャンネル共通。

	void set_touch_debounce_num(uint8_t num);   // タッチディバウンス回数(0～7)をセットする。全チャンネル共通。
	void set_release_debounce_num(uint8_t num); // リリースディバウンス回数(0～7)をセットする。全チャンネル共通。

	uint8_t get_ffi();          // 1次フィルターのサンプリング回数。返値:6,10,18,34回(対応するレジスタ値は0～3)。
	uint8_t get_gloval_cdc();   // 共通の充放電電流設定値。返値:直読0～63 (uA)。
	uint8_t get_gloval_cdt10(); // 共通の充放電時間設定値。返値:10 * 2^(n-2) (us) (対応するレジスタ値0～7)。整数のまま扱いたいので10倍値を返す。
	uint8_t get_sfi();          // 2次フィルターのサンプリング回数。返値:4,6,10,18回(対応するレジスタ値は0～3)。
	uint8_t get_esi();          // 2次フィルターでのサンプリング間隔設定値。返値:2^n (ms) (対応するレジスタ値は0～7)。

	void get_cdc(uint8_t n, uint8_t*buff);   // (n)本の各チャンネルの充放電電流設定値をバッファーに格納する。返値:直読0～63 uA。
	void get_cdt10(uint8_t n, uint8_t*buff); // (n)本の各チャンネルの充放電時間設定値をバッファーに格納する。返値:10 * 2^(n-2) us (対応するレジスタ値は0～7)。整数のまま扱いたいので10倍値を返す。

	
	// 自動コンフィギュレーションレジスタの設定をする。
	// ffi   = 1次フィルターのサンプリング回数。6,10,18,34回のいずれかで指定する(対応するレジスタ値は0～3)。
	// retry = リトライ回数。0,2,4,8回のいずれかで指定する(対応するレジス多値は0～3)。
	// are   = 自動再コンフィギュレーションを実行する。1: 有効、0: 無効。
	// ace   = STOP → RUNモード移行時に自動コンフィギュレーションを実行する。1: 有効、0: 無効。
	// scts  = 充電時間検索を省略する。1: 省略する、0: 省略しない。
	// oorie = 範囲外割り込み有効化。1: 有効、0: 無効。
	// arfie = 自動再コンフィギュレーション失敗割り込み有効化。1: 有効、0: 無効。
	// acfie = 自動コンフィギュレーション失敗割り込み有効化。1: 有効、0: 無効。
	void set_auto_config(uint8_t ffi, uint8_t retry, uint8_t are, uint8_t ace,
                         uint8_t scts, uint8_t oorie, uint8_t arfie, uint8_t acfie);
	
	void set_up_side_limit (uint8_t usl); // 自動(再)コンフィギュレーション時の電極データの上限。0～255。
	void set_low_side_limit(uint8_t lsl); // 自動(再)コンフィギュレーション時の電極データの下限。0～255。
	void set_target_level  (uint8_t  tl); // 自動(再)コンフィギュレーション時の電極データの目標値。0～255。
	
	uint16_t get_out_of_range_status(); // 自動(再)コンフィギュレーションの範囲外ステータス取得する。
	uint8_t auto_config_failed();       // 自動コンフィギュレーションに失敗したかどうかを取得する。
	uint8_t auto_reconfig_failed();     // 自動再コンフィギュレーションに失敗したかどうかを取得する。

	// MHD Rising , NHD Rising , NCL Rising , FDL Rising, 
	// MHD Falling, NHD Falling, NCL Falling, FDL Falling,
	//              NHD Touched, NCL Touched, FDL Touchedの順番に11バイトをバッファーに格納する。
	void get_touch_filter_parameters(uint8_t*buff);
	
	// PROX MHD Rising , PROX NHD Rising , PROX NCL Rising , PROX FDL Rising,
	// PROX MHD Falling, PROX NHD Falling, PROX NCL Falling, PROX FDL Falling,
	//                   PROX NHD Touched, PROX NCL Touched, PROX FDL Touchedの順番に11バイトをバッファーに格納する。
	void get_prox_filter_parameters(uint8_t*buff);
	
	void set_mhd_r(uint8_t val); // rising時のMHDを設定する。値の範囲は1～63。
	void set_nhd_r(uint8_t val); // rising時のNHDを設定する。値の範囲は1～63。
	void set_ncl_r(uint8_t val); // rising時のNCLを設定する。値の範囲は0～255。
	void set_fdl_r(uint8_t val); // rising時のFDLを設定する。値の範囲は0～255。
	
	void set_mhd_f(uint8_t val); // falling時のMHDを設定する。値の範囲は1～63。
	void set_nhd_f(uint8_t val); // falling時のNHDを設定する。値の範囲は1～63。
	void set_ncl_f(uint8_t val); // falling時のNCLを設定する。値の範囲は0～255。
	void set_fdl_f(uint8_t val); // falling時のFDLを設定する。値の範囲は0～255。
	
	void set_nhd_t(uint8_t val); // touched時のNHDを設定する。値の範囲は1～63。
	void set_ncl_t(uint8_t val); // touched時のNCLを設定する。値の範囲は0～255。
	void set_fdl_t(uint8_t val); // touched時のFDLを設定する。値の範囲は0～255。
	
	void set_mhd_prox_r(uint8_t val); // prox rising時のMHDを設定する。値の範囲は1～63。
	void set_nhd_prox_r(uint8_t val); // prox rising時のNHDを設定する。値の範囲は1～63。
	void set_ncl_prox_r(uint8_t val); // prox rising時のNCLを設定する。値の範囲は0～255。
	void set_fdl_prox_r(uint8_t val); // prox rising時のFDLを設定する。値の範囲は0～255。
	
	void set_mhd_prox_f(uint8_t val); // prox falling時のMHDを設定する。値の範囲は1～63。
	void set_nhd_prox_f(uint8_t val); // prox falling時のNHDを設定する。値の範囲は1～63。
	void set_ncl_prox_f(uint8_t val); // prox falling時のNCLを設定する。値の範囲は0～255。
	void set_fdl_prox_f(uint8_t val); // prox falling時のFDLを設定する。値の範囲は0～255。
	
	void set_nhd_proxr_t(uint8_t val); // prox touched時のNHDを設定する。値の範囲は1～63。
	void set_ncl_proxr_t(uint8_t val); // prox touched時のNCLを設定する。値の範囲は0～255。
	void set_fdl_proxr_t(uint8_t val); // prox touched時のFDLを設定する。値の範囲は0～255。
	

	//////////////////////
	/* GPIO関連の函数群 */
	//////////////////////
	void enable_gpio       (uint8_t led_7_0);   // GPIOを有効化する。1: 有効, 0; 無効。
	void set_gpio_direction(uint8_t led_7_0=0); // IOの向きを指定する。1:OUT, 0:IN。
	void write_gpio_data   (uint8_t led_7_0);   // GPIOデータレジスタを書き換える。
	void set_gpio_data     (uint8_t led_7_0);   // 指定したビットを1にセットする。
	void clear_gpio_data   (uint8_t led_7_0);   // 指定したビットをクリアする。
	void toggle_gpio_data  (uint8_t led_7_0);   // 指定したビットをトグルする。
	
	
	// GPIO IN時 : circuit = NORMAL_IN, PULL_DOWN, PULL_UPのいずれか。
	// GPIO OUT時: circuit = PUSH_PULL, SINK     , SOURCEのいずれか。
	void set_gpio_circuit(uint8_t led_n, uint8_t circuit);
	
	uint8_t read_gpio_data(); // GPIOデータレジスタを読み出す。
	
	void analog_write(uint8_t led_n, uint8_t duty); // (ledピン番号, duty=16分のいくつ)


	///////////////////////////////////
	/* 端末に何かを表示するための函数群 */
	///////////////////////////////////
	void print_slave_address();                       // スレーブアドレスを端末に表示する函数。
	void print_touch_status();                        // 全チャンネルのタッチステータスを端末に表示する函数。
	void print_filtered_data(uint8_t n);              // (n)本のチャンネルの電極データを端末に表示する函数。
	void print_baseline_val(uint8_t n);               // (n)本のチャンネルのベースライン値を端末に表示する函数。
	void print_touch_threshold(uint8_t n);            // (n)本のチャンネルのタッチ閾値を端末に表示する函数。
	void print_release_threshold(uint8_t n);          // (n)本のチャンネルのリリース閾値を端末に表示する函数。
	void print_both_threshold(uint8_t n);             // (n)本のチャンネルのタッチ閾値、リリース閾値の両方を端末に表示する函数。
	void print_both_debounce_num();                   // タッチ、リリース両方のディバウンス回数(全チャンネル共通)を端末に表示する函数。
	void print_filter_global_cdc_cdt();               // フィルターのサンプリング回数、グローバルCDC値、グローバルCDT値を端末に表示する函数。
	void print_cdc(uint8_t n);                        // (n)本の各チャンネルの充放電電流設定値を端末に表示する函数。
	void print_cdt10(uint8_t n);                      // (n)本の各チャンネルの充放電時感設定値(10倍値)を端末に表示する函数。
	void print_out_of_range_status_status();          // 自動(再)コンフィギュレーションを実行した結果、範囲外になったかどうかを端末に表示する函数。
	void print_auto_config_failed();                  // 自動コンフィギュレーションに失敗したかどうかを端末に表示する函数。
	void print_auto_reconfig_failed();                // 自動再コンフィギュレーションに失敗したかどうかを端末に表示する函数。
	void print_touch_filter_parameters(uint8_t*buff); // タッチセンサーのフィルタリングパラメーターを表示する函数。
	void print_prox_filter_parameters(uint8_t*buff);  // 近接センサーのフィルタリングパラメーターを表示する函数。
	///////////////////////////////////
	

	// コンストラクタ
	MPR121Class(uint8_t slave_addr);
};

#endif
