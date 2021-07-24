#include<stdio.h>
#include<stdlib.h>
#include <math.h>

#include "../lib/global_vars.h"
#include "zc_timing.h"

/** ------------------------------------------------------------------------------------------------------------------------------
 *  @brief TX data initilization. 
 * 
 *  @param data_to_decode The decoded data input from the gate module, which is a sequence of float type 
 *                in which real numbers and imaginary numbers are alternately stored.
 *  @param data_to_decode_len the length of the decoded data.
 *  @return stepsize The step size each FM0 symbol. 
 */
double zc_clock(float* data_to_decode, int data_to_decode_len){

    int NUM_TAG_BIT_DC = 50;  // 用来计算直流的符号数目
    int win_index = 0;
    int win_length = NUM_TAG_BIT_DC * n_samples_TAG_BIT;  //Length: 100

    double * win_samples;
    win_samples = calloc(win_length, sizeof(double));
    double sample_ampl = 0, avg_ampl = 0;

    int start_index;
    start_index = (NUM_TAG_BIT_DC + 10) * n_samples_TAG_BIT; // 多出8个符号，用来平均误差

    double cur_sample;
    int last_sample = POSITIVE;

    int zc_count = 0, last_zc_count = 0;

    int bits_zc = 0;   // bit1 + 2； bit0 + 1; 最终的bit数为 bit_zc/2
    int first_flag = 1;

    int CLOCK_BITS = 1024 + 10 - (NUM_TAG_BIT_DC + 10) - 80;

    for(int i = 0; i < data_to_decode_len / 2; i++){

        sample_ampl = sqrt(data_to_decode[2 * i] * data_to_decode[2 * i] + \
                            data_to_decode[2 * i + 1] * data_to_decode[2 * i + 1]); // Amplitude of signal
        avg_ampl = avg_ampl + (sample_ampl - win_samples[win_index]) / win_length;
        win_samples[win_index] = sample_ampl;     // Amplitude of last sample
        win_index = (win_index + 1) % win_length; // 0-99

        if(i < start_index) continue;

        // 开始计算：
        cur_sample = sample_ampl - avg_ampl;
        if( ( last_sample && !IS_POSITIVE(cur_sample) ) || ( !last_sample && IS_POSITIVE(cur_sample) )) // 过零点时。
        {
            // 第一次进这个循环时啥也不做；因为这个last_zc_count不好。
            if(first_flag == 1){
                last_zc_count = 0;
                last_sample = IS_POSITIVE(cur_sample);
                first_flag = 0;
                continue;
            }

            last_zc_count ++;
            if(last_zc_count > n_samples_TAG_BIT * 0.25 &&
                    last_zc_count < n_samples_TAG_BIT * 0.75){   // zc_count = 5 ~ 10   bit0时
                bits_zc++; 
                zc_count = zc_count + last_zc_count;
            }
            else if(last_zc_count > n_samples_TAG_BIT * 0.75){ // bit1时
                bits_zc += 2;
                zc_count = zc_count + last_zc_count;
            }
            else{
                fprintf(stderr, "zc clock failed!\n");  // 说明用来同步的bit数不够长
                fprintf(global_mag, "zc_failure,");

                return zc_global;
            }

            if (bits_zc / 2 == CLOCK_BITS)   // 仅仅做一次时钟同步，并且使用尽可能多的点
            {  // Calculate average clock every 32 short pulses 
                float stepsize = 2 * zc_count / (double) bits_zc;  // 平均一个符号的采样点数
                if ( abs(stepsize - n_samples_TAG_BIT) < 2 ) // 相差不过两个采样点才算时钟同步正确
                {
                    fprintf(stderr, "zc clock success!\n");
                    fprintf(global_mag, "zc_success,");
                    zc_global = stepsize;

                    return stepsize;
                }
                else{
                    fprintf(stderr, "zc clock failed!\n");
                    fprintf(global_mag, "zc_failure,");
                    stepsize = zc_global;
                    return stepsize;
                }
                break;
            }
            last_zc_count = 0;
        }
        else{
            last_zc_count ++;
        }
        last_sample = IS_POSITIVE(cur_sample);
    }
    fprintf(global_mag, "zc_failure,");
    return zc_global;

}

