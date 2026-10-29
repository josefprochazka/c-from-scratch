/*
 * memview.h – "paměť pod lupou": adresy, bajty, ukazatele, zarovnání.
 */
#ifndef MEMVIEW_H
#define MEMVIEW_H

#include <stddef.h>

/* Vypíše libovolný kus paměti jako hex + ASCII (jako hex editor). */
void hexdump(const void *data, size_t size);

/* Demo 1: kde v paměti žijí proměnné, sizeof, endianita, hexdump. */
void demo_memory_layout(void);

/* Demo 2: ukazatele, pointerová aritmetika, padding ve strukturách. */
void demo_pointers(void);

#endif /* MEMVIEW_H */
