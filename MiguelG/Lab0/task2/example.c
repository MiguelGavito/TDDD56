#include <pthread.h>

void *foo( void* vp){
  int i = (int)vp;
  return &i;
}

int main(int argc, char *argv[])
{
  int *ptr;
  pthread_t thr;

  pthread_create(&thr, NULL, foo , (void*)ptr);

  pthread_join(thr,NULL);

  return 0;
}
