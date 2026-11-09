#include <stdio.h>
#include <stdlib.h>
int *create_array(int size){
  int *array;

  array = malloc(size * sizeof(int)) ;
  return array;
}

void fill_array(int *array, int size){
  for (int i = 0; i < size; i++) {
    array[i] = i;
  }
}

void print_array(int *array, int size){
  for (int i = 0; i < size; i++) {
    printf("%d\t",array[i]);
    array[i] = 0; 
  }
  free(array);
  printf("\n");
}

int main(int argc, char *argv[])
{
  /*Create array
   * Fill Array
   * Print Array
   */
  int *array = create_array(3);

  fill_array(array, 3);

  print_array(array, 4);
  printf("array %d\n",array[0]);
  return 0;
}
