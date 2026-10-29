/*
 * tasks.h – úkolníček: spojový seznam + ukládání do binárního souboru.
 */
#ifndef TASKS_H
#define TASKS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Příznaky jako bity – do jednoho bajtu se vejde až 8 ano/ne hodnot. */
enum {
    TASK_DONE   = 1u << 0,
    TASK_URGENT = 1u << 1
};

/* Uzel spojového seznamu. Každý úkol je samostatně alokovaný na haldě
 * a ukazuje na další (next). Poslední má next == NULL. */
typedef struct Task {
    int          id;
    uint8_t      flags;
    char        *title;   /* řetězec taky na haldě – samostatný malloc */
    struct Task *next;
} Task;

typedef struct {
    Task   *head;
    size_t  count;
    int     next_id;
} TaskList;

void   tasks_init(TaskList *list);
Task  *tasks_add(TaskList *list, const char *title, uint8_t flags);
Task  *tasks_find(TaskList *list, int id);
bool   tasks_remove(TaskList *list, int id);
void   tasks_print(const TaskList *list);
size_t tasks_clear(TaskList *list);   /* vrací počet uvolněných uzlů */

bool   tasks_save(const TaskList *list, const char *path);
bool   tasks_load(TaskList *list, const char *path);

/* Demo 6 – interaktivní úkolníček */
void demo_tasks(void);

#endif /* TASKS_H */
