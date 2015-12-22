#include <stdio.h>

#include "pthread.h"
    

int main(void){

  printf("%f\n", compute_pi_by_pthread((const int)100));
  return 0;
}
