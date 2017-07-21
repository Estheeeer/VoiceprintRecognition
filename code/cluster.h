#ifndef _CLUSTER_H_
#define _CLUSTER_H_

#include "gmm.h"
#include <stdlib.h>
void k_means(double ** c_vector, GMM * pGMM, int * clusterIndex,  int * cSize, int train_frame_num, int m);

#endif /* _CLUSTER_H_ */

