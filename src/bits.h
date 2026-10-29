/*
 * bits.h – bitové operace: jak se pracuje s registry hardwaru, příznaky, barvami.
 */
#ifndef BITS_H
#define BITS_H

#include <stdint.h>

/* Makra pro práci s jednotlivými bity. Závorky kolem parametrů jsou nutné –
 * makro je jen textová náhrada, bez nich by SET_BIT(x, a + b) dopadlo špatně. */
#define BIT(n)            (1u << (n))
#define SET_BIT(x, n)     ((x) |=  BIT(n))
#define CLEAR_BIT(x, n)   ((x) &= ~BIT(n))
#define TOGGLE_BIT(x, n)  ((x) ^=  BIT(n))
#define TEST_BIT(x, n)    (((x) >> (n)) & 1u)

/* Vypíše nejnižších 'bits' bitů čísla, po čtveřicích: 0000_0101 */
void print_binary(uint32_t value, int bits);

/* Demo 5 */
void demo_bits(void);

#endif /* BITS_H */
