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

/* Maximální počet prvků. #define je jen textová náhrada:
 * překladač všude místo STACK_SIZE vidí 5. */
#define STACK_SIZE 5

/* Zásobník = pole na data + číslo, kolik prvků v něm zrovna je. */
typedef struct {
    int items[STACK_SIZE];
    int count;              /* 0 = prázdný, STACK_SIZE = plný */
} Stack;

/* Jen deklarace (hlavičky funkcí se středníkem, bez těla).
 * Těla funkcí jsou v stack.c. */
void stack_init(Stack *s);
bool stack_is_empty(const Stack *s);
bool stack_is_full(const Stack *s);
bool stack_push(Stack *s, int value);
bool stack_pop(Stack *s, int *out);
bool stack_peek(const Stack *s, int *out);
void stack_print(const Stack *s);

#endif /* STACK_H */
