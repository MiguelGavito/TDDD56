#include <stdio.h>
#include <stdlib.h>
int add(int a, int b){
  return a + b;
}

void add_to(int* target, int value){
  *target += value;
}

void reset(int* target){
  *target = 0;
}

int add_and_clean(int *a, int *b){
  int res = *a + *b;
  printf("a,b = %p,%p\n", a,b);
  printf("*a,*b = %d,%d\n", *a,*b);
  *a = 0;
  *b = 0;

  return res;

}
int main(int argc, char *argv[])
{
  int a = 1;
  int b = 3;
  int c = 5;
  
  int n = add(a,b);
  printf("n:%d\n",n);
  printf("a:%d\nb:%d\n",a,b);

  add_to(&a, c);
  n = add(a,b);

  printf("n:%d\n",n);
  printf("a:%d\nb:%d\n",a,b);

  return 0;
}
