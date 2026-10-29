/*
 * tasks.c – spojový seznam úkolů a jeho uložení do binárního souboru.
 */
#include "tasks.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TASKS_FILE      "tasks.dat"
#define FILE_MAGIC      "TASK"   /* "magické" bajty na začátku souboru */
#define FILE_VERSION    1u
#define MAX_TITLE_LEN   200

void tasks_init(TaskList *list)
{
    list->head = NULL;
    list->count = 0;
    list->next_id = 1;
}

/* Vytvoří uzel a připojí ho na konec seznamu. */
static Task *add_with_id(TaskList *list, int id, const char *title, uint8_t flags)
{
    Task *task = malloc(sizeof *task);   /* sizeof *task = velikost struktury */
    if (task == NULL) {
        return NULL;
    }

    task->title = str_duplicate(title);
    if (task->title == NULL) {
        free(task);   /* nepovedlo se napůl -> uklidit, co už máme */
        return NULL;
    }
    task->id = id;
    task->flags = flags;
    task->next = NULL;

    /* Trik s ukazatelem na ukazatel: 'link' ukazuje na místo, kam se má
     * zapsat adresa nového uzlu – buď list->head, nebo next posledního uzlu.
     * Díky tomu nepotřebujeme speciální případ pro prázdný seznam. */
    Task **link = &list->head;
    while (*link != NULL) {
        link = &(*link)->next;
    }
    *link = task;

    list->count++;
    if (id >= list->next_id) {
        list->next_id = id + 1;
    }
    return task;
}

Task *tasks_add(TaskList *list, const char *title, uint8_t flags)
{
    return add_with_id(list, list->next_id, title, flags);
}

Task *tasks_find(TaskList *list, int id)
{
    for (Task *t = list->head; t != NULL; t = t->next) {
        if (t->id == id) {
            return t;
        }
    }
    return NULL;
}

bool tasks_remove(TaskList *list, int id)
{
    /* Stejný trik: najdeme ukazatel, který na mazaný uzel ukazuje,
     * a přesměrujeme ho na uzel za ním. */
    Task **link = &list->head;
    while (*link != NULL && (*link)->id != id) {
        link = &(*link)->next;
    }
    if (*link == NULL) {
        return false;
    }

    Task *victim = *link;
    *link = victim->next;

    /* Uvolňujeme v opačném pořadí, než jsme alokovali: nejdřív titulek,
     * pak uzel. Kdybychom free(victim) udělali první, victim->title by
     * už bylo čtení z uvolněné paměti (use-after-free). */
    free(victim->title);
    free(victim);
    list->count--;
    return true;
}

void tasks_print(const TaskList *list)
{
    if (list->head == NULL) {
        printf("  (žádné úkoly)\n");
        return;
    }

    printf("  ID  hotovo  urgent  název                      adresa uzlu\n");
    printf("  --  ------  ------  -------------------------  ------------------\n");
    for (const Task *t = list->head; t != NULL; t = t->next) {
        printf("  %2d   [%c]      %c    %-25s  %p\n",
               t->id,
               (t->flags & TASK_DONE) ? 'x' : ' ',
               (t->flags & TASK_URGENT) ? '!' : ' ',
               t->title,
               (const void *)t);
    }
    printf("\n  Uzly leží na haldě různě rozházené – proto je seznam pomalejší\n"
           "  než pole (procesorová cache nemá rád skákání po paměti).\n");
}

size_t tasks_clear(TaskList *list)
{
    size_t freed = 0;
    Task *t = list->head;
    while (t != NULL) {
        Task *next = t->next;   /* uložit PŘED free – pak už t->next nesmíme číst */
        free(t->title);
        free(t);
        t = next;
        freed++;
    }
    tasks_init(list);
    return freed;
}

/* ---------------------------------------------------------------------------
 * Binární soubor
 *
 * Formát (vše little endian):
 *   "TASK"        4 B   magic
 *   version       4 B
 *   count         4 B
 *   pro každý úkol:
 *     id          4 B
 *     flags       1 B
 *     title_len   2 B
 *     title       title_len B (bez '\0')
 *
 * Proč nezapíšeme rovnou fwrite(task, sizeof(Task), 1, f)?
 *   - struktura obsahuje padding (nedefinované bajty),
 *   - obsahuje ukazatele (title, next) – adresa nemá po restartu smysl,
 *   - pořadí bajtů závisí na procesoru.
 * Proto zapisujeme pole po bajtech sami, přesně podle formátu.
 * ------------------------------------------------------------------------- */

static bool write_u16(FILE *f, uint16_t v)
{
    unsigned char b[2] = { (unsigned char)(v & 0xFF), (unsigned char)(v >> 8) };
    return fwrite(b, 1, sizeof b, f) == sizeof b;
}

static bool write_u32(FILE *f, uint32_t v)
{
    unsigned char b[4] = {
        (unsigned char)(v & 0xFF),
        (unsigned char)((v >> 8) & 0xFF),
        (unsigned char)((v >> 16) & 0xFF),
        (unsigned char)((v >> 24) & 0xFF)
    };
    return fwrite(b, 1, sizeof b, f) == sizeof b;
}

static bool read_u16(FILE *f, uint16_t *out)
{
    unsigned char b[2];
    if (fread(b, 1, sizeof b, f) != sizeof b) {
        return false;
    }
    *out = (uint16_t)(b[0] | (b[1] << 8));
    return true;
}

static bool read_u32(FILE *f, uint32_t *out)
{
    unsigned char b[4];
    if (fread(b, 1, sizeof b, f) != sizeof b) {
        return false;
    }
    *out = (uint32_t)b[0] | ((uint32_t)b[1] << 8) |
           ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
    return true;
}

bool tasks_save(const TaskList *list, const char *path)
{
    FILE *f = fopen(path, "wb");   /* "b" = binárně, Windows jinak mění \n na \r\n */
    if (f == NULL) {
        return false;
    }

    bool ok = fwrite(FILE_MAGIC, 1, 4, f) == 4 &&
              write_u32(f, FILE_VERSION) &&
              write_u32(f, (uint32_t)list->count);

    for (const Task *t = list->head; ok && t != NULL; t = t->next) {
        size_t len = strlen(t->title);
        ok = write_u32(f, (uint32_t)t->id) &&
             fputc(t->flags, f) != EOF &&
             write_u16(f, (uint16_t)len) &&
             fwrite(t->title, 1, len, f) == len;
    }

    /* fclose může selhat (např. plný disk při zápisu bufferu) – kontrolujeme. */
    if (fclose(f) != 0) {
        ok = false;
    }
    return ok;
}

bool tasks_load(TaskList *list, const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return false;
    }

    tasks_clear(list);

    char magic[4];
    uint32_t version = 0, count = 0;
    bool ok = fread(magic, 1, 4, f) == 4 &&
              memcmp(magic, FILE_MAGIC, 4) == 0 &&
              read_u32(f, &version) && version == FILE_VERSION &&
              read_u32(f, &count);

    for (uint32_t i = 0; ok && i < count; i++) {
        uint32_t id = 0;
        uint16_t len = 0;
        int flags = 0;
        char title[MAX_TITLE_LEN + 1];

        ok = read_u32(f, &id) &&
             (flags = fgetc(f)) != EOF &&
             read_u16(f, &len) && len <= MAX_TITLE_LEN &&   /* nevěř datům ze souboru! */
             fread(title, 1, len, f) == len;

        if (ok) {
            title[len] = '\0';
            ok = add_with_id(list, (int)id, title, (uint8_t)flags) != NULL;
        }
    }

    fclose(f);
    if (!ok) {
        tasks_clear(list);   /* poškozený soubor -> nenechávat půlku dat */
    }
    return ok;
}

/* --------------------------------------------------------------------------- */

static Task *ask_for_task(TaskList *list)
{
    int id;
    if (!read_int("  ID úkolu: ", &id)) {
        printf("  Neplatné číslo.\n");
        return NULL;
    }
    Task *t = tasks_find(list, id);
    if (t == NULL) {
        printf("  Úkol %d neexistuje.\n", id);
    }
    return t;
}

void demo_tasks(void)
{
    print_header("6) Úkolníček – spojový seznam + binární soubor");

    TaskList list;
    tasks_init(&list);

    if (tasks_load(&list, TASKS_FILE)) {
        printf("Načteno %zu úkolů ze souboru %s.\n", list.count, TASKS_FILE);
    } else {
        printf("Soubor %s nenalezen, začínáme s ukázkovými úkoly.\n", TASKS_FILE);
        tasks_add(&list, "Naučit se ukazatele", TASK_DONE);
        tasks_add(&list, "Napsat vlastní malloc", TASK_URGENT);
        tasks_add(&list, "Hodit projekt na GitHub", 0);
    }
    printf("Každý uzel: sizeof(Task) = %zu B + titulek zvlášť na haldě.\n\n", sizeof(Task));
    tasks_print(&list);

    for (;;) {
        printf("\n  1 přidat  2 vypsat  3 hotovo  4 urgentní  5 smazat  6 uložit  7 načíst  0 zpět\n");
        int choice;
        if (!read_int("  > ", &choice)) {
            if (feof(stdin)) {
                break;
            }
            continue;
        }

        if (choice == 0) {
            break;
        }

        switch (choice) {
        case 1: {
            char title[MAX_TITLE_LEN + 1];
            if (read_line("  Název: ", title, sizeof title) && title[0] != '\0') {
                Task *t = tasks_add(&list, title, 0);
                if (t != NULL) {
                    printf("  Přidán úkol %d na adrese %p.\n", t->id, (void *)t);
                } else {
                    printf("  Došla paměť.\n");
                }
            }
            break;
        }
        case 2:
            tasks_print(&list);
            break;
        case 3: {
            Task *t = ask_for_task(&list);
            if (t != NULL) {
                t->flags = (uint8_t)(t->flags ^ TASK_DONE);   /* XOR přepne bit */
                printf("  Příznaky úkolu %d: 0x%02X\n", t->id, (unsigned)t->flags);
            }
            break;
        }
        case 4: {
            Task *t = ask_for_task(&list);
            if (t != NULL) {
                t->flags = (uint8_t)(t->flags ^ TASK_URGENT);
                printf("  Příznaky úkolu %d: 0x%02X\n", t->id, (unsigned)t->flags);
            }
            break;
        }
        case 5: {
            int id;
            if (read_int("  ID úkolu: ", &id)) {
                printf(tasks_remove(&list, id) ? "  Smazáno (2x free).\n"
                                               : "  Úkol nenalezen.\n");
            }
            break;
        }
        case 6:
            printf(tasks_save(&list, TASKS_FILE) ? "  Uloženo do %s.\n"
                                                 : "  Uložení do %s selhalo.\n", TASKS_FILE);
            break;
        case 7:
            if (tasks_load(&list, TASKS_FILE)) {
                printf("  Načteno %zu úkolů.\n", list.count);
                tasks_print(&list);
            } else {
                printf("  Načtení selhalo (soubor chybí nebo je poškozený).\n");
            }
            break;
        default:
            printf("  Neznámá volba.\n");
            break;
        }
    }

    size_t freed = tasks_clear(&list);
    printf("  Úklid: uvolněno %zu uzlů (každý = 2x free).\n", freed);
}
