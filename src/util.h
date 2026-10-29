/*
 * util.h – drobné pomocné funkce pro práci s konzolí a řetězci.
 *
 * Hlavičkový soubor (.h) obsahuje jen DEKLARACE – "slib", že funkce existuje.
 * Samotná DEFINICE (tělo funkce) je v util.c. Linker je pak spojí dohromady.
 */
#ifndef UTIL_H   /* "include guard" – zabrání vložení hlavičky dvakrát */
#define UTIL_H

#include <stdbool.h>
#include <stddef.h>

/* Vypíše nadpis sekce. */
void print_header(const char *title);

/* Bezpečně načte řádek ze stdin (bez '\n'). Vrací false při EOF. */
bool read_line(const char *prompt, char *buffer, size_t size);

/* Načte celé číslo. Vrací false, pokud vstup není platné číslo. */
bool read_int(const char *prompt, int *out);

/* Počká, až uživatel zmáčkne Enter. */
void wait_for_enter(void);

/* Vlastní obdoba strdup(): alokuje kopii řetězce na haldě.
 * Volající je zodpovědný za free()! */
char *str_duplicate(const char *src);

#endif /* UTIL_H */
