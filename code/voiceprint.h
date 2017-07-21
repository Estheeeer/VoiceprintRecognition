#ifndef VOICEPRINT_H_INCLUDED
#define VOICEPRINT_H_INCLUDED

#include "global.h"
#include "mfcc.h"
#include "gmm.h"

typedef enum BM {
	BUILD = 0,
	IDENTIFY = 1
} BUILD_MODE;

typedef struct VOICE_MODEL_st {
	GMM gmm;
	double value[IDENTIFY_NUM][2];
	int percent;
	int frame_num;
	int m;
	BOOL bValued;
} VOICE_MODEL;

typedef struct VOICE_PRINT_st
{
	VOICE_MODEL voice_mdl;
} VOICE_PRINT;


#endif // VOICEPRINT_H_INCLUDED

extern VOICE_PRINT voiceprint[4];

void VoiceModelInit(VOICE_MODEL * this);
void VoicePrintInit(VOICE_PRINT * this);
BOOL GenerateGMM(VOICE_PRINT * pUsr);
BOOL IdentifyGMM(double ** MFCC, VOICE_PRINT * pUsr);
