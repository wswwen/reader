#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>


#include "../lib/global_vars.h"

void snr_calc(float *data_cw, int cw_len, float* data_fm0, int fm0_len)
{
    int length, start, end;

    double snr_cw_energe = 0, snr_cw_dc = 0;
    length = cw_len / 2;
    start = length / 4;
    end = 3 * length / 4;
    for (int i = start; i < end; i ++){
        snr_cw_energe += data_cw[2 * i] * data_cw[2 * i] + data_cw[2 * i + 1] * data_cw[2 * i + 1];
        snr_cw_dc += sqrt(data_cw[2 * i] * data_cw[2 * i] + data_cw[2 * i + 1] * data_cw[2 * i + 1]);
    }
    double snr_cw_energe_agv = snr_cw_energe / (double)(end - start);
    double snr_cw_dc_agv = snr_cw_dc / (double)(end - start);
    double snr_noise_energe_agv = snr_cw_energe_agv - snr_cw_dc_agv * snr_cw_dc_agv;

    global_snr_noise_energe_agv = snr_noise_energe_agv;
    // printf("FDFAF%.10lf\n\n", global_snr_noise_energe_agv);

    double snr_fm0_energe = 0, snr_fm0_dc = 0;
    length = fm0_len / 2;
    start = length / 4;
    end = 3 * length / 4;
    double *snr_data_fm0 = calloc(length, sizeof(double));
    for (int i = start; i < end; i ++){
        snr_fm0_energe += (data_fm0[2 * i] - snr_cw_dc_agv) * (data_fm0[2 * i] - snr_cw_dc_agv) + (data_fm0[2 * i + 1] -snr_cw_dc_agv) * (data_fm0[2 * i + 1] - snr_cw_dc_agv);
    }
    double snr_fm0_energe_agv = snr_fm0_energe / (double)(end - start);
    double snr_fm0 = snr_fm0_energe_agv - snr_noise_energe_agv;

    double snr = 10 * log10(snr_fm0 / snr_noise_energe_agv);
    
    fprintf(global_mag, "%d,", gate_no);
    snr_flag++;
    fprintf(global_mag, "%lf, ", snr);
}