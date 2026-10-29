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
 */
#include <stdio.h>     /* printf */

#include "stack.h"     /* vlastní hlavička – uvozovky místo <> */

/* Funkce dostávají UKAZATEL na zásobník (Stack *s).
 * Kdybychom předali Stack hodnotou, funkce by dostala kopii
 * a změny by se do původního zásobníku v main() nepropsaly. */

void stack_init(Stack *s)
{
    s->count = 0;   /* s->count je zkratka pro (*s).count */
}

bool stack_is_empty(const Stack *s)
{
    return s->count == 0;
}

bool stack_is_full(const Stack *s)
{
    return s->count == STACK_SIZE;
}

/* Vrací true, když se vložení povedlo, false když je zásobník plný. */
bool stack_push(Stack *s, int value)
{
    if (stack_is_full(s)) {
        return false;   /* "stack overflow" – přetečení */
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
        return;
    }
    /* Vypisujeme odshora dolů, ať to vypadá jako hromádka. */
    for (int i = s->count - 1; i >= 0; i--) {
        printf("  | %5d |%s\n", s->items[i], i == s->count - 1 ? "  <- vrchol" : "");
    }
    printf("  +-------+\n");
    printf("  prvků: %d / %d\n", s->count, STACK_SIZE);
}
