# c-from-scratch

Jednoduchý **zásobník (stack)** v čistém C. Je to konzolový program s menu, rozdělený do tří
souborů (viz níže). Kód je okomentovaný česky. Projekt slouží k připomenutí základů C.

## Co je zásobník

Funguje jako hromádka talířů. Nový talíř se dává **nahoru** a bere se taky ten **horní**.
Tomu se říká **LIFO** (*Last In, First Out*).

| Operace | Co dělá |
|---------|---------|
| `push`  | vloží číslo nahoru |
| `pop`   | vyndá horní číslo |
| `peek`  | ukáže horní číslo, ale nevyndá ho |

Zásobník je pole o pěti prvcích a k němu počítadlo `count`. Když je plný, program ohlásí
*stack overflow*. Když je prázdný a chceš z něj něco vyndat, ohlásí *stack underflow*.

## Co si v kódu procvičíš

- `struct` a `typedef`
- **ukazatele**: funkce dostávají `Stack *s`, aby mohly měnit původní zásobník
- vracení hodnoty přes ukazatel (`int *out`)
- `#define`, `bool`, `switch`, cyklus `do … while`
- čtení vstupu přes `scanf` a ošetření špatného vstupu
- rozdělení kódu na hlavičku (`.h`) a implementaci (`.c`), statická knihovna

## Ukázka

```
=== ZÁSOBNÍK ===
1) push  – vložit číslo
2) pop   – vyndat číslo
3) peek  – co je nahoře?
4) vypsat zásobník
0) konec
Volba: 4
  |    30 |  <- vrchol
  |    20 |
  |    10 |
  +-------+
  prvků: 3 / 5
```

## Jak to spustit ve Visual Studiu

Potřebuješ workload **„Desktop development with C++“**. Doinstaluješ ho přes Visual Studio Installer.

1. `File > Open > Folder...` a vyber složku projektu (tu, kde je `CMakeLists.txt`).
2. Počkej, až VS dokončí konfiguraci CMake. V okně *Output* se objeví „CMake generation finished“.
3. Nahoře v liště vyber jako spouštěcí položku **`zasobnik.exe`**.
4. Zmáčkni **Ctrl+F5** (spustí bez debuggeru) nebo **F5** (spustí s debuggerem).

Projekt nemá `.sln`, protože používá **CMake**. To je běžný způsob, jak se staví C a C++
projekty na Windows i Linuxu. Visual Studio ho otevře přímo jako složku.

## Úkoly na procvičení

1. Přidej operaci „vyprázdni zásobník“.
2. Zvětši zásobník na 100 prvků a zkontroluj, že se nic dalšího nerozbije.
3. Udělej zásobník, který roste: místo pole použij `malloc` a `realloc`.
4. Napiš frontu (*queue*, FIFO) a porovnej ji se zásobníkem.

## Struktura projektu

```
.
├── CMakeLists.txt   # build: knihovna "stack" + program "zasobnik"
└── src/
    ├── stack.h      # hlavička – CO zásobník umí (typ Stack, seznam funkcí)
    ├── stack.c      # implementace – JAK to dělá
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
