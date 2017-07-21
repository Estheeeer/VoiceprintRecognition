#ifndef _MFCC_
#define  _MFCC_
#include "global.h"
#include "FFT2.h"
#include <float.h>
#include <stdlib.h>
#include <stdio.h>

#define SIGNAL_TIME 5
#define FRAME_LEN 256

#define SILENCE_VALUE   0.016
#define D 20 // number of triangular filters

typedef struct VOICEFRAME
{
    COMPX  *pCmpx;
} VOICEFRAME;


BOOL voiceToMFCC(BYTE * pRawVoice, int iSampleNum, double ** MFCC, int train_frame_num);

#endif  //_MFCC_
