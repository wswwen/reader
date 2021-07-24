/** ----------------------------------------------------- Info --------------------------------------------------------------
 *  @author	      W. Wang.
 *  @version      1.0.
 *  @date         2021-6-30. 
 *  @brief        This function implements half-symbol-period matched filtering and downsampling.  
 *  @copyright    Copyright ownership of Prof. Gongpu Wang's team from BJTU,  shall not be reproduced without permission, 
 *                or copy the way it used to. Otherwise Prof. Gongpu Wang will have the right to pursue legal responsibilities.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../lib/global_vars.h"
#include "matched_filter.h"


float matched_sample_I, matched_sample_Q;
float *win_sample_I, *win_sample_Q;
int win_index_filter;

void matched_filter_decim_init(){

    win_index_filter = 0;
    matched_sample_I = 0;
    matched_sample_Q = 0;
    win_sample_I = calloc(1, NUM_TAPS * sizeof(float));
    win_sample_Q = calloc(1, NUM_TAPS * sizeof(float));

}

/** @brief Coherent detection. 1864
 * 
 *  @param rx_data The received data input, which is a sequence of float type 
 *                 in which real numbers and imaginary numbers are alternately stored.
 *  @param rx_data_len The length of the received data. 
 *  @param filtered_data Output the data after matched_filter_decim. 
 *  @return filtered_data_len
 */
int matched_filter(float * rx_data, int rx_data_len, float * filtered_data)
{
    int written = 0;
    for(int i = 0; i < rx_data_len/2; i++)
    {
        matched_sample_I = matched_sample_I + rx_data[2*i] - win_sample_I[win_index_filter]; 
        matched_sample_Q = matched_sample_Q + rx_data[2*i+1] - win_sample_Q[win_index_filter]; 
        win_sample_I[win_index_filter] = rx_data[2*i];
        win_sample_Q[win_index_filter] = rx_data[2*i+1];
        win_index_filter = (win_index_filter + 1) % NUM_TAPS;

        if(i % DECIM == 0)
        {
            filtered_data[written] = matched_sample_I;
            written++;
            filtered_data[written] = matched_sample_Q;
            written++;
        }
    }

    return written;
}