# c-from-scratch

Jednoduchý **zásobník (stack)** v čistém C. Je to konzolový program s menu. Paměť na prvky
si bere z haldy přes `malloc` a podle potřeby ji zvětšuje přes `realloc`. Kód je okomentovaný
česky. Projekt slouží k připomenutí základů C.

## Co je zásobník

Funguje jako hromádka talířů. Nový talíř se dává **nahoru** a bere se taky ten **horní**.
Tomu se říká **LIFO** (*Last In, First Out*).

| Operace | Co dělá |
|---------|---------|
| `push`  | vloží číslo nahoru |
| `pop`   | vyndá horní číslo |
| `peek`  | ukáže horní číslo, ale nevyndá ho |

Když chceš vyndat číslo z prázdného zásobníku, program ohlásí *stack underflow*.

## Jak zásobník pracuje s pamětí

- `stack_init` si přes **`malloc`** vezme z haldy místo na 2 čísla.
- Když se místo zaplní, `stack_push` ho přes **`realloc`** zdvojnásobí (2 → 4 → 8 …).
  Program přitom vypíše starou a novou adresu, takže vidíš, že se pole v paměti přestěhovalo.
- Na konci `stack_free` vrátí paměť přes **`free`**. C nemá garbage collector jako C#,
  takže každý `malloc` musí mít svůj `free`.

V C# tohle dělá `List<int>` sám. Tady to vidíš ručně.

## Co si v kódu procvičíš

- `struct` a `typedef`
- **ukazatele**: funkce dostávají `Stack *s`, aby mohly měnit původní zásobník
- vracení hodnoty přes ukazatel (`int *out`)
- **dynamickou paměť**: `malloc`, `realloc`, `free`, `sizeof`, kontrolu na `NULL`
- `#define`, `bool`, `switch`, cyklus `do … while`
- čtení vstupu přes `scanf` a ošetření špatného vstupu
- rozdělení kódu na hlavičku (`.h`) a implementaci (`.c`), statickou knihovnu

## Ukázka

```
Volba: 1
Číslo: 30
  [realloc: kapacita 2 -> 4, paměť 00000227004236A0 -> 0000022700423920]
Vloženo 30.

Volba: 4
  |    30 |  <- vrchol
  |    20 |
  |    10 |
  +-------+
  prvků: 3, kapacita: 4, paměť na haldě: 0000022700423920
```

## Jak to spustit ve Visual Studiu

Potřebuješ workload **„Desktop development with C++“**. Doinstaluješ ho přes Visual Studio Installer.

1. `File > Open > Folder...` a vyber složku projektu (tu, kde je `CMakeLists.txt`).
2. Počkej, až VS dokončí konfiguraci CMake. V okně *Output* se objeví „CMake generation finished“.
3. Nahoře v liště vyber jako spouštěcí položku **`zasobnik.exe`**.
4. Zmáčkni **Ctrl+F5** (spustí bez debuggeru) nebo **F5** (spustí s debuggerem).

Projekt nemá `.sln`, protože používá **CMake**. To je běžný způsob, jak se staví C a C++
projekty na Windows i Linuxu. Visual Studio ho otevře přímo jako složku.

## Struktura projektu

```
.
├── CMakeLists.txt   # build: knihovna "stack" + program "zasobnik"
└── src/
    ├── stack.h      # hlavička – CO zásobník umí (typ Stack, seznam funkcí)
    ├── stack.c      # implementace – JAK to dělá (malloc, realloc, free)
    └── main.c       # jen menu, zásobník používá přes #include "stack.h"
```

### Proč `.h` a `.c`

- **`stack.h`** obsahuje **deklarace**: jen oznamuje, že funkce existují a jaké mají parametry
  (`bool stack_push(Stack *s, int value);`).
- **`stack.c`** obsahuje **definice**: těla funkcí, tedy skutečný kód.

Překladač C čte každý `.c` soubor zvlášť a shora dolů. Funkci tedy musí znát dřív, než ji
někdo zavolá. `main.c` si proto přes `#include "stack.h"` vloží deklarace. Těla funkcí najde
až **linker**, když spojuje `main.c` se statickou knihovnou `stack.lib` (vznikne ze `stack.c`)
do výsledného `zasobnik.exe`.

## Úkoly na procvičení

1. Přidej operaci „vyprázdni zásobník“.
2. Když se zásobník vyprázdní pod čtvrtinu kapacity, zmenši paměť přes `realloc` na polovinu.
3. Napiš frontu (*queue*, FIFO) a porovnej ji se zásobníkem.
