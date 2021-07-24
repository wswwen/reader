#ifndef __ZC_TIMING_H__
#define __ZC_TIMING_H__


#define POSITIVE 1
#define NEGATIVE 0
#define ZC_BEGIN 1
#define ZC_CLOSE 0


#define IS_POSITIVE(__sample) \
    ( (__sample) > 0 ? POSITIVE : NEGATIVE )


/** ------------------------------------------------------------------------------------------------------------------------------
 *  @brief TX data initilization. 
 * 
 *  @param data_to_decode The decoded data input from the gate module, which is a sequence of float type 
 *                in which real numbers and imaginary numbers are alternately stored.
 *  @param data_to_decode_len the length of the decoded data.
 *  @return stepsize The step size each FM0 symbol. 
 */
double zc_clock(float* data_to_decode, int data_to_decode_len);

#endif