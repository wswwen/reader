#ifndef __GATE_H__
#define __GATE_H__

#define DECODER_CLOSE 0
#define DECODER_BEGIN 1


/** -------------------------------------------------------------------------------------------------------------------------
 *  @brief 	    This function is responsible for (1) collecting data to be decoded from match filtered one, and (2) removing 
 *              DC offset from data to be decoded.
 * 
 *  @param		filtered_data pointer to data after match filtering and downsampling
 *  @param      filtered_data_len length of filtered_data
 *  @param		data_to_decode pointer to data to be decoded by FM0 decoder
 *  @return     data_to_decode_len — length of data_to_decode
 */
int gate(float *filtered_data, int filtered_data_len);

typedef enum {NEG_EDGE, POS_EDGE}SIGNAL_STATE;

void gate_init();

extern float *gate_data;

#endif