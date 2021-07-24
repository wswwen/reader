/** ------------------------------------------------------- Info --------------------------------------------------------------
 *  @author	      Hanxue Ding
 *  @version      1.0
 *  @date         2021-7-2.
 *  @brief        This C file decodes the FM0 data bits from the gated signal.  
 *  @copyright    Copyright ownership of Prof. Gongpu Wang's team from BJTU, shall not be reproduced without permission,
                  or copy the way it used to. Otherwise Prof. Gongpu Wang will have the right to pursue legal responsibilities.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include "../lib/global_vars.h"

#include "../src/zc_timing.h"
#include "../src/tx.h"
#include "../src/matched_filter.h"
#include "../src/gate.h"
#include "../src/FM0_decoder.h"
#include "../src/uhd_usrp.h"
#include "../src/stream_control.h"
#include "../src/SNR.h"

// int stop_signal_called = 0;

// void sigint_handler(int code){
//     (void)code;
//     stop_signal_called = 1;
// }

void *reader_general_work(void *arg)
{
    while(1){
        for (int i = 0; i < 10; i++){
            uhd_tx(cw_tx, n_cw_s);
        }
        uhd_tx(tx_data, n_tx_s);
        for (int i = 0; i<10; i++){
            uhd_tx(cw_tx, n_cw_s);
        }
    }
}


void* rx_decoder(void *arg){
    err_block = 0;
    err_bits_rate = 0;
    block = 0;
    fp_rx = fopen("../misc/rx", "wb");

    size_t items = 0;
    int filtered_data_len, data_to_decode_len;

    rx_data = calloc(RECV_SIZE, sizeof(float) * 2);
    filtered_data = calloc(RECV_SIZE, sizeof(float) * 2);
    bits_decoded = calloc(2000, sizeof(int));

    while (1)
    {
        uhd_rx(rx_data, &items);
        if (gate(rx_data, items * 2))
        {
            stream_ctl_snr(rx_data, items * 2);
            filtered_data_len = matched_filter(rx_data, items * 2, filtered_data);
            stream_ctl_decoder(filtered_data, filtered_data_len);
        }
        if(gate_no == 6)
        {
            exit(0);  // End all threads.
        }
    }
}

int main()
{
    if (!global_param_config())
    {
        fprintf(stderr, "global_param_config success!\n");
    }
    global_vars_init();

    txpara_init();
    tx();//生成发送的指令;

    usrp_tx_config();
    usrp_rx_config();

    matched_filter_decim_init();
    gate_init();
    stream_ctl_init();

    global_mag = fopen("../misc/massage.csv", "wr");
    fprintf(global_mag, "gate_no,zc_timing,SNR,err_num,bit_err_rate,decode_result,\n");

    // signal(SIGINT, &sigint_handler);

    pthread_t usrp_rx_tid, reader_tid;
    pthread_create(&usrp_rx_tid, NULL, rx_decoder, NULL);
    pthread_create(&reader_tid, NULL, reader_general_work, NULL);

    pthread_join(usrp_rx_tid, NULL);
    pthread_join(reader_tid, NULL);

    return 1;
}