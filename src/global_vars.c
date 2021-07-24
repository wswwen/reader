#include <stdio.h>
#include <libconfig.h>
#include <stdlib.h>

#include "../lib/global_vars.h"

/** @brief Use Libconfig library to configure global variables
 * 
 *  @param 
 *  @return success for EXIT_SUCCESS(0), failure for EXIT_FAILURE(1)
 */
int global_param_config(){

    //------------------------------------打开配置文件-------------------------------------------
    config_t cfg;
    config_setting_t *setting;
    config_init(&cfg);
     /* Read the file. If there is an error, report it and exit. */
    if (!config_read_file(&cfg, "../lib/global_vars.cfg"))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return (EXIT_FAILURE);
    }
    //-------------------------------------------------------------------------------------------

    const char *version;
    config_lookup_string(&cfg, "version", &version);
    fprintf(stdout, "current code version : %s\n", version);

    //-----------------------------------从配置文件中读数据-----------------------------------------
    setting = config_lookup(&cfg, "tx_param");
    if(setting != NULL){
        // 发送端参数的配置
        double __dac_rate;
        int __PW_D;
        if (config_setting_lookup_float(setting, "dac_rate", &__dac_rate) && 
            config_setting_lookup_int(setting, "PW_D", &__PW_D))
        {
            printf("tx config success!\n");
            dac_rate = __dac_rate;
            tx_samp_rate = dac_rate;
            sample_d = 1.0 / dac_rate * 1e6; // DAC sample duration
            PW_D = __PW_D;
        }
        else
        {
            printf("tx config fails!\n");
            return (EXIT_FAILURE);
        }
    }


    setting = config_lookup(&cfg, "rx_param");
    if(setting != NULL){
        // 接收端参数的配置
        double  __adc_rate,
                __T_READER_FREQ,
                __h_est_I,
                __h_est_Q;
        if (    config_setting_lookup_int(setting, "DECIM", &DECIM)
             && config_setting_lookup_int(setting, "NUM_TAPS", &NUM_TAPS)
             && config_setting_lookup_int(setting, "T1_D", &T1_D)
             && config_setting_lookup_int(setting, "T2_D", &T2_D)
             && config_setting_lookup_int(setting, "TAG_PREAMBLE_BITS", &TAG_PREAMBLE_BITS)
             && config_setting_lookup_int(setting, "BITLEN", &BITLEN)  
             && config_setting_lookup_float(setting, "adc_rate", &__adc_rate)  
             && config_setting_lookup_float(setting, "T_READER_FREQ", &__T_READER_FREQ)  
             && config_setting_lookup_float(setting, "h_est_I", &__h_est_I)  
             && config_setting_lookup_float(setting, "h_est_Q", &__h_est_Q)  

             )
             {
                 printf("tx config success!\n");
                 
                 adc_rate = __adc_rate;

                 rx_samp_rate = adc_rate;

                 T_READER_FREQ = __T_READER_FREQ;

                 TAG_BIT_D = 1.0 / T_READER_FREQ * 1000000; // Duration in 25 us
                 n_samples_TAG_BIT = TAG_BIT_D * (adc_rate / DECIM / 1000000);
                 PACKETLEN = (BITLEN + 2) * n_samples_TAG_BIT;

                 h_est_I = __h_est_I;
                 h_est_Q = __h_est_Q;

                 sample_to_ungate = n_samples_TAG_BIT * 1.05 * (BITLEN + TAG_PREAMBLE_BITS); //
             }
        else {
            fprintf(stderr, "rx config fails!\n");
            return (EXIT_FAILURE);
        }

        config_setting_t *arr = config_setting_get_member(setting, "TAG_PREAMBLE"); //先把setting 的孩子表示出来
        int count = config_setting_length(arr);                            //求数组或列表长度
        for (int i = 0; i < count; i++)
        {
            TAG_PREAMBLE[i] = config_setting_get_int_elem(arr, i);
        }
        
    }


    setting = config_lookup(&cfg, "uhd_usrp");
    if(setting != NULL){
        if (   config_setting_lookup_float(setting, "tx_center_freq", &tx_center_freq)  
             && config_setting_lookup_float(setting, "tx_gain", &tx_gain)  
             && config_setting_lookup_string(setting, "tx_antenna", &tx_antenna)  

             && config_setting_lookup_int(setting, "RECV_SIZE", &RECV_SIZE)  

             && config_setting_lookup_float(setting, "rx_center_freq", &rx_center_freq)
             && config_setting_lookup_float(setting, "rx_gain", &rx_gain)
             && config_setting_lookup_string(setting, "rx_antenna", &rx_antenna)  
             )
             {
                 printf("uhd_usrp config success!\n");
             }
        else {
            fprintf(stderr, "uhd_usrp config fails!\n");
            return (EXIT_FAILURE);
        }
    }

    config_destroy(&cfg);  // 关闭配置文件

    return EXIT_SUCCESS;
}


int global_vars_init(){
    zc_global = n_samples_TAG_BIT;

}
