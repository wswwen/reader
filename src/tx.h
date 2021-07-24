#ifndef __TX_H__
#define __TX_H__

//---------------------------------------------------global variable----------------------------------------------------------
typedef struct   
{
    int QUERY_CODE[4];  //区分不同信令
    int DR;             //确定反向链路数据速率
    int M[2];           //确定反向链路编码方式
    int TREXT;          //确定反向链路前导码种类
}QUERY_BITS;

int TARI_D, DATA0_D, DATA1_D, TRCAL_D, RTCAL_D, DELIM_D;
int QUERY_LEN, ACK_LEN, NAK_LEN;
int n_delim_s, n_pw_s, n_data0_s, n_data1_s, n_rtcal_s, n_trcal_s, n_preamble_s, n_frame_sync_s, n_cw_s, n_tx_s;
int * delim, * data_0, * data_1, * rtcal, * trcal, * preamble, * frame_sync_code;
int * query_bits, * ack_bits, * nak_bits, * query_encoded, * ack_encoded, * nak_encoded, * query, * ack, * nak;
float * query_tx, * ack_tx, * nak_tx, * cw_tx, * tx_data;

/** ------------------------------------------------------------------------------------------------------------------------------
 *  @brief TX data initilization. 
 * 
 *  @param void
 *  @return void
 */
void txpara_init();

/** ------------------------------------------------------------------------------------------------------------------------------
 *  @brief Command bits generation. 
 * 
 *  @param cmd_bits command data bits
 *  @param query 
 *  @return void
 */
void cmd_bits_gen(int * cmd_bits, QUERY_BITS query);

/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Append CRC to command bits. 
 * 
 *  @param cmd_bits command data bits
 *  @return void
 */
void crc_append(int * cmd_bits);

/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Encode command data bits with PIE. 
 * 
 *  @param cmd_bits command data bits
 *  @param cmd_bits_len length of command data bits
 *  @param cmd_encoded pointer to encoded command data
 *  @return cmd_encoded_len: length of encoded command data
 */
int pie_encoder(int * cmd_bits, int cmd_bits_len, int * cmd_encoded);

/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Append preamble to encoded cmd bits. 
 * 
 *  @param cmd_encoded pointer to encoded command data
 *  @param cmd_encoded_len length of encoded command data
 *  @param cmd command data after preamble appending
 *  @return cmd_len: length of encoded command data after appending preamble 
 */
int preamble_append(int * cmd_encoded, int cmd_encoded_len, int * cmd);

/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Continuous wave generation. 
 * 
 *  @param cw_tx pointer to continuous wave bits
 *  @param cw_len length of continuous wave bits
 *  @return cw_tx_len: length of cw to send
 */
int cw_gen(float * cw_tx, int cw_len);

/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Generate cmd data to send. 
 * 
 *  @param cmd pointer to encoded command data
 *  @param cmd_len length of encoded command data
 *  @param cmd_tx pointer to encoded command data to send
 *  @return cmd_tx_len: length of encoded command data to send
 */
int txdata_gen(int * cmd_encoded, int cmd_encoded_len, float * cmd_tx);


/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Generate intact data to send. 
 * 
//  *  @param tx_data pointer to encoded intact data
//  *  @return tx_len: length of encoded intact data data to send

//  *  @return tx_len length of intact data
 *  @return void
 * */
void tx();



#endif
