#include <stdio.h>

#include "pthread.h"
    

int main(int argc, char **argv){
  if(argc > 1){
    find_max_with_pthread(atoi(argv[1]));
  }
  return 0;
}
