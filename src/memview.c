/*
 * memview.c – pohled pod kapotu: adresy, bajty, endianita, zarovnání.
 *
 * Tohle je přesně to, co vyšší jazyky (C#, Java, Python) schovávají.
 * V C vidíš, že proměnná je jen pojmenované místo v paměti s nějakou adresou.
 */
#include "memview.h"
#include "util.h"

#include <stddef.h>   /* offsetof */
#include <stdint.h>   /* uint32_t, uintptr_t ... typy s přesnou velikostí */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Globální proměnné žijí po celou dobu běhu programu.
 * Inicializované jdou do sekce .data, nulové do .bss.
 * 'static' na globální úrovni = viditelné jen v tomto souboru. */
static int g_initialized_global = 42;
static int g_zero_global;

/* Konstantní data překladač dá do sekce jen pro čtení (.rdata / .rodata). */
static const char g_message[] = "Ahoj z read-only dat";

void hexdump(const void *data, size_t size)
{
    /* Na paměť se díváme jako na pole bajtů – unsigned char je v C "bajt". */
    const unsigned char *bytes = (const unsigned char *)data;

    for (size_t offset = 0; offset < size; offset += 16) {
        printf("  %p  ", (const void *)(bytes + offset));

        for (size_t i = 0; i < 16; i++) {
            if (offset + i < size) {
                printf("%02X ", bytes[offset + i]);
            } else {
                printf("   ");
            }
        }

        printf(" |");
        for (size_t i = 0; i < 16 && offset + i < size; i++) {
            unsigned char c = bytes[offset + i];
            putchar((c >= 32 && c < 127) ? c : '.');   /* netisknutelné jako '.' */
        }
        printf("|\n");
    }
}

/* Makro s "stringifikací": #type udělá z názvu typu řetězec.
 * PRINT_SIZE(int) se rozvine na printf("...", "int", sizeof(int)) */
#define PRINT_SIZE(type) printf("  %-12s %2zu B\n", #type, sizeof(type))

void demo_memory_layout(void)
{
    print_header("1) Paměť pod lupou");

    int local = 7;                 /* zásobník (stack) – zanikne s koncem funkce */
    static int static_local = 13;  /* jako globální, jen viditelná uvnitř funkce */

    /* malloc = požádej o blok na haldě (heap). Žije, dokud nezavoláš free(). */
    int *heap_value = malloc(sizeof *heap_value);
    if (heap_value == NULL) {
        fprintf(stderr, "malloc selhal\n");
        return;
    }
    *heap_value = 99;

    printf("Kde co bydlí v paměti (adresy se mezi spuštěními mění kvůli ASLR):\n\n");
    printf("  kód     funkce demo_memory_layout  %p\n", (void *)(uintptr_t)demo_memory_layout);
    printf("  rodata  g_message                  %p\n", (const void *)g_message);
    printf("  data    g_initialized_global       %p\n", (void *)&g_initialized_global);
    printf("  bss     g_zero_global              %p\n", (void *)&g_zero_global);
    printf("  static  static_local               %p\n", (void *)&static_local);
    printf("  heap    *heap_value                %p\n", (void *)heap_value);
    printf("  stack   local                      %p\n", (void *)&local);
    printf("\n  Všimni si: globální/statická data leží u sebe, halda a zásobník jinde.\n");

    printf("\nVelikosti typů (sizeof) na této platformě:\n");
    PRINT_SIZE(char);
    PRINT_SIZE(short);
    PRINT_SIZE(int);
    PRINT_SIZE(long);        /* Windows: 4 B, Linux 64bit: 8 B – nepřenositelné! */
    PRINT_SIZE(long long);
    PRINT_SIZE(float);
    PRINT_SIZE(double);
    PRINT_SIZE(void *);      /* 8 B na 64bit systému */
    PRINT_SIZE(size_t);

    /* --- Endianita: v jakém pořadí jsou bajty čísla uložené v paměti --- */
    uint32_t number = 0x11223344;
    printf("\nČíslo 0x11223344 v paměti:\n");
    hexdump(&number, sizeof number);

    const unsigned char *first_byte = (const unsigned char *)&number;
    if (*first_byte == 0x44) {
        printf("  -> LITTLE endian (x86/x64/ARM): nejnižší bajt je první.\n");
    } else {
        printf("  -> BIG endian: nejvyšší bajt je první.\n");
    }

    /* --- double podle IEEE 754 --- */
    double pi = 3.14159265358979;
    printf("\ndouble %.5f v paměti (formát IEEE 754, 8 bajtů):\n", pi);
    hexdump(&pi, sizeof pi);

    /* --- řetězec je jen pole charů zakončené nulou --- */
    char text[] = "Ahoj, C!";
    printf("\nŘetězec \"%s\" – sizeof = %zu (včetně ukončovací nuly '\\0'):\n",
           text, sizeof text);
    hexdump(text, sizeof text);

    /* --- malloc paměť NEnuluje --- */
    unsigned char *raw = malloc(16);
    if (raw != NULL) {
        printf("\nČerstvě alokovaných 16 B z malloc (neinicializované!):\n");
        hexdump(raw, 16);
        printf("  V Debug buildu MSVC vyplní novou haldu hodnotou 0xCD,\n"
               "  aby šlo poznat čtení neinicializované paměti. V Release je tam smetí.\n");
        free(raw);
    }

    /* Každý malloc musí mít svůj free. Pak ukazatel "vynulujeme", aby nevznikl
     * dangling pointer (ukazatel na už uvolněnou paměť). */
    free(heap_value);
    heap_value = NULL;
}

/* Prohození dvou čísel – funkce dostane ADRESY, takže může měnit originály.
 * (C předává vše hodnotou; ukazatel je způsob, jak předat "odkazem".) */
static void swap_ints(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Dvě struktury se stejnými členy, jen v jiném pořadí. */
typedef struct {
    char a;   /* 1 B */
    int  b;   /* 4 B – musí začínat na adrese dělitelné 4 -> 3 B výplně před ním */
    char c;   /* 1 B + 3 B výplně na konci, aby pole struktur bylo zarovnané */
} Padded;

typedef struct {
    int  b;
    char a;
    char c;   /* + jen 2 B výplně */
} Reordered;

void demo_pointers(void)
{
    print_header("2) Ukazatele, aritmetika a zarovnání struktur");

    /* --- ukazatel na ukazatel --- */
    int value = 5;
    int *ptr = &value;          /* ptr obsahuje adresu proměnné value */
    int **ptr_to_ptr = &ptr;    /* a tohle adresu ukazatele ptr */

    printf("value          = %d\n", value);
    printf("&value         = %p   (adresa value)\n", (void *)&value);
    printf("ptr            = %p   (ptr ukazuje na value)\n", (void *)ptr);
    printf("*ptr           = %d   (dereference = hodnota na adrese)\n", *ptr);
    printf("&ptr           = %p   (i ukazatel má svou adresu)\n", (void *)&ptr);
    printf("**ptr_to_ptr   = %d\n", **ptr_to_ptr);

    /* --- pointerová aritmetika --- */
    int numbers[5] = { 10, 20, 30, 40, 50 };
    int *p = numbers;   /* pole se "rozpadne" na ukazatel na první prvek */

    printf("\nPole int[5] a pointerová aritmetika:\n");
    for (int i = 0; i < 5; i++) {
        /* numbers[i] je v C doslova totéž co *(numbers + i) */
        printf("  numbers[%d] = %d   adresa %p   *(p + %d) = %d\n",
               i, numbers[i], (void *)&numbers[i], i, *(p + i));
    }
    printf("  p + 1 neposune o 1 bajt, ale o sizeof(int) = %td B\n",
           (char *)(p + 1) - (char *)p);

    /* --- předávání ukazatelem --- */
    int x = 1, y = 2;
    printf("\nPřed swap_ints(&x, &y): x = %d, y = %d\n", x, y);
    swap_ints(&x, &y);
    printf("Po  swap_ints(&x, &y): x = %d, y = %d\n", x, y);

    /* --- padding ve strukturách --- */
    printf("\nZarovnání (padding) struktur:\n");
    printf("  Padded    { char a; int b; char c; }  sizeof = %zu B\n", sizeof(Padded));
    printf("     offsetof a = %zu, b = %zu, c = %zu\n",
           offsetof(Padded, a), offsetof(Padded, b), offsetof(Padded, c));
    printf("  Reordered { int b; char a; char c; }  sizeof = %zu B\n", sizeof(Reordered));
    printf("     offsetof b = %zu, a = %zu, c = %zu\n",
           offsetof(Reordered, b), offsetof(Reordered, a), offsetof(Reordered, c));

    /* Strukturu nejdřív vyplníme 0xCC, ať jsou vidět bajty výplně.
     * (Obsah výplně není standardem zaručený, v praxi tam ale 0xCC zůstane.) */
    Padded s;
    memset(&s, 0xCC, sizeof s);
    s.a = 'A';
    s.b = 0x11223344;
    s.c = 'C';
    printf("\n  Padded v paměti (CC = výplň, kterou tam vložil překladač):\n");
    hexdump(&s, sizeof s);
    printf("  Ponaučení: řaď členy od největších po nejmenší a ušetříš paměť.\n");
}
