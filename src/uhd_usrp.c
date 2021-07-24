#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <uhd.h>
#include <unistd.h>  //usleep()

#include "uhd_usrp.h"
#include "../lib/global_vars.h"


uhd_usrp_handle usrp;  // Create USRP
uhd_tx_streamer_handle tx_streamer;  // Create TX streamer
uhd_tx_metadata_handle tx_md;  // Create TX metadata
uhd_rx_streamer_handle rx_streamer;  // Create RX streamer
uhd_rx_metadata_handle rx_md;  // Create RX metadata


FILE *tx_f;

//--------------------------------- usrp_tx_config thread----------------------------------
void usrp_tx_config()
{
    tx_f = fopen("../misc/tx", "wb");

    char* device_args = device_args;
    double freq = tx_center_freq;
    double rate = tx_samp_rate;
    double gain = tx_gain;
    size_t channel = 0;
    const char *antenna= "TX/RX"; //tx_antenna;//"TX/RX";

    if (!device_args)
        device_args = strdup("");

    uhd_tune_request_t tune_request = { // Create other necessary structs
        .target_freq = freq,
        .rf_freq_policy = UHD_TUNE_REQUEST_POLICY_AUTO,
        .dsp_freq_policy = UHD_TUNE_REQUEST_POLICY_AUTO
    };
    uhd_tune_result_t tune_result;

    uhd_stream_args_t stream_args = {
        .cpu_format = "fc32",
        .otw_format = "sc16",
        .args = "",
        .channel_list = &channel,
        .n_channels = 1
    };
    
    uhd_usrp_make(&usrp, device_args);
    uhd_tx_streamer_make(&tx_streamer);
    uhd_tx_metadata_make(&tx_md, false, 0, 0.1, true, false);
    
    // fprintf(stderr, "Setting RX Rate: %f...\n", rate);

    uhd_usrp_set_tx_rate(usrp, rate, channel);
    uhd_usrp_set_tx_gain(usrp, gain, channel, "");
    uhd_usrp_set_tx_freq(usrp, &tune_request, channel, &tune_result);
    uhd_usrp_get_tx_stream(usrp, &stream_args, tx_streamer);
    uhd_usrp_set_tx_antenna(usrp, antenna, channel);

    fprintf(stderr, "Creating USRP with args \"%s\"...\n", device_args);
    fprintf(stderr, "Setting TX Rate: %f...\n", rate);
    fprintf(stderr, "Setting TX Gain: %f db...\n", gain);
    fprintf(stderr, "Setting TX frequency: %f MHz...\n", freq / 1e6);
    fprintf(stderr, "Setting TX antenna: %s \n", antenna);
    // fprintf(stderr, "Buffer max size in samples: %zu\n", buff_max_length);

    uhd_usrp_get_tx_rate(usrp, channel, &rate);
    uhd_usrp_get_tx_gain(usrp, channel, "", &gain);
    uhd_usrp_get_tx_freq(usrp, channel, &freq);
    printf("Actual TX Rate: %f...\n", rate);
    printf("Actual TX Gain: %f...\n", gain);
    fprintf(stderr, "Actual TX frequency: %f MHz...\n", freq / 1e6);
    fprintf(stderr, "|-----------TX config done\n");
}


//--------------------------------- usrp_tx_query_cw thread----------------------------------

void usrp_rx_config()
{
    //char* device_args = device_args;
    double freq = rx_center_freq;
    double rate = rx_samp_rate;
    double gain = rx_gain;
    size_t channel = 0;
    const char *antenna= "RX1"; //rx_antenna;//"TX/RX";

    // if (!device_args)
    // device_args = strdup( "" ;

    // Create other necessary structs
    uhd_tune_request_t tune_request = {
        .target_freq = freq,
        .rf_freq_policy = UHD_TUNE_REQUEST_POLICY_AUTO,
        .dsp_freq_policy = UHD_TUNE_REQUEST_POLICY_AUTO,
    };
    uhd_tune_result_t tune_result;

    uhd_stream_args_t stream_args = {
        .cpu_format = "fc32",
        .otw_format = "sc16",
        .args = "",
        .channel_list = &channel,
        .n_channels = 1
    };

    uhd_stream_cmd_t stream_cmd = {
        // .stream_mode = UHD_STREAM_MODE_NUM_SAMPS_AND_DONE,
        .stream_mode = UHD_STREAM_MODE_START_CONTINUOUS,
        // .num_samps = 100000,//n_samples,
        .stream_now = true
    };
    
    //uhd_usrp_make(&usrp, device_args);
    uhd_rx_streamer_make(&rx_streamer);
    uhd_rx_metadata_make(&rx_md);

    uhd_usrp_set_rx_rate(usrp, rate, channel);
    uhd_usrp_set_rx_gain(usrp, gain, channel, "");
    uhd_usrp_set_rx_freq(usrp, &tune_request, channel, &tune_result);
    uhd_usrp_get_rx_stream(usrp, &stream_args, rx_streamer);
    uhd_usrp_set_rx_antenna(usrp, antenna, channel);

    uhd_usrp_set_rx_dc_offset_enabled(usrp, false, 0);   

    //fp/home/vivo/Desktop/ww/111/ww/zhiliu/rx60ntf(stderr, "Setting RX frequency: %f MHz...\n", freq / 1e6);
    fprintf(stderr, "Setting RX antenna: %s \n", antenna);
    // fprintf(stderr, "Buffer max size in samples: %zu\n", buff_max_length);

    uhd_usrp_get_rx_rate(usrp, channel, &rate);
    uhd_usrp_get_rx_gain(usrp, channel, "", &gain);
    uhd_usrp_get_rx_freq(usrp, channel, &freq);
    uhd_rx_streamer_issue_stream_cmd(rx_streamer, &stream_cmd); //??????
    printf("Actual RX Rate: %f...\n", rate);
    printf("Actual RX Gain: %f...\n", gain);
    fprintf(stderr, "Actual RX frequency: %f MHz...\n", freq / 1e6);
    fprintf(stderr, "Issuing stream command.\n");
    fprintf(stderr, "|-----------RX config done\n");
}

void uhd_tx(float * buff, size_t length){

    size_t num_samps_sent = 0; 

    const void** send_ptr = (const void**)&buff; 
    // fwrite(*send_ptr, sizeof(float), length * 2, tx_f);

    uhd_tx_streamer_send(tx_streamer, send_ptr, length, &tx_md, 0.1, &num_samps_sent); 

}


void uhd_rx(void *buff, size_t *length){

    size_t num_samps_recv = 0;

    void ** recv_ptr = (void**)&buff;

    uhd_rx_streamer_recv(rx_streamer, recv_ptr, RECV_SIZE, &rx_md, 3.0, false, &num_samps_recv);

    *length = num_samps_recv;
    
}