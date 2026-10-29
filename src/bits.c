/*
 * bits.c – bitové operace v praxi.
 */
#include "bits.h"
#include "util.h"

#include <stdbool.h>
#include <stdio.h>

void print_binary(uint32_t value, int bits)
{
    for (int i = bits - 1; i >= 0; i--) {
        putchar(TEST_BIT(value, i) ? '1' : '0');
        if (i % 4 == 0 && i != 0) {
            putchar('_');
        }
    }
}

/* Počet jedniček (Kernighanův trik): x & (x - 1) smaže nejnižší jedničku. */
static int count_set_bits(uint32_t x)
{
    int count = 0;
    while (x != 0) {
        x &= x - 1;
        count++;
    }
    return count;
}

static bool is_power_of_two(uint32_t x)
{
    return x != 0 && (x & (x - 1)) == 0;
}

/* Bity řídicího registru – v datasheetu čipu by byly popsané stejně. */
enum {
    REG_ENABLE    = 0,   /* bit 0: zapnout periferii */
    REG_INTERRUPT = 3,   /* bit 3: povolit přerušení */
    REG_LED       = 7    /* bit 7: rozsvítit LED */
};

static void print_register(const char *label, uint32_t reg)
{
    printf("  %-28s ", label);
    print_binary(reg, 8);
    printf("  (0x%02X)\n", (unsigned)reg);
}

void demo_bits(void)
{
    print_header("5) Bitové operace");

    /* Na mikrokontroléru bys psal něco jako:
     *     volatile uint32_t *ctrl = (volatile uint32_t *)0x40021000;
     *     *ctrl |= BIT(0);
     * = zápis přímo na adresu hardwarového registru. 'volatile' říká překladači,
     * ať čtení/zápis neoptimalizuje, protože hodnotu mění hardware.
     * Na PC to udělat nemůžeme (OS by nás zastavil), tak registr simulujeme. */
    uint32_t control = 0;

    printf("Simulovaný 8bitový řídicí registr:\n");
    print_register("start", control);
    SET_BIT(control, REG_ENABLE);
    print_register("SET_BIT(ENABLE)", control);
    SET_BIT(control, REG_INTERRUPT);
    print_register("SET_BIT(INTERRUPT)", control);
    TOGGLE_BIT(control, REG_LED);
    print_register("TOGGLE_BIT(LED)", control);
    TOGGLE_BIT(control, REG_LED);
    print_register("TOGGLE_BIT(LED) znovu", control);
    CLEAR_BIT(control, REG_ENABLE);
    print_register("CLEAR_BIT(ENABLE)", control);
    printf("  TEST_BIT(INTERRUPT) = %u\n", TEST_BIT(control, REG_INTERRUPT));

    /* --- posuny --- */
    printf("\nPosuny = násobení/dělení mocninou dvojky:\n");
    uint32_t n = 5;
    printf("  %u << 1 = %u    %u << 3 = %u    %u >> 1 = %u\n",
           n, n << 1, n, n << 3, n, n >> 1);

    /* --- počítání bitů, mocniny dvojky --- */
    printf("\nPočet jedniček a mocniny dvojky:\n");
    uint32_t samples[] = { 1, 6, 64, 255, 1000, 1024 };
    for (size_t i = 0; i < sizeof samples / sizeof samples[0]; i++) {
        printf("  %5u = ", samples[i]);
        print_binary(samples[i], 12);
        printf("  jedniček: %2d  mocnina 2: %s\n",
               count_set_bits(samples[i]), is_power_of_two(samples[i]) ? "ano" : "ne");
    }

    /* --- zabalení barvy do jednoho čísla (jako v grafických kartách) --- */
    uint8_t r = 0xFF, g = 0x88, b = 0x22;
    uint32_t color = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;

    printf("\nBarva RGB(%u, %u, %u) zabalená do 32 bitů: 0x%06X\n",
           (unsigned)r, (unsigned)g, (unsigned)b, (unsigned)color);
    printf("  rozbalení maskou: R = (c >> 16) & 0xFF = %u\n", (unsigned)((color >> 16) & 0xFF));
    printf("                    G = (c >>  8) & 0xFF = %u\n", (unsigned)((color >> 8) & 0xFF));
    printf("                    B =  c        & 0xFF = %u\n", (unsigned)(color & 0xFF));

    /* --- XOR trik --- */
    unsigned a = 12, c = 34;
    a ^= c;
    c ^= a;
    a ^= c;
    printf("\nXOR swap bez pomocné proměnné: a = %u, c = %u\n"
           "  (hezký trik, ale v reálném kódu radši klasický tmp – je čitelnější)\n", a, c);
}
