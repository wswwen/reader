#ifndef __MATCHED_FLITER_H__
#define __MATCHED_FLITER_H__

/** @brief Coherent detection. 
 * 
 *  @param rx_data The received data input, which is a sequence of float type 
 *                 in which real numbers and imaginary numbers are alternately stored.
 *  @param rx_data_len The length of the received data. 
 *  @param filtered_data Output the data after matched_filter_decim. 
 *  @return filtered_data_len
 */
int matched_filter(float * rx_data, int rx_data_len, float * filtered_data);


void matched_filter_decim_init();



#endif