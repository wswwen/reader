/** ------------------------------------------------------- Info --------------------------------------------------------------
 *  @author     H.X.Ding
 *  @version    1.0
 *  @date       2021-6-30
 *  @brief      This C file is responsible for generate tx data for send.
 *  @copyright  Copyright ownership of Prof. Gongpu Wang's team from BJTU, shall not be reproduced without permission,
                or copy the way it used to. Otherwise Prof. Gongpu Wang will have the right to pursue legal responsibilities.
*/

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../lib/global_vars.h"
#include "tx.h"

/** ------------------------------------------------------------------------------------------------------------------------------
 *  @brief TX data initilization. 
 * 
 *  @param void
 *  @return void
 */
void txpara_init()
{
	//Duration in us
  TARI_D= 24;  //Tari，related to BLF，2PW
  DATA0_D = 24;  // Data 0 duration，related to BLF，Tari
  DATA1_D = 48;  // Data 1 duration，related to BLF，2Tari
  TRCAL_D = 200;  // TRcal duration，related to BLF，DR/BLF
  RTCAL_D = 72;  // RTcal duration，related to BLF，data0+data1
  DELIM_D = 12;  // Delimeter duration

  // QUERY_LEN = 12;
  QUERY_LEN = 22;
  ACK_LEN = 12;
  NAK_LEN = 12;
    
  // Number of sampling points
  n_delim_s = DELIM_D / sample_d;  //delim points: 12
  n_pw_s = PW_D / sample_d;   //Pulse Width: 12
  n_data0_s = 2 * PW_D / sample_d;  //data_0: 24
  n_data1_s = 4 * PW_D / sample_d;  //data_1: 48
  n_rtcal_s = RTCAL_D / sample_d;  //rtcal: 72
  n_trcal_s = TRCAL_D / sample_d;  //trcal: 200
  n_preamble_s = n_delim_s + n_data0_s + n_rtcal_s + n_trcal_s;  //preamble
  n_frame_sync_s = n_delim_s + n_data0_s + n_rtcal_s;  //frame_syncronization
  // n_cw_s = 5000;
  n_cw_s = 2000 / sample_d;

  delim = (int *)malloc(n_delim_s * sizeof(int));
  data_0 = (int *)malloc(n_data0_s * sizeof(int));
  data_1 = (int *)malloc(n_data1_s * sizeof(int));
  rtcal = (int *)malloc(n_rtcal_s * sizeof(int));
  trcal = (int *)malloc(n_trcal_s * sizeof(int));
  preamble = (int *)malloc(n_preamble_s * sizeof(int));
  frame_sync_code = (int *)malloc(n_frame_sync_s * sizeof(int));

  query_bits = (int *)malloc(QUERY_LEN * sizeof(int));
  ack_bits = (int *)malloc(ACK_LEN * sizeof(int));
  nak_bits = (int *)malloc(NAK_LEN * sizeof(int));
  query_encoded = (int *)malloc((QUERY_LEN * n_data1_s) * sizeof(int));
  ack_encoded = (int *)malloc((ACK_LEN * n_data1_s) * sizeof(int));
  nak_encoded = (int *)malloc((NAK_LEN * n_data1_s) * sizeof(int));  
  query = (int *)malloc((QUERY_LEN * n_data1_s + n_preamble_s) * sizeof(int));
  ack = (int *)malloc((ACK_LEN * n_data1_s + n_preamble_s) * sizeof(int));
  nak = (int *)malloc((NAK_LEN * n_data1_s + n_preamble_s) * sizeof(int)); 
  query_tx = (float *)malloc(2 * (QUERY_LEN * n_data1_s + n_preamble_s) * sizeof(float));
  ack_tx = (float *)malloc(2 * (ACK_LEN * n_data1_s + n_preamble_s) * sizeof(float));
  nak_tx = (float *)malloc(2 * (NAK_LEN * n_data1_s + n_preamble_s) * sizeof(float));   


  // delimiter 
  for(int i =0; i < n_delim_s; i++)
  {
    delim[i] = 0;
  }

  //data0、data1
  for (int i = 0; i < 0.5 * n_data0_s; i++)
  {
    data_0[i]= 1;
  }
  for (int i = 0.5 * n_data0_s; i < n_data0_s; i++)
  {
    data_0[i]= 0;
  }
  for (int i = 0; i < 0.75 * n_data1_s; i++)
  {
    data_1[i]= 1;
  }
  for (int i = 0.75 * n_data1_s; i < n_data1_s; i++)
  {
    data_1[i]= 0;
  }
    
  //RTcal、TRcal
  for (int i = 0; i < n_rtcal_s - n_pw_s; i++)
  {
    rtcal[i]= 1;
  }
  for (int i = n_rtcal_s - n_pw_s; i < n_rtcal_s; i++)
  {
    rtcal[i]= 0;
  }
  for (int i = 0; i < n_trcal_s - n_pw_s; i++)
  {
    trcal[i]= 1;
  }
  for (int i = n_trcal_s - n_pw_s; i < n_trcal_s; i++)
  {
    trcal[i]= 0;
  }

  int written = 0;
  // preamble、frame sync
  memcpy(&frame_sync_code[written], &delim[0], sizeof(int) * n_delim_s); 
  written += n_delim_s;
  memcpy(&frame_sync_code[written], &data_0[0], sizeof(int) * n_data0_s); 
  written += n_data0_s;
  memcpy(&frame_sync_code[written], &rtcal[0], sizeof(int) * n_rtcal_s); 
  written += n_rtcal_s;

  written = 0;
  memcpy(&preamble[written], &frame_sync_code[0], sizeof(int) * n_frame_sync_s); 
  written += n_frame_sync_s;
  memcpy(&preamble[written], &trcal[0], sizeof(int) * n_trcal_s); 
  written += n_trcal_s;
  printf("txpara_init success!\n");
}

/** ------------------------------------------------------------------------------------------------------------------------------
 *  @brief Command bits generation. 
 * 
 *  @param cmd_bits command data bits
 *  @param cmd command type
 *  @param query 
 *  @return void
 */
void cmd_bits_gen(int * cmd_bits, QUERY_BITS query)
{

  int SEL[2] = {0, 0};     // Inventory special Tag / Inventory all Tags
  int SESSION[2] = {0, 0}; //S0，S1，S2，S3
  int TARGET = 0;          // A or B
  int Q = 0; 
  int Q_VALUE[16][4] =
  {
    {0, 0, 0, 0}, {0, 0, 0, 1}, {0, 0, 1, 0}, {0, 0, 1, 1}, 
    {0, 1, 0, 0}, {0, 1, 0, 1}, {0, 1, 1, 0}, {0, 1, 1, 1}, 
    {1, 0, 0, 0}, {1, 0, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 1}, 
    {1, 1, 0, 0}, {1, 1, 0, 1}, {1, 1, 1, 0}, {1, 1, 1, 1}
  };
  // cmd_bits[0] = query.QUERY_CODE[0];
  // cmd_bits[1] = query.QUERY_CODE[1];
  // cmd_bits[2] = query.QUERY_CODE[2];
  // cmd_bits[3] = query.QUERY_CODE[3];
  // cmd_bits[4] = query.DR;
  // cmd_bits[5] = query.M[0];
  // cmd_bits[6] = query.M[1];
  // cmd_bits[7] = query.TREXT;

  // cmd_bits[8] = SEL[0];
  // cmd_bits[9] = SEL[1];
  // cmd_bits[10] = SESSION[0];
  // cmd_bits[11] = SESSION[1];
  // cmd_bits[12] = TARGET;
  // cmd_bits[13] = Q_VALUE[Q][0];
  // cmd_bits[14] = Q_VALUE[Q][1];
  // cmd_bits[15] = Q_VALUE[Q][2];
  // cmd_bits[16] = Q_VALUE[Q][3];
  query_bits[0] = query.QUERY_CODE[0];
  query_bits[1] = query.QUERY_CODE[1];
  query_bits[2] = query.QUERY_CODE[2];
  query_bits[3] = query.QUERY_CODE[3];
  query_bits[4] = query.DR;
  query_bits[5] = query.M[0];
  query_bits[6] = query.M[1];
  query_bits[7] = query.TREXT;

  query_bits[8] = SEL[0];
  query_bits[9] = SEL[1];
  query_bits[10] = SESSION[0];
  query_bits[11] = SESSION[1];
  query_bits[12] = TARGET;
  // query_bits[13] = Q_VALUE[Q][0];
  query_bits[13] = 1;
  query_bits[14] = Q_VALUE[Q][1];
  query_bits[15] = Q_VALUE[Q][2];
  query_bits[16] = Q_VALUE[Q][3];
  crc_append(query_bits); //CRC5


}

/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Append CRC to command bits. 
 * 
 *  @param cmd_bits command data bits
 *  @return void
 */
void crc_append(int * cmd_bits)
{
  int crc[5] = {1, 0, 0, 1, 0}; //CRC5
  for (int i = 0; i < 17; i++)
  {
    int tmp[5] = {0, 0, 0, 0, 0};
    tmp[4] = crc[3];
    if (crc[4] == 1)
    {
      if (cmd_bits[i] == 1)
      {
        tmp[0] = 0;
        tmp[1] = crc[0];
        tmp[2] = crc[1];
        tmp[3] = crc[2];
      }
      else
      {
        tmp[0] = 1;
        tmp[1] = crc[0];
        tmp[2] = crc[1];
        if (crc[2] == 1)
        {
          tmp[3] = 0;
        }
        else
        {
          tmp[3] = 1;
        }
      }
    }
    else
    {
      if (cmd_bits[i] == 1)
      {
        tmp[0] = 1;
        tmp[1] = crc[0];
        tmp[2] = crc[1];
        if (crc[2] == 1)
        {
          tmp[3] = 0;
        }
        else
        {
          tmp[3] = 1;
        }
      }
      else
      {
        tmp[0] = 0;
        tmp[1] = crc[0];
        tmp[2] = crc[1];
        tmp[3] = crc[2];
      }
    }
    memcpy(crc, tmp, 5 * sizeof(float)); //从tmp的开始位置拷贝5*sizeof(float)个字节的数据到crc
  }
  for (int i = 0; i < 5; i++)
    cmd_bits[i + 17] = crc[4 - i];
}

/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Encode command data bits with PIE. 
 *
 *  @param cmd_bits command data bits
 *  @param cmd_bits_len length of command data bits
 *  @param cmd_encoded pointer to encoded command data
 *  @return cmd_encoded_len: length of encoded command data
 */
int pie_encoder(int * cmd_bits, int cmd_bits_len, int * cmd_encoded)  
{
	int cmd_encoded_len = 0;
    for (int i = 0; i < cmd_bits_len; i++)
    {
      if (cmd_bits[i] == 1)
      {
        memcpy(&cmd_encoded[cmd_encoded_len], &data_1[0], sizeof(int) * n_data1_s); //48
        cmd_encoded_len += n_data1_s;
      }
      else
      {
        memcpy(&cmd_encoded[cmd_encoded_len], &data_0[0], sizeof(int) * n_data0_s); //24
        cmd_encoded_len += n_data0_s;
      }
    }
	return cmd_encoded_len;
}

/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Append preamble to encoded cmd bits. 
 * 
 *  @param cmd_encoded pointer to encoded command data
 *  @param cmd_encoded_len length of encoded command data
 *  @param cmd command data after preamble appending
 *  @return cmd_len: length of encoded command data after appending preamble 
 */
int preamble_append(int * cmd_encoded, int cmd_encoded_len, int * cmd)
{
  int cmd_len = 0;
  memcpy(&cmd[0], &preamble[0], sizeof(int) * n_preamble_s); //308
  cmd_len += n_preamble_s;
  memcpy(&cmd[cmd_len], &cmd_encoded[0], sizeof(int) * cmd_encoded_len); //308
  cmd_len += cmd_encoded_len;
  return cmd_len;
}

/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Continuous wave generation. 
 * 
 *  @param cw_tx pointer to continuous wave bits
 *  @param cw_len length of continuous wave bits
 *  @return void
 */
int cw_gen(float * cw_tx, int cw_len)
{
  int written = 0;
  for(int i = 0; i < cw_len; i++)
  {
    cw_tx[written] = 1;
    written ++;
    cw_tx[written] = 0;
    written ++;
  }
  //cw_tx = (float *)realloc(cw_tx, written * sizeof(float));   
  return written;
}

/** ----------------------------------------------------------------------------------------------------------------------------
 *  @brief Generate cmd data to send. 
 * 
 *  @param cmd pointer to encoded command data
 *  @param cmd_len length of encoded command data
 *  @param cmd_tx pointer to encoded command data to send
 *  @return cmd_tx_len: length of encoded command data to send
 */
int txdata_gen(int * cmd, int cmd_len, float * cmd_tx)
{
  int cmd_tx_len = 0;
    for(int i = 0; i < cmd_len; i++)
    {
        cmd_tx[cmd_tx_len] = cmd[i];
        cmd_tx_len ++;
        cmd_tx[cmd_tx_len] = 0;
        cmd_tx_len++;
    }
  return cmd_tx_len;
}

void tx(){
    int cmd_encoded_len, cmd_len, cmd_tx_len, cw_tx_len;
    // int cw_len, cw_tx_len;
    QUERY_BITS q = {{1, 0, 0, 0}, 0, {0, 0}, 0};
    // cw_len = T1_D + n_samples_TAG_BIT * BITLEN + T2_D; //1761+10*1024+480
    // printf("cw_len = %d\n", cw_len);
    
    // cw_tx = (float *)malloc(2 * cw_len * sizeof(float));

    /* CW  */
    cw_tx = calloc(1, 2 * n_cw_s * sizeof(float));
    cw_tx_len = cw_gen(cw_tx,n_cw_s);

    cmd_bits_gen(query_bits, q);
    crc_append(query_bits);
    cmd_encoded_len = pie_encoder(query_bits, QUERY_LEN, query_encoded);
    cmd_len = preamble_append(query_encoded, cmd_encoded_len, query);
    cmd_tx_len = txdata_gen(query, cmd_len, query_tx);
    // cw_tx_len = cw_gen(cw_tx, cw_len);

    n_tx_s = 0;
    tx_data  = (float *)calloc(cw_tx_len + cmd_tx_len, sizeof(float)); 
    memcpy(&tx_data[n_tx_s], &query_tx[0], sizeof(float) * cmd_tx_len); 
    n_tx_s += cmd_tx_len;
    memcpy(&tx_data[n_tx_s], &cw_tx[0], sizeof(float) * cw_tx_len); 
    n_tx_s += cw_tx_len;
    // fwrite_b("tx_d", tx_data, n_tx_s);

    n_tx_s /= 2;
    printf("tx success!\n");
  // return n_tx_s;
}



