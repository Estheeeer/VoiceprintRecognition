#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "tistdtypes.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void *mmalloc(size_t size);
void *ccalloc(size_t nmemb, size_t size);
void ffree(void *ptr);
extern int totalMemory;

#define BUFFLEN (512 * 2)
#define BUFF_NUM 10

#define NULL 0

#define PI   3.1415926L
#define _2PI 6.2831853L

#ifndef BOOL
#define BOOL Bool
#endif

#ifndef BYTE
#define BYTE unsigned char
#define PBYTE BYTE *
#endif

#ifndef FALSE
#define FALSE 0
#define TRUE  (!FALSE)
#endif

typedef unsigned char TBYTE , *PTBYTE ;
typedef void * HRECORD;

typedef struct COMPX
{
    double real;
    double imag;
} COMPX;

#define PERCENT 20
#define IDENTIFY_NUM 4
#define IDENTIFY_FRAME_NUM 200
#define IDENTIFY_BUFF_NUM ((IDENTIFY_FRAME_NUM/2 + 10)/(BUFFLEN/FRAME_LEN))

#define M 5
#define FRAME_NUM 1000
#define g_build_buff_num (FRAME_NUM/2 + 50) / (BUFFLEN/FRAME_LEN)

#define STANDBY 0
#define RECORDING1 1
#define RECORDING2 2
#define BUILDMODEL1 3
#define BUILDMODEL2 4
#define IDENTIFYRECORDING 5
#define IDENTIFYING 6

extern BOOL bExit;
extern BOOL g_bStopRecv;
extern BOOL killThreading;
extern BOOL g_buildGMM;

#endif /* _GLOBAL_H_ */
