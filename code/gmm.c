#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "gmm.h"
#include "cluster.h"

#define DIFF_GMM_VALUE     0.1
#define IDENTIFY_VALUE    (-30000)
//#define IDENTITY_FRAME_NUM 100

static BOOL InitGMM(double ** MFCC, GMM * pGMM, int train_frame_num, int m);
static BOOL BuildGMM(GMM * pGMM, GMM * Out, double ** X, int train_frame_num, int m);
static double GMM_diff(GMM * gmm1, GMM * gmm2);
static double GMM_density(GMM * pGMM, double * X, int index);


void InitGMMClass(GMM * pGMM, int m)
{
	pGMM->m = m;
	pGMM->p = NULL;
	pGMM->mu = NULL;
	pGMM->cMatrix = NULL;
}

void * CallocGMM(GMM * pGMM)
{
	int i;
	int m = pGMM->m;

	pGMM->p = (double *)ccalloc(m, sizeof(double));
	if (!pGMM->p)
	{
		return NULL;
	}
	pGMM->mu = (double **)mmalloc(m * sizeof(double *));
	if (!pGMM->mu)
	{
		free(pGMM->p);
		return NULL;
	}
	pGMM->cMatrix = (double **)mmalloc(m * sizeof(double *));
	if (!pGMM->cMatrix)
	{
		free(pGMM->p);
		free(pGMM->mu);
		return NULL;
	}
	for (i = 0; i < m; ++i)
	{
		pGMM->mu[i] = (double *)ccalloc(D, sizeof(double));
		if (!pGMM->mu[i])
		{
			for(--i; i >=0 ; --i)
			{
				free(pGMM->mu[i]);
				free(pGMM->cMatrix[i]);
			}
			free(pGMM->p);
			free(pGMM->mu);
			free(pGMM->cMatrix);
			return NULL;
		}
		pGMM->cMatrix[i] = (double *)ccalloc(D, sizeof(double));
		if (!pGMM->cMatrix[i])
		{
			free(pGMM->mu[i]);
			for(--i; i >=0 ; --i)
			{
				free(pGMM->mu[i]);
				free(pGMM->cMatrix[i]);
			}
			free(pGMM->p);
			free(pGMM->mu);
			free(pGMM->cMatrix);
			return NULL;
		}
	} // for (i = 0; i < m; ++i)
	return pGMM;
}


void FreeGMM(GMM * pGMM) {
	int i;
	int m = pGMM->m;

	if (!pGMM) {
		return;
	}
	if (pGMM->p) {
		free(pGMM->p);
	}
	if (!pGMM->mu && !pGMM->cMatrix) {
		return;
	}
	for (i = 0; i < m; ++i) {
		if (pGMM->mu && pGMM->mu[i]) {
			free(pGMM->mu[i]);
		}
		if (pGMM->cMatrix && pGMM->cMatrix[i]) {
			free(pGMM->cMatrix[i]);
		}
	}
	if (pGMM->mu) {
		free(pGMM->mu);
	}
	if(pGMM->cMatrix) {
		free(pGMM->cMatrix);
	}
}


static BOOL InitGMM(double ** MFCC, GMM * pGMM, int train_frame_num, int m) {
	int i;
	int j;
	int * clusterIndex = NULL;
	int * clusterSize = NULL;
	double ** sum = NULL;
	GMM gmm = *pGMM;


	clusterIndex = (int *)ccalloc(train_frame_num, sizeof(int));
	assert(clusterIndex != NULL);
	clusterSize = (int *)ccalloc(m, sizeof(int));
	assert(clusterSize != NULL);
    sum = (double **)mmalloc(m * sizeof(double *));
    assert(sum != NULL);
	for (i = 0; i < m; ++i) {
		sum[i] = (double *)ccalloc(D, sizeof(double));
		assert(sum[i] != NULL);
	}
	k_means(MFCC, pGMM, clusterIndex, clusterSize, train_frame_num, m);

	for (i = 0; i < m; ++i) {
		gmm.p[i] = 1.0 / m;
	}

	for (i = 0; i < train_frame_num; ++i) {
		for (j = 0; j < D; ++j) {
			sum[clusterIndex[i]][j] += pow(MFCC[i][j], 2);
		}
	}
	for (i = 0; i < m; ++i) {
		if (clusterSize[i] > 0) {
			for (j = 0; j < D; ++j) {
				gmm.cMatrix[i][j] = sum[i][j] / clusterSize[i] - pow(gmm.mu[i][j], 2);
				if (gmm.cMatrix[i][j] < 0) {
					printf("Initial value of GMM`cMatrix < 0");
					FreeGMM(&gmm);
					free(clusterIndex);
					free(clusterSize);
					for (i = 0; i < m; ++i) {
						free(sum[i]);
					}
					free(sum);
					return FALSE;
				}
				else if(gmm.cMatrix[i][j] < 1e-10) {
					gmm.cMatrix[i][j] = 0;
				}
			}
		}
		else {
			printf("InitGMM: Decrease M and increase frame number");
			printf("clusterSize[%d]: %d\n", i, clusterSize[i]);
			FreeGMM(&gmm);
			free(clusterIndex);
			free(clusterSize);
			for (i = 0; i < m; ++i) {
				free(sum[i]);
			}
			free(sum);
			return FALSE;
		}
	}


	free(clusterIndex);
	free(clusterSize);
	for (i = 0; i < m; ++i) {
		free(sum[i]);
	}
	free(sum);
	return TRUE;
}

static double GMM_diff(GMM *gmm1, GMM *gmm2) {
	int i;
	int j;
	double diff = 0;

	for (i = 0; i < M; ++i) {
		diff += pow(gmm1->p[i] - gmm2->p[i], 2);
		for (j = 0; j < D; ++j) {
			diff += pow(gmm1->mu[i][j] - gmm2->mu[i][j], 2);
			diff += pow(gmm1->cMatrix[i][j] - gmm2->cMatrix[i][j], 2);
		}
	}

	return diff;
}


static double GMM_density(GMM * pGMM, double * X, int index) {
	int i;
	double temp = 0;
	double sqrt_Matrix_value = 1;
	double res = 0;


	for (i = 0; i < D; ++i) {
		temp += pow(X[i] - pGMM->mu[index][i], 2) / pGMM->cMatrix[index][i];
		sqrt_Matrix_value *= sqrt(pGMM->cMatrix[index][i]);
	}
	res = pow(_2PI, D/-2) / sqrt_Matrix_value * exp(temp / -2);
	return res;
}

static BOOL BuildGMM(GMM * pGMM, GMM * Out, double ** X, int train_frame_num, int m) {
	int i;
	int j;
	int k;
	GMM temp_gmm = *Out;
	double ** p = NULL;
	double * sum_p = NULL;
	double * pb = NULL;
	double sum_pb = 0;

	pb = (double *)ccalloc(m, sizeof(double));
	assert(pb != NULL);
	sum_p = (double *)ccalloc(m, sizeof(double));
	assert(sum_p != NULL);
	p = (double **)mmalloc(train_frame_num * sizeof(double *));
	assert(p != NULL);
	for (i = 0; i < train_frame_num; ++i) {
		p[i] = (double *)ccalloc(m, sizeof(double));
		assert(p[i] != NULL);
	}
	for (i = 0; i < train_frame_num; ++i) {
		sum_pb = 0;
		for (j = 0; j < m; ++j) {
			pb[j] = pGMM->p[j] * GMM_density(pGMM, X[i], j);
			sum_pb += pb[j];
		}
		if (sum_pb == 0 || sum_pb != sum_pb ) {
			printf("sum_ph = %lf\n", sum_pb);
			for (i = 0; i < train_frame_num; ++i) {
				free(p[i]);
			}
			free(p);
			free(pb);
			free(sum_p);
			return FALSE;
		}
		for (j = 0; j < m; ++j) {
			p[i][j] = pb[j] / sum_pb; // p(i | x_t, lambda)
			sum_p[j] += p[i][j];
		}
	}

	for (j = 0; j < m; ++j) {
		temp_gmm.p[j] = sum_p[j] / train_frame_num;

		for (i = 0; i < train_frame_num; ++i) {
			for (k = 0; k < D; ++k) {
				if (i == 0) {
					temp_gmm.mu[j][k] = 0;
					temp_gmm.cMatrix[j][k] = 0;
				}
				temp_gmm.mu[j][k] += p[i][j] * X[i][k];
				temp_gmm.cMatrix[j][k] += p[i][j] * pow(X[i][k], 2);
			}
		}
		for (k = 0; k < D; ++k) {
			temp_gmm.mu[j][k] /= sum_p[j]; // mu
			temp_gmm.cMatrix[j][k] /= sum_p[j];
			temp_gmm.cMatrix[j][k] -= pow(temp_gmm.mu[j][k], 2); // sigma^2
			if (temp_gmm.cMatrix[j][k] <= 0) {
				printf("BuildGMM: cMatrix <= 0");
				for (i = 0; i < train_frame_num; ++i) {
					free(p[i]);
				}
				free(p);
				free(pb);
				free(sum_p);
				return FALSE;
			}
		} // for (k = 0; k < D; ++k)
	}

	for (i = 0; i < train_frame_num; ++i) {
		free(p[i]);
	}
	free(p);
	free(pb);
	free(sum_p);
	return TRUE;
}


BOOL GMMs(double **MFCC, GMM * pGMM, int train_frame_num, int m) {
	GMM first_gmm;
	GMM second_gmm;
	GMM * first = &first_gmm;
	GMM * second = &second_gmm;
	GMM * temp = NULL;
	BOOL res;
	double diff = 0;

	InitGMMClass(&first_gmm, m);
	CallocGMM(&first_gmm);
	InitGMMClass(&second_gmm, m);
	CallocGMM(&second_gmm);
	if (!InitGMM(MFCC, first, train_frame_num, m)) {
		return FALSE;
	}

	do {
		res = BuildGMM(first, second, MFCC, train_frame_num, m);
		if (!res) {
			FreeGMM(first);
			FreeGMM(second);
			return FALSE;
		}
		temp = first;
		first = second;
		second = temp;
		diff = GMM_diff(first, second);
		printf("Model Error = %lf\n", diff);
	}
	while (diff >= DIFF_GMM_VALUE);

	FreeGMM(first);
	*pGMM = *second;
	return TRUE;
}


BOOL Identify(double ** X, double * value, GMM * pGMM, int frame_num, int m) {
	int j;
	int k;
	double temp = 0;
	double * pb = NULL;

	double sum_pb = 0;

	pb = (double *)ccalloc(m, sizeof(double));
	assert(pb != NULL);
	for (j = 0; j < frame_num; ++j) {
		sum_pb = 0;
		for (k = 0; k < m; ++k) {
			pb[k] = pGMM[0].p[k] * GMM_density(pGMM, X[j], k);
			sum_pb += pb[k];
		}
		if (sum_pb <= 0) {
			printf("sum_pb <= 0\n");
			return FALSE;
		}
		else {
			temp += log(sum_pb);
		}
	}

	*value = temp;
	free(pb);
	return TRUE;
}

