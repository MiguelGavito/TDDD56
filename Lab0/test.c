#include <stdio.h>
#include <stdlib.h>

typedef struct TrioT{
    int primero;
    int segundo;
    int tercero;

    struct TrioT *siguiente;
} TrioT;

int main(void){
    int x = 10;

    int *p = &x;

    int *A;

    int AB[5] = {1, 2, 3, 4, 5};

    A = malloc(5 * sizeof(int));  

    printf("x:%d\n",x);
    printf("p:%p\n",(void *)p);
    printf("A+0 = %p\n", (void*)A+0);
    printf("AB+0 = %p\n", (void*)AB+0);

    printf("AB+1 = %p\n", (void*)AB+1);

    int a = 5;
    int b = 7;
    int c = 8;

    
    
    TrioT *nodo1 = malloc(sizeof(TrioT));
    TrioT *nodo2 = malloc(sizeof(TrioT));
    TrioT *nodo3 = malloc(sizeof(TrioT));

    if (nodo1 == NULL || nodo2 == NULL || nodo3 == NULL) {
        free(nodo1);
        free(nodo2);
        free(nodo3);
        return 1;
    }

    // Asignar valores
    *nodo1 = (TrioT){5, 7, 8, nodo2};
    *nodo2 = (TrioT){2, 4, 6, nodo3};
    *nodo3 = (TrioT){1, 3, 9, NULL};

    // Recorrer nodos
    TrioT *actual = nodo1;

    while (actual != NULL) {

        printf("%d %d %d\n",
            actual->primero,
            actual->segundo,
            actual->tercero
        );

        actual = actual->siguiente;
    }

    // Liberar memoria
    free(nodo1);
    free(nodo2);
    free(nodo3);

    return 0;
}
