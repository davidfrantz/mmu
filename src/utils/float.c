/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
This file contains some float functions
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/


#include "float.h"


/** Equality test for floats
+++ This function tests for quasi equality of floats
--- a:      number 1
--- b:      number 2
+++ Return: true/false
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/
bool fequal(float a, float b){
float diff, max, A, B;

  diff = fabs(a-b);
  A = fabs(a);
  B = fabs(b);

  max = (B > A) ? B : A;

  if (diff <= max * FLT_EPSILON) return true;

  return false;
}

