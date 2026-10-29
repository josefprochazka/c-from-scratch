/*
 * main.c – vstupní bod programu: jen menu.
 *
 * Samotný zásobník je v stack.h (co umí) a stack.c (jak to dělá).
 * main.c ho jen používá – neví a nezajímá ho, jak je uvnitř udělaný.
 */
#include <stdio.h>     /* printf, scanf */

#ifdef _WIN32
#include <windows.h>   /* SetConsoleOutputCP – kvůli češtině v konzoli */
#endif

#include "stack.h"     /* typ Stack a funkce stack_push, stack_pop, ... */

/* Po špatném vstupu (např. písmeno místo čísla) zůstane text ve vstupu
 * a scanf by ho četl pořád dokola. Tady ho zahodíme až do konce řádku.
 * 'static' = funkce je vidět jen v tomhle souboru (jako private v C#). */
static void clear_input(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

int main(void)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   /* ať konzole Windows ukáže háčky a čárky */
#endif

    Stack stack;          /* proměnná typu Stack – leží na zásobníku programu :) */
    stack_init(&stack);   /* &stack = adresa proměnné stack */

    int choice;
    do {
        printf("\n=== ZÁSOBNÍK ===\n");
        printf("1) push  – vložit číslo\n");
        printf("2) pop   – vyndat číslo\n");
        printf("3) peek  – co je nahoře?\n");
        printf("4) vypsat zásobník\n");
        printf("0) konec\n");
        printf("Volba: ");

        /* scanf vrací, kolik hodnot se podařilo načíst. Chceme 1. */
        if (scanf("%d", &choice) != 1) {
            if (feof(stdin)) {
                break;   /* konec vstupu (Ctrl+Z) */
            }
            clear_input();
            printf("Zadej číslo.\n");
            choice = -1;   /* ať cyklus pokračuje */
            continue;
        }

        int value;
        switch (choice) {
        case 1:
            printf("Číslo: ");
            if (scanf("%d", &value) != 1) {
                clear_input();
                printf("To není číslo.\n");
                break;
            }
            if (stack_push(&stack, value)) {
                printf("Vloženo %d.\n", value);
            } else {
                printf("Zásobník je plný! (stack overflow)\n");
            }
            break;

        case 2:
            if (stack_pop(&stack, &value)) {
                printf("Vyndáno %d.\n", value);
            } else {
                printf("Zásobník je prázdný! (stack underflow)\n");
            }
            break;

        case 3:
            if (stack_peek(&stack, &value)) {
                printf("Nahoře je %d.\n", value);
            } else {
                printf("Zásobník je prázdný.\n");
            }
            break;

        case 4:
            stack_print(&stack);
            break;

        case 0:
            printf("Ahoj!\n");
            break;

        default:
            printf("Taková volba není.\n");
            break;
        }
    } while (choice != 0);

    return 0;   /* 0 = program skončil v pořádku */
}
