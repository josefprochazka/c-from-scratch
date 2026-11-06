/*
 * stack.h – HLAVIČKA zásobníku.
 *
 * Hlavička jen oznamuje, CO zásobník umí (typ Stack a seznam funkcí).
 * JAK to dělá, je v stack.c. Kdo chce zásobník použít, napíše
 * #include "stack.h" – podobně jako using / interface v C#.
 */

/* "Include guard": kdyby se stack.h vložil do jednoho souboru dvakrát,
 * podruhé se díky tomu přeskočí (jinak by překladač hlásil, že Stack
 * je definovaný dvakrát). */
#ifndef STACK_H
#define STACK_H

#include <stdbool.h>   /* typ bool, true, false */

/* Kolik místa si zásobník vezme na začátku. Schválně málo,
 * ať je hned vidět, jak se zvětšuje. */
#define STACK_START_CAPACITY 2

/* Zásobník = ukazatel na pole na HALDĚ + kolik prvků v něm je
 * + kolik se jich tam vejde, než bude potřeba víc paměti. */
typedef struct {
    int *items;     /* pole z malloc – na začátku neexistuje, vznikne v stack_init */
    int  count;     /* kolik prvků v zásobníku je */
    int  capacity;  /* kolik prvků se vejde do aktuálně alokované paměti */
} Stack;

/* Jen deklarace (hlavičky funkcí se středníkem, bez těla).
 * Těla funkcí jsou v stack.c. */
bool stack_init(Stack *s);   /* false = došla paměť */
void stack_free(Stack *s);   /* vrátí paměť – NUTNO zavolat na konci! */
bool stack_is_empty(const Stack *s);
bool stack_push(Stack *s, int value);
bool stack_pop(Stack *s, int *out);
bool stack_peek(const Stack *s, int *out);
void stack_print(const Stack *s);

#endif /* STACK_H */
