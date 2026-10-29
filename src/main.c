/*
 * main.c – vstupní bod programu a hlavní menu.
 *
 * C Memory Lab: malá konzolová laboratoř, která ukazuje, co C umí
 * "blízko u hardwaru" – adresy, bajty, malloc/free, vlastní alokátor, bity.
 */
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN   /* z windows.h jen to nejnutnější */
#include <windows.h>
#endif

/* Debug CRT od Microsoftu umí na konci programu vypsat neuvolněnou paměť. */
#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#include "arena.h"
#include "bits.h"
#include "memview.h"
#include "tasks.h"
#include "util.h"
#include "vector.h"

/* Menu je pole struktur, kde každá obsahuje UKAZATEL NA FUNKCI.
 * Místo dlouhého switch prostě zavoláme MENU[i].run(). */
typedef void (*DemoFn)(void);

typedef struct {
    const char *title;
    DemoFn      run;
} MenuItem;

static const MenuItem MENU[] = {
    { "Paměť pod lupou (adresy, sizeof, endianita, hexdump)", demo_memory_layout },
    { "Ukazatele, pointerová aritmetika, padding struktur",   demo_pointers },
    { "Dynamické pole (malloc / realloc / free, qsort)",      demo_vector },
    { "Vlastní alokátor – arena",                             demo_arena },
    { "Bitové operace (registry, masky, barvy)",              demo_bits },
    { "Úkolníček (spojový seznam + binární soubor)",          demo_tasks },
};

/* Počet prvků pole – klasický C idiom. Funguje jen na skutečné pole,
 * ne na ukazatel (tam by sizeof vrátil velikost ukazatele). */
#define MENU_COUNT (sizeof MENU / sizeof MENU[0])

static void print_menu(void)
{
    printf("\n");
    printf("   ____   __  __                                 _          _     \n");
    printf("  / ___| |  \\/  | ___ _ __ ___   ___  _ __ _   _| |    __ _| |__  \n");
    printf(" | |     | |\\/| |/ _ \\ '_ ` _ \\ / _ \\| '__| | | | |   / _` | '_ \\ \n");
    printf(" | |___  | |  | |  __/ | | | | | (_) | |  | |_| | |__| (_| | |_) |\n");
    printf("  \\____| |_|  |_|\\___|_| |_| |_|\\___/|_|   \\__, |_____\\__,_|_.__/ \n");
    printf("                                           |___/                  \n\n");

    for (size_t i = 0; i < MENU_COUNT; i++) {
        printf("  %zu) %s\n", i + 1, MENU[i].title);
    }
    printf("  0) Konec\n\n");
}

int main(void)
{
#ifdef _WIN32
    /* Ať se v konzoli Windows správně zobrazí čeština (UTF-8). */
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

#if defined(_MSC_VER) && defined(_DEBUG)
    /* Při ukončení vypíše do okna Output ve Visual Studiu všechny leaky.
     * Zkus někde smazat free() a uvidíš "Detected memory leaks!". */
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    for (;;) {
        print_menu();

        int choice;
        if (!read_int("Volba: ", &choice)) {
            if (feof(stdin)) {
                break;   /* Ctrl+Z / konec vstupu */
            }
            printf("Zadej číslo z menu.\n");
            continue;
        }

        if (choice == 0) {
            break;
        }
        if (choice < 1 || choice > (int)MENU_COUNT) {
            printf("Taková volba není.\n");
            continue;
        }

        MENU[choice - 1].run();   /* volání přes ukazatel na funkci */
        wait_for_enter();
    }

    printf("Ahoj! Veškerá paměť je uvolněná. :)\n");
    return EXIT_SUCCESS;
}
