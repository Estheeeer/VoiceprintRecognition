#ifndef _GMM_H_
#define _GMM_H_

#include "mfcc.h"

typedef struct GMM
{
       int m;
       double * p;
       double ** mu;
       double ** cMatrix;
} GMM;


BOOL GMMs(double ** X, GMM * pGMM, int train_frame_num, int m);
BOOL Identify(double ** X, double * value, GMM * pGMM, int frame_num, int m);
void InitGMMClass(GMM * pGMM, int m);
void *CallocGMM(GMM * pGMM);
void FreeGMM(GMM * pGMM);

#endif /* _GMM_H_ */
