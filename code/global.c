#include "global.h"
#include "voiceprint.h"

BOOL bExit             = FALSE;
BOOL killThreading = FALSE;
BOOL g_buildGMM = FALSE;

int totalMemory = 0;

void *mmalloc(size_t size) {
	totalMemory += size;
	return malloc(size);
}

void *ccalloc(size_t nmemb, size_t size) {
	totalMemory += size * nmemb;
	return calloc(nmemb, size);
}
void ffree(void *ptr) {
	free(ptr);
}
