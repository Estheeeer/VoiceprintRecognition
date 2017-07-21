#include "cluster.h"

#define DIFF_CENTER_VALUE  1

inline double getDistance(double * p1, double * p2) {
	int i = 0;
	double distance = 0;

	while (i < D) {
		distance += pow(p1[i]-p2[i], 2);
		++i;
	}
	return distance;
}

void k_means(double ** c_vector, GMM * pGMM, int * clusterIndex,  int * cSize, int train_frame_num, int m) {
	int i;
	int j;
	int k;
	int farIndex = 0;
	int * clusterSize = NULL;
	double maxDistance = 0;
	double minDistance = 0;
	double distance = 0;
	double ** mCenter       = NULL;
	double diff_Center       = 0;
    GMM gmm = *pGMM;

    clusterSize = (int *)ccalloc(m, sizeof(int));
    assert(clusterSize != NULL);
    mCenter = (double **)mmalloc(m * sizeof(double *));
    assert(mCenter != NULL);
	for (i = 0; i < m; ++i) {
		mCenter[i] = (double *)ccalloc(D, sizeof(double));
		assert(mCenter[i] != NULL);
	}


	for (i = 0; i < D; ++i) {
		for(j = 0; j < train_frame_num; ++j) {
			gmm.mu[0][i] += c_vector[j][i];
		}
		gmm.mu[0][i] /= train_frame_num;
	}

	for (k = 1; k < m; ++k) {
		farIndex = 0;
		maxDistance = 0;
		for (j = 0; j < train_frame_num; ++j) {
			distance  = 0;
			for (i = 0; i < k; ++i) {
				distance += sqrt(getDistance(gmm.mu[i], c_vector[j]));
			}
			if (distance > maxDistance) {
				maxDistance = distance;
				farIndex = j;
			}
		}
		memcpy(gmm.mu[k], c_vector[farIndex], D*sizeof(double));


NEXTLOOP:
		for (j = 0; j < k+1; ++j) {
			memcpy(mCenter[j], gmm.mu[j], D*sizeof(double));
		}

		for (j = 0; j < train_frame_num; ++j) {
			farIndex = 0;
			minDistance = getDistance(mCenter[0], c_vector[j]);
			for (i = 1; i <= k; ++i) {
				distance = getDistance(mCenter[i], c_vector[j]);
				if (distance < minDistance) {
					minDistance = distance;
					farIndex = i;
				}
			}
			if (k == m - 1) {
				clusterIndex[j] = farIndex;
			}
			clusterSize[farIndex] += 1;
			for (i = 0; i < D; ++i) {
				if (clusterSize[farIndex] == 1) {
					gmm.mu[farIndex][i] = 0;
				}
				gmm.mu[farIndex][i] += c_vector[j][i];
			}

		}

		if (k == m - 1) {
			memcpy(cSize, clusterSize, m * sizeof(int));
		}

		diff_Center = 0;
		for (i = 0; i <= k; ++i) {
			double temp = 0;
			if (clusterSize[i] > 0) {
				for (j = 0; j < D; ++j) {
					gmm.mu[i][j] /= clusterSize[i];
					temp += pow(gmm.mu[i][j] - mCenter[i][j], 2);
				}
				diff_Center += sqrt(temp);
			}
			clusterSize[i] = 0;
		}
		if (diff_Center >= DIFF_CENTER_VALUE) {
			goto NEXTLOOP;
		}

	}  // end for (k = 1; k < M; ++k)

	free(clusterSize);
	for (i = 0; i < m; ++i) {
		free(mCenter[i]);
	}
	free(mCenter);
}
