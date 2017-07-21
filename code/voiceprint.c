#include "voiceprint.h"

extern BYTE pVoiceBuffer[g_build_buff_num][BUFFLEN];

VOICE_PRINT voiceprint[4];

void VoiceModelInit(VOICE_MODEL * this) {
	int i, j;

	for (i = 0; i < IDENTIFY_NUM; ++i) {
		for (j = 0; j < 2; ++j) {
			this->value[i][j] = 0;
		}
	}
	InitGMMClass(&this->gmm, M);
	this->percent = PERCENT;
	this->m = M;
	this->bValued = FALSE;
	this->frame_num = FRAME_NUM;
}


void VoicePrintInit(VOICE_PRINT * this) {
	VoiceModelInit(&this->voice_mdl);
}

BOOL IdentifyGMM(double ** MFCC, VOICE_PRINT * pUsr) {
	int i;
	BOOL bRes;
	double temp;

	for (i = 0; i < IDENTIFY_NUM; ++i) {
		bRes = Identify(&MFCC[i*(IDENTIFY_FRAME_NUM / IDENTIFY_NUM)], &temp, &(pUsr->voice_mdl.gmm), IDENTIFY_FRAME_NUM / IDENTIFY_NUM, pUsr->voice_mdl.m);
		if (bRes) {
			pUsr->voice_mdl.value[i][IDENTIFY] = fabs(temp);
		} else {
			printf("MFCC Identify Failed.\n");
			return FALSE;
		}
	}
	return TRUE;
}

BOOL GenerateGMM(VOICE_PRINT * pUsr) {
	int i;
	BOOL bRes;
	TBYTE *pRawVoice = NULL;
	double **MFCC = NULL;
	double temp;

	pRawVoice = (TBYTE *)pVoiceBuffer;
	MFCC = (double **)mmalloc(FRAME_NUM * sizeof(double *));
	assert(MFCC != NULL);
	for (i = 0; i < FRAME_NUM; ++i) {
		MFCC[i] = (double *)mmalloc(D * sizeof(double));
		assert(MFCC[i] != NULL);
	}
	printf("Generating MFCC...\n");
	if (voiceToMFCC(pRawVoice, g_build_buff_num * BUFFLEN, MFCC, FRAME_NUM)) {

		printf("Building GMM Model...\n");
		if (!GMMs(MFCC, &pUsr->voice_mdl.gmm, pUsr->voice_mdl.frame_num, pUsr->voice_mdl.m)) {
			printf("Generate GMM failed.\n");
			for (i = 0; i < FRAME_NUM; ++i) {
				free(MFCC[i]);
			}
			free(MFCC);
			return FALSE;
		}

		for (i = 0; i < IDENTIFY_NUM; ++i) {
			bRes = Identify(&MFCC[i*(IDENTIFY_FRAME_NUM / IDENTIFY_NUM)], &temp, &(pUsr->voice_mdl.gmm), IDENTIFY_FRAME_NUM / IDENTIFY_NUM, pUsr->voice_mdl.m);
			if (bRes) {
				pUsr->voice_mdl.value[i][BUILD] = fabs(temp);
			} else {
				printf("MFCC Identify Failed.\n");
				for (i = 0; i < FRAME_NUM; ++i) {
					free(MFCC[i]);
				}
				free(MFCC);
				return FALSE;
			}
		}
		for (i = 0; i < FRAME_NUM; ++i) {
			free(MFCC[i]);
		}
		free(MFCC);
	} else {
		printf("MFCC failed.\n");
		for (i = 0; i < FRAME_NUM; ++i) {
			free(MFCC[i]);
		}
		free(MFCC);
		return FALSE;
	}
	return TRUE;
}
