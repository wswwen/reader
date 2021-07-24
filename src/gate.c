/** ------------------------------------------------------- Info --------------------------------------------------------------
 *  @author		H.X. Ding, S.L. Cao, and J.L. Lin
 *  @version	1.0
 *  @date 		2021-6-30
 *  @brief 	    This C file finds the effective data frame from match filtered signal.
 *  @copyright 	Copyright ownership of Prof. Gongpu Wang's team from BJTU, shall not be reproduced without permission,
                or copy the way it used to. Otherwise Prof. Gongpu Wang will have the right to pursue legal responsibilities.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../lib/global_vars.h"
#include "gate.h"

#define NUM_PULSES_COMMAND 15  // Minimum number of continuous pulses for cmd determination



// extern int regis[15];
// stream_ctl模块也在用的量，控制其他模块
int global_n_samples;
int stream_decoder_first;
int stream_snr_first;

// Number of sample
float n_samples_PW; // Number of sampling point in PW duration: 12
float n_samples_T1; // Number of sampling point in T1 duration: 240

// Gate block parameters
float THRESH_FRACTION;
int WIN_SIZE_D, DC_SIZE_D, win_index, dc_index, win_length, dc_length;

float *win_samples;
float *dc_samples_I, *dc_samples_Q;

float sample_ampl, avg_ampl;
int n_samples, num_pulses;
float dc_est_I, dc_est_Q;
float sample_thresh;

SIGNAL_STATE signal_state = NEG_EDGE;

int GATE_OPEN = 0; //

float *gate_data;

int written = 0;


void gate_init(){
    // Number of sample

    gate_data = calloc((200000 - 67000) * 2, sizeof(float));

    n_samples_PW = PW_D * (adc_rate / DECIM / 1000000);   // Number of sampling point in PW duration: 12
    n_samples_T1 = T1_D * (adc_rate / DECIM / 1000000);   // Number of sampling point in T1 duration: 240

    // Gate block parameters
    THRESH_FRACTION = 0.75;
    WIN_SIZE_D = 250;
    DC_SIZE_D = 120;  // Duration in which dc offset is estimated (T1_D is 250)
    win_index = 0, dc_index = 0;
    win_length = WIN_SIZE_D * (adc_rate/ DECIM /1000000);  //Length: 100
    dc_length  = DC_SIZE_D  * (adc_rate / DECIM / 1000000); //Length of DC offset: 48

    win_samples = (float *)calloc(win_length, sizeof(float));
    dc_samples_I = (float *)calloc(dc_length, sizeof(float));
    dc_samples_Q = (float *)calloc(dc_length, sizeof(float));

    sample_ampl = 0;
    avg_ampl = 0;
    n_samples = 0;
    num_pulses = 0;
    dc_est_I = 0;
    dc_est_Q = 0;
    sample_thresh = 0;
}


/** -------------------------------------------------------------------------------------------------------------------------
 *  @brief 	    This function is responsible for (1) collecting data to be decoded from match filtered one, and (2) removing 
 *              DC offset from data to be decoded.
 * 
 *  @param		filtered_data pointer to data after match filtering and downsampling
 *  @param      filtered_data_len length of filtered_data
 *  @param		data_to_decode pointer to data to be decoded by FM0 decoder
 *  @return     data_to_decode_len — length of data_to_decode
 */
int gate(float *filtered_data, int filtered_data_len)
{

    for (int i = 0; i < filtered_data_len / 2; i++)
    {

        if ( !GATE_OPEN )
        {
            sample_ampl = sqrt(filtered_data[2 * i] * filtered_data[2 * i] + filtered_data[2 * i + 1] * filtered_data[2 * i + 1]); // Amplitude of signal
            avg_ampl = avg_ampl + (sample_ampl - win_samples[win_index]) / win_length;
            win_samples[win_index] = sample_ampl;     // Amplitude of last sample
            win_index = (win_index + 1) % win_length; // 0-99
            // Threshold for detecting negative/positive edges
            sample_thresh = avg_ampl * THRESH_FRACTION; // 0.75

            n_samples++; // Number of sampling point（Duration for continuous wave）

            // Positive edge -> Negative edge
            if (sample_ampl < sample_thresh && signal_state == POS_EDGE)
            {
                n_samples = 0;
                signal_state = NEG_EDGE;
            }
            // Negative edge -> Positive edge
            else if (sample_ampl > sample_thresh && signal_state == NEG_EDGE)
            {
                signal_state = POS_EDGE;
                if (n_samples > n_samples_PW / 2)
                    num_pulses++; // Number of sampling point for PW（Duration of cmd）
                else
                    num_pulses = 0;
                n_samples = 0;
            }

            if (n_samples > n_samples_PW * 10 && signal_state == POS_EDGE && num_pulses > NUM_PULSES_COMMAND) //5
            {
                global_n_samples = i;
                stream_decoder_first = 1;
                stream_snr_first = 1;
                
                GATE_OPEN = 1;
                num_pulses = 0;
                n_samples = n_samples_PW * 10; 

                for(int k = 0; k<14; k++){
                    regis[k] = 0;
                }
                regis[14] = 1;
                snr_flag = 0;

                gate_no ++;
                
                printf("gate open\n");
            }
        }
        else
        {
            n_samples++;
            if ( n_samples > 700000 - 440000 ){
                GATE_OPEN = 0;
                n_samples = 0;
                num_pulses = 0;

                printf("error block rate = %f\n", (float)err_block/block);
                printf("error bits rate = %f\n", err_bits_rate/block);
                printf("gate close\n");

            }
        }
    }
    return GATE_OPEN ? DECODER_BEGIN : DECODER_CLOSE;
}
