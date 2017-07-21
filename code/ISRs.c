// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011

///////////////////////////////////////////////////////////////////////
// Filename: ISRs.c
//
// Synopsis: Interrupt service routine for codec data transmit/receive
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h" 
#include "voiceprint.h"
  
// Data is received as 2 16-bit words (left/right) packed into one
// 32-bit word.  The union allows the data to be accessed as a single 
// entity when transferring to and from the serial port, but still be 
// able to manipulate the left and right channels independently.

#define LEFT  0
#define RIGHT 1

volatile union {
	Uint32 UINT; // left+right
	Int16 Channel[2]; // left=0, right=1
} SineWaveOut;

volatile union {
	Uint32 UINT;
	Int16 Channel[2];
} CodecDataIn, CodecDataOut;

short xLeft, xRight, yLeft, yRight;

BYTE pVoiceBuffer[g_build_buff_num][BUFFLEN];
int vbIdx = 0;
int vbfIdx = 0;

int status = STANDBY;
int voice_power = 0;
#define SILENT_THRESHOLD 3000

interrupt void Codec_ISR()
{
	if(CheckForOverrun()) // overrun error occurred (i.e. halted DSP)
		return; // so serial port is reset to recover

	CodecDataIn.UINT = ReadCodecData(); // get input data samples
	xLeft = CodecDataIn.Channel[0];
	yLeft = xLeft - 1150;
	CodecDataOut.Channel[0] = CodecDataOut.Channel[1] = yLeft;

	if (status == RECORDING1 || status == RECORDING2) {
		if (abs(yLeft / 40) < 128) {
			pVoiceBuffer[vbIdx][vbfIdx] = abs(yLeft / 40); // make this in range -128 - 128
			voice_power += pVoiceBuffer[vbIdx][vbfIdx++];
		}

		int buff_num = g_build_buff_num;

		if (vbfIdx >= BUFFLEN) {
			vbfIdx = 0;
			if (voice_power > SILENT_THRESHOLD) {
				// not silent
				++vbIdx;
			}
			voice_power = 0;
			if (vbIdx==buff_num) {
				vbIdx = 0;
				if (status == RECORDING1) {
					status = BUILDMODEL1;
				} else if (status == RECORDING2) {
					status = BUILDMODEL2;
				}
			}
		}
	} else if (status == IDENTIFYRECORDING) {
		if (!voiceprint[0].voice_mdl.bValued &&
			!voiceprint[1].voice_mdl.bValued &&
			!voiceprint[2].voice_mdl.bValued &&
			!voiceprint[3].voice_mdl.bValued) {
			printf("No voice model created\n");
			status = STANDBY;
		} else {
			pVoiceBuffer[vbIdx][vbfIdx] = abs(yLeft / 40); // make this in range -128 - 128
			voice_power += pVoiceBuffer[vbIdx][vbfIdx++];
			if (vbfIdx >= BUFFLEN) {
				vbfIdx = 0;
				if (voice_power > SILENT_THRESHOLD) {
					// not silent
					++vbIdx;
				}
				voice_power = 0;
				if (vbIdx==IDENTIFY_BUFF_NUM) {
					vbIdx = 0;
					status = IDENTIFYING;
				}
			}
		}
	}

	WriteCodecData(CodecDataOut.UINT); // send output data to port
}
