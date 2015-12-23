#ifndef __PTHREAD_TEMPLATE_H__
#define __PTHREAD_TEMPLATE_H__

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

/* struct for arguments for threads */
typedef struct _thread_func_param
{
  int *data;
  int thread_id;
  int begin;
  int end;
  int **max;
  int **argmax;
  int *selected;
  pthread_mutex_t* mutex;  
} thread_func_param;

/* thread function */
void* thread_func(void* arg)
{
  thread_func_param* param = (thread_func_param*)arg;
  int i = 0;
  int thread_argmax = param->begin;
  int thread_max = param->data[thread_argmax];

  for (i = param->begin + 1; i <= param->end; i++) {
    if((param->data[i] > thread_max) && (param->selected[i] == 0)){
      thread_max = param->data[i];
      thread_argmax = i;
    }
  }

  (*(param->max))[param->thread_id] = thread_max;
  (*(param->argmax))[param->thread_id] = thread_argmax;

  pthread_mutex_lock(param->mutex);
  /* some mutual exclusive code */
  pthread_mutex_unlock(param->mutex);
  printf("thread %d\t%d\t%d\n", param->thread_id, thread_max, thread_argmax);

  return NULL;
}

/* find max and argmax of an array of int */
int find_max_with_pthread(const int array_length)
{
  int t;
  int T = 10;

  /* # of threads := # of processors */
  int num_of_threads = (int)sysconf(_SC_NPROCESSORS_ONLN);

  fprintf(stderr, "# of threads = %d\n", num_of_threads);

  int index = 0;

  /* sample data; array for the results */
  int data[array_length];
  for(index = 0; index < array_length; index++){
    data[index] = index;
  }

  int *max = calloc(sizeof(int), num_of_threads);
  int *argmax = calloc(sizeof(int), num_of_threads);
  int *selected = calloc(sizeof(int), array_length);

  int max_all, argmax_all;

  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  thread_func_param* params = NULL;
  pthread_t* threads = NULL;

  pthread_mutex_init(&mutex, NULL);

  /* set variables */
  params = (thread_func_param*)malloc(sizeof(thread_func_param) * num_of_threads);
  for (index = 0; index < num_of_threads; index++) {
    params[index].data = data;
    params[index].thread_id = index;
    params[index].begin = (index == 0)? 0 : params[index - 1].end + 1;
    params[index].end = ((index == num_of_threads - 1)
			 ? array_length - 1
			 : (array_length / num_of_threads * (index + 1) - 1));
    params[index].max = &max;
    params[index].argmax = &argmax;
    params[index].selected = selected;
    params[index].mutex = &mutex;
  }

  for(t = 0; t < T; t++){  
    /* generate threads */
    threads = (pthread_t*)malloc(sizeof(pthread_t) * num_of_threads);
    for (index = 0; index < num_of_threads; index++) {
      pthread_create(&threads[index], NULL, thread_func, (void*)&params[index]);
    }
    
    /* wait for threads */
    for (index = 0; index < num_of_threads; index++) {
      pthread_join(threads[index], NULL);
    }
    
    /* dump results for each thread */
    for(index = 0; index < num_of_threads; index++){
      printf("%d\t%d\t%d\n", index, max[index], argmax[index]);
    }
    
    /* compute final results */
    max_all = max[0];
    argmax_all = argmax[0];
    
    for(index = 1; index < num_of_threads; index++){
      if(max[index] > max_all){
	max_all = max[index];
	argmax_all = argmax[index];
      }
    }
    selected[argmax_all] = 1;

    /* dump final results */
    printf("t = %d:\t%d @ %d\n", t, max_all, argmax_all);
  }


  free(threads);
  free(params);

  pthread_mutex_destroy(&mutex);


  free(max);
  free(argmax);
  free(selected);

  return 0;
}

#endif
