#ifndef __UHD_USRP_H__
#define __UHD_USRP_H__


void usrp_tx_config();


void usrp_rx_config();


void uhd_rx(void *buff, size_t *length);


void uhd_tx(float * buff, size_t length);


#endif