/*
 * arena.h – vlastní alokátor typu "arena" (taky bump allocator).
 *
 * Princip: jednou si řekneme o velký blok paměti (jeden malloc) a pak z něj
 * "ukrajujeme" posouváním ukazatele. Uvolňuje se všechno najednou.
 * Používají to hry, překladače, webové servery (paměť na jeden request) ...
 */
#ifndef ARENA_H
#define ARENA_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    unsigned char *base;      /* začátek bloku */
    size_t         capacity;  /* celková velikost */
    size_t         offset;    /* kolik bajtů je už "ukrojeno" */
} Arena;

bool  arena_init(Arena *arena, size_t capacity);

/* align musí být mocnina dvojky (1, 2, 4, 8, 16 ...). Vrací NULL, když dojde místo. */
void *arena_alloc(Arena *arena, size_t size, size_t align);

/* Zapomene všechny alokace – paměť jde použít znovu. O(1)! */
void  arena_reset(Arena *arena);

void  arena_destroy(Arena *arena);

/* Demo 4 */
void demo_arena(void);

#endif /* ARENA_H */
