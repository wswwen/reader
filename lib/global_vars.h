#ifndef __GLOBAL_VARS_H__
#define __GLOBAL_VARS_H__

//-------------------------------------------- Global variable --------------------------------------------------
int DECIM, NUM_TAPS, TAG_PREAMBLE_BITS, T1_D, T2_D, BITLEN, PACKETLEN, PW_D;
float dac_rate, sample_d, adc_rate, T_READER_FREQ, TAG_BIT_D, n_samples_TAG_BIT, h_est_I, h_est_Q;
int sample_to_ungate;
float * rx_data, * filtered_data, * data_to_decode;
int * bits_decoded;
int TAG_PREAMBLE[12];

int err_block, block;
float err_bits_rate;
int RECV_SIZE;
double tx_samp_rate, tx_center_freq, tx_gain, rx_samp_rate, rx_center_freq, rx_gain;

const char *rx_antenna;
const char *tx_antenna;
int regis[15];


FILE *global_mag;
FILE *fp_rx;


// FILE *test_f;
int gate_no;
int snr_flag;
float zc_global;
double global_snr_noise_energe_agv;

/** @def DECODE_SUCCESS/DECODE_FAILURE .  
 *  @brief Decoding successful return DECODE_SUCCESS, otherwise return DECODE_FAILURE. 
 */

// void global_vars_init();

int global_param_config();
int global_vars_init();

#endif