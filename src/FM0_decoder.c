/** ------------------------------------------------------- Info --------------------------------------------------------------
 *  @author	      Wen Wang, Hanxue Ding, Yunping Mu.
 *  @version      1.0
 *  @date         2021-6-30.
 *  @brief        This C file decodes the FM0 data bits from the gated signal.  
 *  @copyright    Copyright ownership of Prof. Gongpu Wang's team from BJTU, shall not be reproduced without permission,
                  or copy the way it used to. Otherwise Prof. Gongpu Wang will have the right to pursue legal responsibilities.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../lib/global_vars.h"
#include "FM0_decoder.h"
#define DECODE_SUCCESS 1
#define DECODE_FAILURE 0

// int regis[15];

/** -------------------------------------------------------------------------------------------------------------------------
 *  @brief FM0 decoder main function for extracting valid data from received signal.
 * 
 *  @param data_to_decode The decoded data input from the gate module, which is a sequence of float type 
 *                        in which real numbers and imaginary numbers are alternately stored.
 *  @param data_to_decoder_len the length of the decoded data.
 *  @param bits_decoded Output the bit sequence after decoding.
 *  @param stepsize The step size each FM0 symbol, from zc_clock. 
 *  @return if check crc successes return DECODE_SUCCESS, otherwise return DECODE_FAILURE. 
 */
int FM0_decoder(float* data_to_decode, int data_to_decoder_len, int* bits_decoded, float stepsize)
{
    int frame_start_index, bits_decoded_len;

    frame_start_index = frame_sync(data_to_decode, stepsize);

    channel_estimation(data_to_decode, frame_start_index, stepsize);

    bits_decoded_len = coh_detection(data_to_decode, frame_start_index, stepsize, bits_decoded);

    int i;
    //----------------------------------------------------- m-sequence comparision----------------------------------------------
    int num = 0;//number of error
    int* m_seq;
    m_seq = m_sequence(regis);//生成m序列

    float error_rate;

    for(i = 0; i < 992; i++)
    {
        if (bits_decoded[i+16] != m_seq[i])
        {
            num++;
        }
    }
    error_rate = (float)num/992;
    err_bits_rate += error_rate;
    printf("num = %d, Error rate = %f\n", num, error_rate);
    fprintf(global_mag, "%d,%f,", num, error_rate);

    if(1 == crc_check(bits_decoded, bits_decoded_len))
        return DECODE_SUCCESS;
    else
        return DECODE_FAILURE;
}


/** -------------------------------------------------------------------------------------------------------------------------
 *  @brief Frame synchronization.
 * 
 *  @param data_to_decode The decoded data input from the gate module, which is a sequence of float type 
 *                        in which real numbers and imaginary numbers are alternately stored. 
 *  @param stepsize The step size each FM0 symbol. 
 *  @return The position of the frame start point after frame synchronization: frame_start_index 
 */
int frame_sync(float * data_to_decode, float stepsize) 
{
    int frame_start_index = 0;
    float max = 0;
    float corr;
    float corr_I,corr_Q;
    // Do not have to check entire vector (not optimal)
    for (int i = 0; i < 5 * stepsize; i+=2)  //i<15
    {
        corr = 0;
        corr_I = 0;
        corr_Q = 0;

        // Sync after matched filter (equivalent)
        for (int j = 0; j < 12; j++) //j<12
        {
            corr_I = corr_I + data_to_decode[ (int)(i + j * stepsize) ] * TAG_PREAMBLE[j];
            corr_Q = corr_Q + data_to_decode[ (int)(i+1 + j * stepsize )] * TAG_PREAMBLE[j];
        }
        corr = corr_I * corr_I + corr_Q * corr_Q;  //Corr is equal to the sum of squares of the real and imaginary parts of corr2
        if (corr > max)
        {
            max = corr;
            frame_start_index = i;
        }
    }
    // printf("|-----------frame_start_index :%d\n", frame_start_index);
    return frame_start_index;  // 28
}


/** -------------------------------------------------------------------------------------------------------------------------
 *  @brief  channel_estimation.  
 * 
 *  @param data_to_decode The decoded data input from the gate module, which is a sequence of float type 
 *                in which real numbers and imaginary numbers are alternately stored.
 *  @param frame_start_index The position of the frame start point after frame synchronization. 
 *  @param stepsize The step size each FM0 symbol. 
 *  @return 
 */
int channel_estimation(float * data_to_decode, int frame_start_index, float stepsize){
    // TAG_PREAMBLE {1,1,0,1,0,0,1,0,0,0,1,1}  0,1,3,6,10,11 Least squared channel estimation
    h_est_I = (data_to_decode[frame_start_index] + data_to_decode[(int) (frame_start_index + stepsize)] 
                            + data_to_decode[(int) (frame_start_index + 3*stepsize)] 
                            + data_to_decode[(int) (frame_start_index + 6*stepsize)]
                            + data_to_decode[(int) (frame_start_index + 10*stepsize)]
                            + data_to_decode[(int) (frame_start_index + 11*stepsize)])/6;  
    h_est_Q = (data_to_decode[frame_start_index+1] + data_to_decode[(int) (frame_start_index+1 + stepsize)] 
                            + data_to_decode[(int) (frame_start_index+1 + 3*stepsize)] 
                            + data_to_decode[(int) (frame_start_index+1 + 6*stepsize)]
                            + data_to_decode[(int) (frame_start_index+1 + 10*stepsize)]
                            + data_to_decode[(int) (frame_start_index+1 + 11*stepsize)])/6; 

    // printf("FDFAF%f\n", NORM(h_est_I, h_est_Q)/2.0);
    // printf("FDFAF%lf\n\n", global_snr_noise_energe_agv);
    // double snr = 10 * log10(NORM(h_est_I, h_est_Q)/2.0 / global_snr_noise_energe_agv);
    // fprintf(global_mag, "%lf, ", snr);

}


/** -------------------------------------------------------------------------------------------------------------------------
 *  @brief Coherent detection.
 * 
 *  @param data_to_decode The decoded data input from the gate module, which is a sequence of float type 
 *                in which real numbers and imaginary numbers are alternately stored.
 *  @param frame_start_index The position of the frame start point after frame synchronization. 
 *  @param stepsize The step size each FM0 symbol. 
 *  @param bits_decoded Output the bit sequence after decoding. 
 *  @return void
 */
int coh_detection(float * data_to_decode, int frame_start_index, float stepsize, int * bits_decoded)
{
    frame_start_index = (int)((frame_start_index + 2 * TAG_PREAMBLE_BITS * stepsize + 2 * stepsize)/2)*2;

    float result = 0;
    int prev = 1, written = 0;

    for (int j = 0; j < BITLEN * 2; j += 2)
    {
        result = (data_to_decode[INDEX(j)] - data_to_decode[INDEX(j + 1)]) * h_est_I +
                 (data_to_decode[INDEX(j) + 1] - data_to_decode[INDEX(j + 1) + 1]) * h_est_Q;

        // coh_detection
        if (result > 0) //S1
        {
            if (prev == 1) //previous S1
            {
                bits_decoded[written] = 0; // decoded tag bits
                written++;
            }
            else //previous S0
            {
                bits_decoded[written] = 1; // decoded tag bits
                written++;
            } 
            prev = 1;
        }
        else //S0
        {
            if (prev == -1)
            {
                bits_decoded[written] = 0; 
                written++;
            }
            else
            {
                bits_decoded[written] = 1; 
                written++;
            }
            prev = -1;
        }
    }
    return written;
}

/** -------------------------------------------------------------------------------------------------------------------------
 *  @brief 16-bit CRC check.
 * 
 *  @param bits_decoded The bit sequence after decoding. 
 *  @param bits_decoded_len The length of the bit sequence after decoding. 
 *  @return success for 1, fails for 0
 */
int crc_check(int * bits_decoded,  int bits_decoded_len)
{
    char char_bits[bits_decoded_len];
    for (int i = 0; i < bits_decoded_len; i++) //char bir =16+96+16
    {
        if (bits_decoded[i] == 0)
            char_bits[i] = '0';
        else
            char_bits[i] = '1';
    }
    register unsigned short i, j;
    register unsigned short crc_16, rcvd_crc;
    unsigned char *data;
    int num_bytes = bits_decoded_len / 8;  // bit->byte
    data = calloc(1, num_bytes); // data store
    int mask;

    for (i = 0; i < num_bytes; i++)
    {
        mask = 0x80;
        data[i] = 0;
        for (j = 0; j < 8; j++)
        {
            if (char_bits[(i * 8) + j] == '1')
            {
                data[i] = data[i] | mask;
            }
            mask = mask >> 1; // 1-bit right-rotation
        }
    }
    //recv CRC
    rcvd_crc = (data[num_bytes - 2] << 8) + data[num_bytes - 1];

    //calculated CRC
    crc_16 = 0xFFFF; //initilization
    for (i = 0; i < num_bytes - 2; i++)
    {
        crc_16 ^= data[i] << 8;
        for (j = 0; j < 8; j++)
        {
            if (crc_16 & 0x8000) //whether the most significant bit is 1
            {
                crc_16 <<= 1;     // 1-bit left-rotation
                crc_16 ^= 0x1021; // polynomial hex code
            }
            else
                crc_16 <<= 1;
        }
    }
     crc_16 = ~crc_16;  // Negation

    if (rcvd_crc != crc_16)
        return 0;
    else
        return 1;
}

//对m序列数组初始化，模拟寄存器中随机的初始值(暂时未使用)
void init_regis(int *a, int lenth)
{
    //flag是一个初始化标识符，如果初始化生成全0需要重新初始化
    int flag = 0;
    srand((unsigned)time(NULL));
    for (int i = 0; i < lenth; ++i)
    {
        a[i] = rand()%2;

        if (a[i] == 1)
        {
            flag = 1;
        }
    }
    if (flag == 0)
    {
        init_regis(a,10);
    }

}

int *m_sequence(int *regis)
{
    int m_len = 992;
    int initlenth = 15;
    int *m_data;
    m_data = calloc(sizeof(int), m_len);
    //    init_regis(regis,initlenth);//初始化函数
    //    showlist(regis,initlenth);
    int flag = 1;
    for (int i = 0; i < m_len / 8; ++i)
    {
        for (int k = flag * 8 - 1; k >= (flag - 1) * 8; --k)
        {
            m_data[k] = regis[14];
            // step(regis,initlenth);
            //第一位置值计算且临时保存
            int temp = (regis[0] + regis[14]) % 2;
            //其后位置直接顺延
            for (int j = initlenth - 1; j > 0; --j)
            {
                regis[j] = regis[j - 1];
            }
            //把第一位数原为插回去
            regis[0] = temp;
        }
        flag += 1;
    }
    // printf("array: ");
    // for (int i = 0; i < m_len; ++i) {
    //     printf("%d ",m_data[i]);
    // }
    // printf("\n");

    return m_data;
}