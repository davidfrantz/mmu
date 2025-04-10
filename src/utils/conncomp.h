/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Connected components header
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/


#ifndef CONNCOMP_H
#define CONNCOMP_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>  


#ifdef __cplusplus
extern "C" {
#endif

int connectedcomponents_(bool *image, int *CCL, off_t nx, off_t ny);
void ccl_tracer(off_t *cy, off_t *cx, int *dir, bool *image, int *CCL, off_t nx, off_t ny);
void ccl_contourtrace(off_t cy, off_t cx, int label, int dir, bool *image, int *CCL, off_t nx, off_t ny);

#ifdef __cplusplus
}
#endif

#endif

