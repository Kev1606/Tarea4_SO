#include <stdio.h>
#define ALLOCSIZE 10000 // tamaño del espacio disponible

static char allocbuf[ALLOCSIZE]; // almacenamiento para alloc
static char *allocp1 = allocbuf; // siguiente posición libre para i=1 (arriba hacia abajo)
static char *allocp2 = allocbuf + ALLOCSIZE; // siguiente posición libre para i=2 (abajo hacia arriba)

char *alloc(int n, int i) {
    if (i == 1) { // Asignación de arriba hacia abajo
        if (allocp1 + n <= allocp2) {
            allocp1 += n;
            return allocp1 - n;
        } else {
            return 0; // No hay espacio suficiente
        }
    } else if (i == 2) { // Asignación de abajo hacia arriba
        if (allocp2 - n >= allocp1) {
            allocp2 -= n;
            return allocp2;
        } else {
            return 0; // No hay espacio suficiente
        }
    }
    return 0; // i no es ni 1 ni 2
}

void afree(char *p) {
    if (p >= allocbuf && p < allocbuf + ALLOCSIZE) {
        if (p < allocp2) // Si p está en la mitad inferior de la pila
            allocp1 = p;
        else // Si p está en la mitad superior de la pila
            allocp2 = p + sizeof(p);
    }
}

int main() {
    char *p1, *p2, *p3, *p4;

    // Prueba de asignación en la dirección de arriba hacia abajo
    p1 = alloc(1000, 1);
    if (p1 != 0)
        printf("Asignación exitosa de 1000 caracteres en dirección 1. Dirección: %p\n", p1);
    else
        printf("Asignación fallida en dirección 1.\n");

    // Prueba de asignación en la dirección de abajo hacia arriba
    p2 = alloc(1000, 2);
    if (p2 != 0)
        printf("Asignación exitosa de 1000 caracteres en dirección 2. Dirección: %p\n", p2);
    else
        printf("Asignación fallida en dirección 2.\n");

    // Intento de asignar más memoria de la disponible
    p3 = alloc(9000, 1);
    if (p3 != 0)
        printf("Asignación exitosa de 9000 caracteres en dirección 1. Dirección: %p\n", p3);
    else
        printf("Asignación fallida en dirección 1 (memoria insuficiente).\n");

    // Liberar memoria y reintentar asignación
    afree(p1);
    printf("Memoria liberada en dirección: %p\n", p1);

    p4 = alloc(500, 1);
    if (p4 != 0)
        printf("Asignación exitosa de 500 caracteres en dirección 1 después de liberar. Dirección: %p\n", p4);
    else
        printf("Asignación fallida en dirección 1 después de liberar.\n");

    // Verificar el estado final de los punteros de asignación
    printf("Estado final de allocp1: %p, allocp2: %p\n", allocp1, allocp2);

    return 0;
}
