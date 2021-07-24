#ifndef __FM0_DECODER_H__
#define __FM0_DECODER_H__

#define INDEX(_index)   \
    ((int) (((_index) * stepsize + frame_start_index ) / 2) * 2 )
#define NORM(_first, _second)   \
    (float)((_first)*(_first) + (_second)*(_second))



/** -------------------------------------------------------------------------------------------------------------------------
 *  @brief FM0 decoder main function for extracting valid data from received signal.
 * 
 *  @param data_to_decode The decoded data input from the gate module, which is a sequence of float type 
 *                        in which real numbers and imaginary numbers are alternately stored.
 *  @param data_to_decoder_len the length of the decoded data.
 *  @param bits_decoded Output the bit sequence after decoding.
 *  @return if check crc success return DECODE_SUCCESS, otherwise return DECODE_FAILURE. 
 */
int FM0_decoder(float* data_to_decode, int data_to_decode_len, int* bits_decoded, float stepsize);

int channel_estimation(float * data_to_decode, int frame_start_index, float stepsize);
/** -------------------------------------------------------------------------------------------------------------------------
 *  @brief Frame synchronization.
 * 
 *  @param data_to_decode The decoded data input from the gate module, which is a sequence of float type 
 *                        in which real numbers and imaginary numbers are alternately stored. 
 *  @param stepsize 
 *  @return The position of the frame start point after frame synchronization: frame_start_index 
 */
int frame_sync(float * data_to_decode, float stepsize);


/** -------------------------------------------------------------------------------------------------------------------------
 *  @brief Coherent detection. 
 * 
 *  @param data_to_decode The decoded data input from the gate module, which is a sequence of float type 
 *                in which real numbers and imaginary numbers are alternately stored.
 *  @param frame_start_index The position of the frame start point after frame synchronization. 
 *  @param stepsize The step size after symbol synchronization. 
 *  @param bits_decoded Output the bit sequence after decoding. 
 *  @return void
 */
int coh_detection(float * data_to_decode, int frame_start_index, float stepsize, int * bits_decoded);

/** -------------------------------------------------------------------------------------------------------------------------
 *  @brief 16-bit CRC check. 
 * 
 *  @param bits_decoded The bit sequence after decoding. 
 *  @param bits_decoded_len The length of the bit sequence after decoding. 
 *  @return success for 1, fails for 0
 */
int crc_check(int *bits_decoded, int bits_decoded_len);
// void FM0_decoder_init();

/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Initialize m-sequence. 
 * 
 *  @param a pointer to m-sequence
 *  @param lenth number of m-sequence's register
 *  @return void
 */
void init_regis(int *a, int lenth);



/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Generate m-sequence to compare. 
 * 
//  *  @param m_data pointer to m-sequence
//  *  @return tx_len: length of m-sequence
    *  @return m_data: pointer to m-sequence    
 */
int* m_sequence();

#endif