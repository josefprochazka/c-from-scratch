/*
 * util.c – implementace pomocných funkcí z util.h
 */
#include "util.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_header(const char *title)
{
    printf("\n==========================================================\n");
    printf("  %s\n", title);
    printf("==========================================================\n\n");
}

bool read_line(const char *prompt, char *buffer, size_t size)
{
    if (prompt != NULL) {
        printf("%s", prompt);
        fflush(stdout);   /* printf bufferuje – vynutíme vypsání před čtením */
    }

    /* fgets je bezpečný: nikdy nezapíše víc než 'size' bajtů (na rozdíl od gets) */
    if (fgets(buffer, (int)size, stdin) == NULL) {
        return false;     /* EOF nebo chyba */
    }

    /* strcspn vrátí index prvního '\n' (nebo délku řetězce, když tam není) */
    size_t len = strcspn(buffer, "\n");
    if (buffer[len] == '\n') {
        buffer[len] = '\0';
    } else {
        /* Řádek byl delší než buffer – zbytek zahodíme, jinak by "přetekl"
         * do dalšího čtení. */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }
    return true;
}

bool read_int(const char *prompt, int *out)
{
    char buffer[64];
    if (!read_line(prompt, buffer, sizeof buffer)) {
        return false;
    }

    /* strtol je robustnější než atoi: pozná chybu i přetečení */
    char *end = NULL;
    errno = 0;
    long value = strtol(buffer, &end, 10);

    if (end == buffer || *end != '\0' || errno == ERANGE ||
        value < INT_MIN || value > INT_MAX) {
        return false;
    }

    *out = (int)value;
    return true;
}

void wait_for_enter(void)
{
    char dummy[8];
    read_line("\n[Enter] pro návrat do menu...", dummy, sizeof dummy);
}

char *str_duplicate(const char *src)
{
    size_t len = strlen(src) + 1;   /* +1 pro ukončovací '\0' – klasická past! */
    char *copy = malloc(len);
    if (copy != NULL) {
        memcpy(copy, src, len);
    }
    return copy;
}
