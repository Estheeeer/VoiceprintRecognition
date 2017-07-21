#include "FFT2.h"

inline void EE(COMPX b1, COMPX b2, COMPX  * result) {
    result->real = b1.real*b2.real - b1.imag*b2.imag;
    result->imag = b1.real*b2.imag + b1.imag*b2.real;
}


void FFT2(COMPX *xin, int N) {
    int f, m, LH, nm, i, k, j, L;
    double p, ps;
    int le, B, ip;
    COMPX w, t;

    LH = N / 2;
    f = N;
    for (m = 1; (f = f / 2) != 1; ++m) {
        ;
    }
    nm = N-2;
    j = N / 2;
    for (i = 1; i <= nm; ++i) {
        if (i < j) {
            t = xin[j];
            xin[j] = xin[i];
            xin[i] = t;
        }
        k = LH;
        while (j >= k) {
            j = j - k;
            k = k / 2;
        }
        j = j + k;
    }

	for (L = 1; L<=m; L++) {
		le = (int)pow(2, L);
		B = le / 2;
		for (j = 0; j <= B - 1; j++) {
			p = pow(2, m-L) * j;
			ps = 2 * PI/ N * p;
			w.real = cos(ps);
			w.imag = -sin(ps);
			for (i = j; i <= N-1; i = i + le) {
				ip = i + B;
				EE(xin[ip], w, &t);
				xin[ip].real = xin[i].real - t.real;
				xin[ip].imag = xin[i].imag - t.imag;
				xin[i].real = xin[i].real + t.real;
				xin[i].imag = xin[i].imag + t.imag;
			}
		}
	}
}
