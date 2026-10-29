/*
 * vector.c – dynamické pole: realloc, kapacita, qsort s function pointery.
 */
#include "vector.h"
#include "util.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void vec_init(IntVector *vec)
{
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
}

bool vec_push(IntVector *vec, int value)
{
    if (vec->size == vec->capacity) {
        /* Kapacitu zdvojnásobujeme -> v průměru O(1) na jeden push. */
        size_t new_capacity = (vec->capacity == 0) ? 4 : vec->capacity * 2;

        /* Starou adresu si uložíme jako ČÍSLO. Po úspěšném realloc už starý
         * ukazatel nesmíme ani porovnávat – mohl být uvolněn. */
        uintptr_t old_address = (uintptr_t)vec->data;

        /* POZOR: nikdy nepiš  vec->data = realloc(vec->data, ...);
         * Když realloc selže, vrátí NULL a ty ztratíš jediný odkaz na starý
         * blok -> memory leak. Proto výsledek nejdřív do pomocné proměnné. */
        int *new_data = realloc(vec->data, new_capacity * sizeof *new_data);
        if (new_data == NULL) {
            return false;   /* starý blok vec->data je stále platný */
        }

        /* Výpis jen pro účely dema – ať je vidět, co realloc dělá. */
        printf("    realloc: kapacita %2zu -> %2zu  (%3zu B)  blok %s\n",
               vec->capacity, new_capacity, new_capacity * sizeof(int),
               ((uintptr_t)new_data == old_address) ? "zůstal na místě"
                                                    : "se PŘESTĚHOVAL");

        vec->data = new_data;
        vec->capacity = new_capacity;
    }

    vec->data[vec->size++] = value;
    return true;
}

void vec_free(IntVector *vec)
{
    free(vec->data);   /* free(NULL) je v pořádku, nic neudělá */
    vec_init(vec);     /* uvedeme strukturu do čistého stavu */
}

static void vec_print(const IntVector *vec)
{
    printf("  [");
    for (size_t i = 0; i < vec->size; i++) {
        printf("%s%d", (i == 0) ? "" : ", ", vec->data[i]);
    }
    printf("]\n");
}

/* Porovnávací funkce pro qsort. qsort neví, jaký typ třídí – dostává
 * void* (ukazatel na "cokoliv") a my si ho přetypujeme. */
static int compare_ascending(const void *a, const void *b)
{
    int x = *(const int *)a;
    int y = *(const int *)b;
    /* Ne "return x - y;" – to může přetéct u velkých čísel. */
    return (x > y) - (x < y);
}

static int compare_descending(const void *a, const void *b)
{
    return compare_ascending(b, a);
}

void demo_vector(void)
{
    print_header("3) Dynamické pole – malloc / realloc / free");

    IntVector vec;
    vec_init(&vec);

    srand((unsigned)time(NULL));

    printf("Přidávám 20 náhodných čísel, sleduj, kdy se volá realloc:\n");
    for (int i = 0; i < 20; i++) {
        if (!vec_push(&vec, rand() % 100)) {
            fprintf(stderr, "Došla paměť!\n");
            vec_free(&vec);
            return;
        }
    }

    printf("\nObsah (size = %zu, capacity = %zu):\n", vec.size, vec.capacity);
    vec_print(&vec);

    /* Ukazatel na funkci: proměnná, která obsahuje adresu funkce.
     * Syntaxe: návratový_typ (*jméno)(parametry) */
    int (*comparator)(const void *, const void *);

    comparator = compare_ascending;
    qsort(vec.data, vec.size, sizeof vec.data[0], comparator);
    printf("\nqsort s compare_ascending:\n");
    vec_print(&vec);

    comparator = compare_descending;
    qsort(vec.data, vec.size, sizeof vec.data[0], comparator);
    printf("qsort s compare_descending:\n");
    vec_print(&vec);

    /* Průchod polem čistě přes ukazatele – takhle "myslí" procesor. */
    long sum = 0;
    for (const int *it = vec.data; it != vec.data + vec.size; ++it) {
        sum += *it;
    }
    printf("\nSoučet (průchod ukazatelem it++): %ld\n", sum);

    /* calloc = malloc + vynulování. Parametry: počet prvků, velikost prvku. */
    int *zeros = calloc(8, sizeof *zeros);
    if (zeros != NULL) {
        printf("\ncalloc(8, sizeof(int)) – paměť je zaručeně vynulovaná:\n  ");
        for (int i = 0; i < 8; i++) {
            printf("%d ", zeros[i]);
        }
        printf("\n");
        free(zeros);
    }

    vec_free(&vec);
    printf("\nvec_free: paměť vrácena systému (data = %p).\n", (void *)vec.data);
}
