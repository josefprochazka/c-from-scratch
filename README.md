# C Memory Lab 🔬

Malá interaktivní konzolová aplikace v čistém **C (C11)**, která ukazuje, proč je C
„blízko u hardwaru“. Uvidíš, kde v paměti leží proměnné, jak vypadají jednotlivé bajty
a jak funguje `malloc` / `realloc` / `free`. Taky si vyzkoušíš vlastní alokátor,
bitové operace jako u registrů mikrokontroléru a spojový seznam ukládaný do binárního souboru.

Kód je okomentovaný česky. Je to projekt na připomenutí si C a hraní si s ním.

---

## Co aplikace dělá

Po spuštění se ukáže menu. Každá položka je samostatné demo:

| # | Demo | Co uvidíš | Soubor |
|---|------|-----------|--------|
| 1 | **Paměť pod lupou** | adresy proměnných na zásobníku, haldě, v `.data`/`.bss`/`.rdata`; `sizeof` typů; endianitu; `double` a řetězec bajt po bajtu (hexdump); neinicializovanou paměť z `malloc` | `src/memview.c` |
| 2 | **Ukazatele** | ukazatel na ukazatel, pointerovou aritmetiku (`p + 1` = +4 B), předávání odkazem (`swap`), **padding** ve strukturách a `offsetof` | `src/memview.c` |
| 3 | **Dynamické pole** | vlastní obdobu `std::vector`: `realloc` se zdvojnásobením kapacity (a jestli se blok přestěhoval), `calloc`, `qsort` s **ukazateli na funkce** | `src/vector.c` |
| 4 | **Arena alokátor** | vlastní alokátor: jeden `malloc`, pak „ukrajování“ se **zarovnáním** adres, reset v O(1) | `src/arena.c` |
| 5 | **Bitové operace** | simulovaný HW registr (`SET_BIT`, `CLEAR_BIT`, `TOGGLE_BIT`), posuny, počítání jedniček, mocniny dvojky, zabalení RGB barvy do `uint32_t` | `src/bits.c` |
| 6 | **Úkolníček** | interaktivní spojový seznam (přidat / přepnout / smazat), bitové příznaky, uložení do **binárního souboru** s vlastním formátem (`tasks.dat`) | `src/tasks.c` |

A ještě pár drobností navíc:
- **Menu je pole ukazatelů na funkce** (`main.c`), takže tam není žádný dlouhý `switch`.
- V Debug buildu ve Visual Studiu se zapne **detekce memory leaků** (`_CrtSetDbgFlag`).
- Bezpečné čtení vstupu: `fgets` + `strtol` místo `scanf`/`gets` (`util.c`).

## Jak to spustit ve Visual Studiu

### 0. Jednorázově: nainstaluj C/C++ kompilátor
Visual Studio ve výchozím stavu kompilátor C/C++ neobsahuje.
1. Spusť **Visual Studio Installer**.
2. U své verze VS klikni na **Modify**.
3. Zaškrtni workload **„Desktop development with C++“** (Vývoj desktopových aplikací pomocí C++).
   Přidá se i MSVC kompilátor a *C++ CMake tools for Windows*.
4. Klikni na **Modify** / **Install**.

### 1. Otevři projekt
Projekt používá **CMake**. Visual Studio ho umí otevřít rovnou, bez `.sln`:

1. `File > Open > Folder...` a vyber složku s tímto projektem (tu, kde je `CMakeLists.txt`).
2. Chvíli počkej, než VS dokončí konfiguraci CMake (dole v okně *Output* se objeví „CMake generation finished“).
3. Nahoře v liště vyber jako spouštěcí položku **`c-memory-lab.exe`**.
4. Zmáčkni **F5** (spuštění s debuggerem) nebo **Ctrl+F5** (bez debuggeru).

### Alternativa: příkazová řádka
V **Developer PowerShell for VS** (najdeš ho ve Start menu):

```powershell
cmake -S . -B build
cmake --build build --config Debug
.\build\Debug\c-memory-lab.exe
```

Příkaz `cmake -S . -B build` ti zároveň vygeneruje klasické `build\CMemoryLab.sln`, takže ho jde otevřít i „postaru“.

## Tipy, co si zkusit

- **Okno Memory v debuggeru:** dej breakpoint (F9) do `demo_memory_layout`,
  spusť F5 a otevři `Debug > Windows > Memory > Memory 1`. Do pole *Address* vlož adresu,
  kterou program vypsal, a uvidíš ty samé bajty jako v hexdumpu.
- **Vyrob memory leak:** zakomentuj v `vector.c` řádek `free(zeros);`, spusť v Debug (F5)
  a po ukončení programu se v okně *Output* objeví `Detected memory leaks!`.
- **Debug vs. Release:** v Debug buildu MSVC vyplní novou paměť z haldy hodnotou `0xCD`
  a neinicializovaný zásobník `0xCC`. V Release tam bude náhodné smetí.
- **Prohlédni si `tasks.dat`:** v úkolníčku dej *uložit* a otevři soubor v hex editoru
  (třeba ve VS: pravým na soubor > *Open With... > Binary Editor*). Na začátku bude `TASK`.

## Úkoly na procvičení

1. Přidej do `IntVector` funkci `vec_pop` a zmenšování kapacity, když je pole poloprázdné.
2. Udělej z `IntVector` generické pole přes `void *` a `element_size` (jako to dělá `qsort`).
3. Přidej do arény „značky“ (`arena_mark` / `arena_rewind_to`), aby šlo vracet jen část alokací.
4. Přidej k úkolům termín (`uint32_t` jako Unix timestamp) a zvyš `FILE_VERSION` na 2.
5. Přepiš spojový seznam na obousměrný (`prev` ukazatel).

## Struktura projektu

```
.
├── CMakeLists.txt     # build konfigurace (VS ji otevře přes Open Folder)
├── README.md
└── src/
    ├── main.c         # menu z pole ukazatelů na funkce
    ├── util.c/.h      # bezpečné čtení vstupu, str_duplicate
    ├── memview.c/.h   # hexdump, adresy, sizeof, endianita, ukazatele, padding
    ├── vector.c/.h    # dynamické pole (realloc), qsort
    ├── arena.c/.h     # arena alokátor se zarovnáním
    ├── bits.c/.h      # bitové operace a makra
    └── tasks.c/.h     # spojový seznam + binární soubor
```

## Poznámka k češtině v konzoli

Program na Windows přepne konzoli do UTF-8 (`SetConsoleOutputCP`), aby byla vidět diakritika.
Ve starší konzoli (conhost) může nefungovat **zadávání** znaků s diakritikou.
V tom případě piš názvy úkolů bez háčků a čárek, nebo použij Windows Terminal.
