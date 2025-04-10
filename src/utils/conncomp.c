/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
This file contains functions for connected components segmenting
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/


#include "conncomp.h"


/** This function performs connected components labeling, i.e. it segments
+++ all 8-connected TRUE patches. A unique ID is given to each segment, 
+++ starting at top-left.
+++-----------------------------------------------------------------------
+++ Chang, F., Chen, C.-J., Lu, C.-J. (2004). A linear-time component-la-
+++ beling algorithm using contour tracing technique. Computer Vision and
+++ Image Understanding, 93 (2), 206-220.
+++-----------------------------------------------------------------------
--- image:  Binary image (only use with 0/1)
--- CCL:    Connected components
--- nx:     number of columns
--- ny:     number of rows
+++ Return: number of segments
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/
int connectedcomponents_(bool *image, int *CCL, off_t nx, off_t ny){
off_t i, j, p;
int dir;
int k = 0, label = 0;


  for (i=0, p=0; i<ny; i++){
  for (j=0, label=0; j<nx; j++, p++){


    if (image[p]){

      if (label != 0){ // use last label

        CCL[p] = label;

      } else {

        label = CCL[p];

        if (label == 0){

          label = ++k;
          if (k == UINT_MAX) printf("hit int limit!\n");
          dir = 0;
          ccl_contourtrace(i, j, label, dir, image, CCL, nx, ny); // external contour
          CCL[p] = label;

        }

      }

    } else if (label != 0){ // if last pixel was labeled

      if (CCL[p] == 0){
        dir = 1;
        ccl_contourtrace(i, j-1, label, dir, image, CCL, nx, ny); // internal contour
      }

      label = 0;

    }

  }
  }


  // replace -1 with 0
  for (p=0; p<nx*ny; p++){
    if (CCL[p] < 0) CCL[p] = 0;
  }

  printf("%d objects detected\n", k);

  return(k);
}


/** Tracer Function for connected components labeling
+++ This function is used in connectedcomponents
+++--------------------------------------------------------------------**/
void ccl_tracer(off_t *cy, off_t *cx, int *dir, bool *image, int *CCL, off_t nx, off_t ny){
int i, tval;
off_t y, x;
static int neighbor[8][2] = {{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};


  for (i=0; i<7; i++){

    y = *cy + neighbor[*dir][0];
    x = *cx + neighbor[*dir][1];

    if (y>=0 && y<ny && x>=0 && x<nx){

      tval = image[y*nx+x];
      
    } else {

      tval = 0;

    }

    if (tval == 0){

      if (y>=0 && y<ny && x>=0 && x<nx) CCL[y*nx+x] = -1;
      *dir = (*dir + 1) % 8;

    } else {

      *cy = y;
      *cx = x;
      break;

    }

  }

  return;
}


/** Contour tracing Function for connected components labeling
+++ This function is used in connectedcomponents
+++--------------------------------------------------------------------**/
void ccl_contourtrace(off_t cy, off_t cx, int label, int dir, 
                      bool *image, int *CCL, off_t nx, off_t ny){
bool stop = false, search = true;
off_t fx, fy, sx = cx, sy = cy;

  ccl_tracer(&cy, &cx, &dir, image, CCL, nx, ny);

  if (cx != sx || cy != sy){

    fx = cx;
    fy = cy;

    while (search){

      dir = (dir + 6) % 8;
      CCL[cy*nx+cx] = label;
      ccl_tracer(&cy, &cx, &dir, image, CCL, nx, ny);

      if (cx == sx && cy == sy){

        stop = true;

      } else if (stop){

        if (cx == fx && cy == fy){
          search = false;
        } else {
          stop = false;
        }

      }

    }

  }

  return;
}




/**
int binary_to_objects(bool *image, int nx, int ny, int nmin, int **OBJ, int **SIZE, int *nobj){
int *CCL = NULL; // connected component labelling
int *CCS = NULL; // connected component object size
int id, nc, p, no;


  #ifdef FORCE_CLOCK
  time_t TIME; time(&TIME);
  #endif
  
  
  nc = nx*ny;

  alloc((void**)&CCL, nc, sizeof(int));

  // to ensure that 1st object gets ID=1
  image[0] = false;

  if ((no = connectedcomponents_(image, CCL, nx, ny)) > 0){

    alloc((void**)&CCS, no, sizeof(int));

    for (p=0; p<nc; p++){
      if (!image[p]) continue;
      id = CCL[p]; 
      CCS[id-1]++;
    }

    for (p=0; p<nc; p++){
      if (!image[p]) continue;
      id = CCL[p];
      if (CCS[id-1] < nmin) image[p] = 0;
    }

    for (p=0; p<nc; p++) CCL[p] = 0;
    free((void*)CCS);

    if ((no = connectedcomponents_(image, CCL, nx, ny)) > 0){

      alloc((void**)&CCS, no, sizeof(int));

      for (p=0; p<nc; p++){
        if (!image[p]){ CCL[p] = 0; continue;}
        id = CCL[p]; 
        CCS[id-1]++;
      }

    } else free((void*)CCL);

  } else free((void*)CCL);

  
  #ifdef FORCE_CLOCK
  proctime_print("binary to objects", TIME);
  #endif
  
  *nobj = no;
  *OBJ  = CCL;
  *SIZE = CCS;
  return 0;
}
**/
