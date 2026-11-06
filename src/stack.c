/*
 * stack.c – IMPLEMENTACE zásobníku (těla funkcí ze stack.h).
 *
 * Zásobník je jako hromádka talířů: nový talíř dáš vždycky NAHORU
 * a bereš taky vždycky ten HORNÍ. Říká se tomu LIFO
 * (Last In, First Out – co přišlo poslední, odchází první).
 *
 * Operace:
 *   push – vlož prvek nahoru
 *   pop  – vyndej horní prvek
 *   peek – jen se podívej, co je nahoře (nic nevyndavej)
 *
 * Paměť na prvky si bereme z HALDY přes malloc a když dojde,
 * zvětšíme ji přes realloc. Zásobník tak nikdy není "plný".
 * V C# tohle dělá List<int> sám – tady to vidíš ručně.
 */
#include <stdio.h>     /* printf */
#include <stdlib.h>    /* malloc, realloc, free */

#include "stack.h"     /* vlastní hlavička – uvozovky místo <> */

/* Funkce dostávají UKAZATEL na zásobník (Stack *s).
 * Kdybychom předali Stack hodnotou, funkce by dostala kopii
 * a změny by se do původního zásobníku v main() nepropsaly. */

bool stack_init(Stack *s)
{
    /* malloc(počet BAJTŮ) – proto krát sizeof(int), což jsou 4 B.
     * Vrací adresu nové paměti, nebo NULL, když paměť není. */
    s->items = malloc(STACK_START_CAPACITY * sizeof(int));
    if (s->items == NULL) {
        return false;
    }
    s->count = 0;   /* s->count je zkratka pro (*s).count */
    s->capacity = STACK_START_CAPACITY;
    return true;
}

void stack_free(Stack *s)
{
    /* Každý malloc musí mít svůj free, jinak paměť "uteče" (memory leak).
     * C nemá garbage collector jako C#. */
    free(s->items);
    s->items = NULL;   /* ať omylem nesáhneme na už vrácenou paměť */
    s->count = 0;
    s->capacity = 0;
}

bool stack_is_empty(const Stack *s)
{
    return s->count == 0;
}

/* Vrací true, když se vložení povedlo, false když došla paměť. */
bool stack_push(Stack *s, int value)
{
    if (s->count == s->capacity) {
        /* Místo došlo -> zdvojnásobíme ho. realloc vezme starou paměť,
         * udělá větší a staré prvky do ní zkopíruje (adresa se může změnit!). */
        int new_capacity = s->capacity * 2;
        int *bigger = realloc(s->items, new_capacity * sizeof(int));
        if (bigger == NULL) {
            return false;   /* stará paměť zůstala platná, nic se neztratilo */
        }
        printf("  [realloc: kapacita %d -> %d, paměť %p -> %p]\n",
               s->capacity, new_capacity, (void *)s->items, (void *)bigger);
        s->items = bigger;
        s->capacity = new_capacity;
    }
    s->items[s->count] = value;   /* zapiš na první volné místo */
    s->count++;
    return true;
}

/* Výsledek vracíme přes ukazatel 'out', protože návratovou hodnotu
 * už používáme na hlášení úspěchu (bool). V C běžný trik. */
bool stack_pop(Stack *s, int *out)
{
    if (stack_is_empty(s)) {
        return false;   /* "stack underflow" – není co vyndat */
    }
    s->count--;
    *out = s->items[s->count];   /* *out = zapiš na adresu, kam out ukazuje */
    return true;
}

bool stack_peek(const Stack *s, int *out)
{
    if (stack_is_empty(s)) {
        return false;
    }
    *out = s->items[s->count - 1];   /* horní prvek, ale count neměníme */
    return true;
}

void stack_print(const Stack *s)
{
    if (stack_is_empty(s)) {
        printf("  (zásobník je prázdný)\n");
    } else {
        /* Vypisujeme odshora dolů, ať to vypadá jako hromádka. */
        for (int i = s->count - 1; i >= 0; i--) {
            printf("  | %5d |%s\n", s->items[i], i == s->count - 1 ? "  <- vrchol" : "");
        }
        printf("  +-------+\n");
    }
    printf("  prvků: %d, kapacita: %d, paměť na haldě: %p\n",
           s->count, s->capacity, (void *)s->items);
}
