#ifndef __STREAM_CONTROL__
#define __STREAM_CONTROL__

void stream_ctl_decoder(float *data, int length);
void stream_ctl_snr(float *data, int length);

void stream_ctl_init();

#endif