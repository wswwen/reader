#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../lib/global_vars.h"
#include "gate.h"
#include "FM0_decoder.h"
#include "zc_timing.h"
#include "SNR.h"


// 这些值由gate模块控制，gate开时，
extern int stream_decoder_first;
extern int stream_snr_first;
extern float n_samples_PW;

extern int global_n_samples;

// 本模块
int f1 = 447186 - 429540;


int stream_decoder_sample = 0;
int stream_decoder_written;

int stream_snr_sample = 0;

float *sc_gate_data, *sc_gate_data_block_dc;

float *sc2snr_data_cw;
float *sc2snr_data_fm0;
int sc2snr_data_cw_len_written;
int sc2snr_data_fm0_written;

float *sc_dc_samples_I, *sc_dc_samples_Q;
float sc_dc_est_I, sc_dc_est_Q;
int sc_dc_SIZE_D, sc_dc_index, sc_dc_length;

void stream_ctl_init(){

    sc2snr_data_cw = calloc((f1+1) * 2, sizeof(float));
    sc2snr_data_cw_len_written = 0;
    sc2snr_data_fm0 = calloc(f1 + sample_to_ungate + 1, 2 * sizeof(float));
    sc2snr_data_fm0_written = 0;

    sc_gate_data = calloc(sample_to_ungate * 2, sizeof(float));
    sc_gate_data_block_dc = calloc(sample_to_ungate * 2, sizeof(float));
    stream_decoder_written = 0;

    sc_dc_SIZE_D = 120;
    sc_dc_length  = sc_dc_SIZE_D  * (adc_rate / DECIM / 1000000); 

    sc_dc_samples_I = (float *)calloc(sc_dc_length, sizeof(float));
    sc_dc_samples_Q = (float *)calloc(sc_dc_length, sizeof(float));
    sc_dc_est_I = 0;
    sc_dc_est_Q = 0;
    sc_dc_index = 0;

}

void stream_ctl_decoder(float *data, int length)
{
    if(stream_decoder_first){
        stream_decoder_sample = n_samples_PW * 10 - global_n_samples;
        stream_decoder_first = 0;
    }
    for(int i = 0; i < length / 2; i ++)
    {
        stream_decoder_sample ++;
        if( stream_decoder_sample <= f1 )
        {
            // stream_decoder_written = 0;
            sc_dc_est_I = sc_dc_est_I + (data[2 * i] - sc_dc_samples_I[sc_dc_index]) / sc_dc_length;
            sc_dc_est_Q = sc_dc_est_Q + (data[2 * i + 1] - sc_dc_samples_Q[sc_dc_index]) / sc_dc_length;
            sc_dc_samples_I[sc_dc_index] = data[2 * i];
            sc_dc_samples_Q[sc_dc_index] = data[2 * i + 1];
            sc_dc_index = (sc_dc_index + 1) % sc_dc_length;

            if(stream_decoder_sample == f1){

            }
        }

        if ( stream_decoder_sample > f1 && stream_decoder_sample <= f1 + sample_to_ungate ){
            sc_gate_data[stream_decoder_written] = data[2 * i]; //DC offset removal
            sc_gate_data_block_dc[stream_decoder_written] = data[2 * i] - sc_dc_est_I; //DC offset removal
            stream_decoder_written++;
            sc_gate_data[stream_decoder_written] = data[2 * i + 1]; 
            sc_gate_data_block_dc[stream_decoder_written] = data[2 * i + 1] - sc_dc_est_Q; 
            stream_decoder_written++;

            if(stream_decoder_sample == f1 + sample_to_ungate){

                float stepsize = zc_clock(sc_gate_data, sample_to_ungate * 2);
                printf ("stepsize : %f\n", stepsize);
                // fwrite(sc_gate_data_block_dc, sizeof(float), sample_to_ungate * 2, test_f);  // debug for gate and T1;
                if (1 == FM0_decoder(sc_gate_data_block_dc, sample_to_ungate * 2, bits_decoded, stepsize)){
                    printf("Decode Success! \n");
                    fprintf(global_mag, "De_Success,\n");
                    block++;
                }
                    
                else{
                    block++;
                    err_block++;
                    printf("Decode Failure! \n");
                    fprintf(global_mag, "De_Failure,\n");
                }

                stream_decoder_written = 0;
            }
        }
    }
}



void stream_ctl_snr(float *data, int length)
{
    if(stream_snr_first){
        stream_snr_sample = n_samples_PW * 10 - global_n_samples;
        stream_snr_first = 0;
    }
    for(int i = 0; i < length / 2; i ++)
    {
        stream_snr_sample ++;
        if( stream_snr_sample <= f1 )
        {
            sc2snr_data_cw[sc2snr_data_cw_len_written++] = data[2 * i]; //DC offset removal
            sc2snr_data_cw[sc2snr_data_cw_len_written++] = data[2 * i + 1]; //DC offset removal

            if(stream_snr_sample == f1){
                
            }

        }

        if ( stream_snr_sample > f1 && stream_snr_sample <= f1 + sample_to_ungate ){
            
            sc2snr_data_fm0[sc2snr_data_fm0_written++] = data[2 * i]; //DC offset removal
            sc2snr_data_fm0[sc2snr_data_fm0_written++] = data[2 * i + 1]; //DC offset removal
            if(stream_snr_sample == f1 + sample_to_ungate){
                snr_calc(sc2snr_data_cw, sc2snr_data_cw_len_written, sc2snr_data_fm0, sc2snr_data_fm0_written);
                sc2snr_data_cw_len_written = 0;
                sc2snr_data_fm0_written = 0;
            }
        }
    }
}