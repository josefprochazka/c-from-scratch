/*
 * arena.c – implementace jednoduchého arena alokátoru.
 */
#include "arena.h"
#include "memview.h"
#include "util.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool arena_init(Arena *arena, size_t capacity)
{
    arena->base = malloc(capacity);   /* jediný malloc pro celou arénu */
    arena->capacity = (arena->base != NULL) ? capacity : 0;
    arena->offset = 0;
    return arena->base != NULL;
}

void *arena_alloc(Arena *arena, size_t size, size_t align)
{
    /* Mocnina dvojky má právě jeden bit nastavený: 8 = 0b1000, 7 = 0b0111,
     * takže  8 & 7 == 0. Klasický bitový trik. */
    if (align == 0 || (align & (align - 1)) != 0) {
        return NULL;
    }

    /* Zarovnání nahoru na násobek 'align':
     *   (adresa + align - 1) & ~(align - 1)
     * např. adresa 13, align 8:  (13 + 7) & ~7 = 20 & ...11111000 = 16 */
    uintptr_t current = (uintptr_t)(arena->base + arena->offset);
    uintptr_t aligned = (current + (align - 1)) & ~(uintptr_t)(align - 1);
    size_t padding = (size_t)(aligned - current);

    if (padding + size > arena->capacity - arena->offset) {
        return NULL;   /* došlo místo */
    }

    arena->offset += padding + size;
    return (void *)aligned;
}

void arena_reset(Arena *arena)
{
    arena->offset = 0;   /* to je celé "uvolnění" :) */
}

void arena_destroy(Arena *arena)
{
    free(arena->base);
    arena->base = NULL;
    arena->capacity = 0;
    arena->offset = 0;
}

/* Pomocník pro demo: alokuje a vypíše, co se stalo. */
static void *alloc_verbose(Arena *arena, size_t size, size_t align, const char *what)
{
    size_t before = arena->offset;
    void *ptr = arena_alloc(arena, size, align);

    if (ptr == NULL) {
        printf("  %-22s %4zu B  align %2zu  -> NULL (v aréně není místo)\n",
               what, size, align);
    } else {
        size_t start = (size_t)((unsigned char *)ptr - arena->base);
        printf("  %-22s %4zu B  align %2zu  -> offset %4zu  (výplň %zu B)  %p\n",
               what, size, align, start, start - before, ptr);
    }
    return ptr;
}

typedef struct {
    float x, y, z;
} Vec3;

void demo_arena(void)
{
    print_header("4) Vlastní alokátor – arena");

    Arena arena;
    if (!arena_init(&arena, 1024)) {
        fprintf(stderr, "malloc selhal\n");
        return;
    }
    printf("Aréna: 1 malloc na %zu B, začátek na %p\n\n", arena.capacity, (void *)arena.base);

    /* _Alignof (C11) říká, na jakou adresu musí typ začínat. */
    char *name = alloc_verbose(&arena, 6, _Alignof(char), "char[6] \"Cecko\"");
    if (name != NULL) {
        strcpy(name, "Cecko");
    }

    double *d = alloc_verbose(&arena, sizeof(double), _Alignof(double), "double");
    if (d != NULL) {
        *d = 2.71828;
    }

    Vec3 *points = alloc_verbose(&arena, 10 * sizeof(Vec3), _Alignof(Vec3), "Vec3[10]");
    if (points != NULL) {
        for (int i = 0; i < 10; i++) {
            points[i].x = (float)i;
            points[i].y = (float)(i * i);
            points[i].z = 0.0f;
        }
    }

    alloc_verbose(&arena, 1, 1, "char");
    alloc_verbose(&arena, 64, 16, "buffer 64 B (align 16)");
    alloc_verbose(&arena, 2000, 8, "obří blok 2000 B");

    printf("\nVyužito %zu z %zu B.\n", arena.offset, arena.capacity);
    if (name != NULL && d != NULL && points != NULL) {
        printf("Data jsou normálně použitelná: name = \"%s\", d = %.5f, points[3] = (%.0f, %.0f, %.0f)\n",
               name, *d, points[3].x, points[3].y, points[3].z);
    }

    /* Výplň mezi daty je neinicializovaná – v Debug MSVC uvidíš CD. */
    printf("\nPrvních 48 bajtů arény (vidíš \"Cecko\", výplň a double):\n");
    hexdump(arena.base, 48);

    arena_reset(&arena);
    printf("\narena_reset: offset = %zu. Všechno \"uvolněno\" jedním přiřazením,\n"
           "bez desítek volání free(). Paměť se použije znovu.\n", arena.offset);

    arena_destroy(&arena);
    printf("arena_destroy: jediný free() celého bloku.\n");
}
