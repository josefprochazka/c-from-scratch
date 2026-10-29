/*
 * vector.h – dynamické pole intů, které samo roste (jako std::vector / List<T>).
 */
#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    int    *data;      /* blok na haldě */
    size_t  size;      /* kolik prvků je použito */
    size_t  capacity;  /* pro kolik prvků je místo */
} IntVector;

void vec_init(IntVector *vec);
bool vec_push(IntVector *vec, int value);   /* false = došla paměť */
void vec_free(IntVector *vec);

/* Demo 3: malloc/calloc/realloc/free + qsort s ukazateli na funkce. */
void demo_vector(void);

#endif /* VECTOR_H */
