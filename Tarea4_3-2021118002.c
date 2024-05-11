#include <stdio.h>
#include <unistd.h>

typedef long Align; /* Alineamiento al límite superior */

union header { /* Encabezado de bloque */
    struct {
        union header *ptr; /* Siguiente bloque */
        unsigned size; /* Tamaño del bloque */
    } s;
    Align x; /* Alineamiento de bloque */
};

typedef union header Header;

static Header base; /* Lista vacía al inicio */
static Header *freep = NULL; /* Lista libre */
static Header *lastp = NULL; /* Último bloque visitado */

/* free: coloca el bloque ap en la lista vacía */
void free(void *ap) {
    Header *bp, *p;

    bp = (Header *)ap - 1; /* Apunta al encabezado de un bloque */
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break; /* Libera bloque al inicio o al final */

    if (bp + bp->s.size == p->s.ptr) { /* Fusiona al nbr superior */
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    } else {
        bp->s.ptr = p->s.ptr;
    }
    if (p + p->s.size == bp) {
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    } else {
        p->s.ptr = bp;
    }
    freep = p;
}

#define NALLOC 1024 /* Mínimo de unidades por requerir */

/* morecore: solicita más memoria del sistema */
static Header *morecore(unsigned nu) {
    char *cp;
    Header *up;

    if (nu < NALLOC)
        nu = NALLOC;
    cp = sbrk(nu * sizeof(Header));
    if (cp == (char *)-1) /* No hay espacio */
        return NULL;
    up = (Header *)cp;
    up->s.size = nu;
    free((void *)(up + 1));
    return freep;
}

/* malloc: asignador de memoria con estrategia de siguiente ajuste */
void *malloc(unsigned long nbytes) {
    Header *p, *prevp;
    unsigned nunits;

    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    if ((prevp = lastp) == NULL) { /* No hay lista libre */
        base.s.ptr = freep = prevp = lastp = &base;
        base.s.size = 0;
    }
    for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) { /* Suficientemente grande */
            if (p->s.size == nunits) /* Exacto */
                prevp->s.ptr = p->s.ptr;
            else { /* Reducir el bloque excedente */
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            lastp = prevp;
            return (void *)(p + 1);
        }
        if (p == lastp) /* Dio la vuelta a la lista libre */
            if ((p = morecore(nunits)) == NULL)
                return NULL; /* No quedó memoria */
    }
}

/* Programa de prueba */
int main() {
    printf("Test de malloc con estrategia de siguiente ajuste:\n");
    int *array1 = malloc(100 * sizeof(int));
    int *array2 = malloc(50 * sizeof(int));
    printf("array1 asignado en: %p\n", array1);
    printf("array2 asignado en: %p\n", array2);
    free(array1);
    int *array3 = malloc(25 * sizeof(int));
    printf("array3 asignado en: %p\n", array3);
    free(array2);
    free(array3);
    return 0;
}
