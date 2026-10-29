# Visual Studio 2022 umí CMake projekt otevřít přímo: File > Open > Folder...
cmake_minimum_required(VERSION 3.21)

project(CMemoryLab LANGUAGES C)

# Standard C11 (kvůli _Alignof, stdint.h, stdbool.h ...)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(c-memory-lab
    src/main.c
    src/util.c
    src/memview.c
    src/vector.c
    src/arena.c
    src/bits.c
    src/tasks.c
)

if(MSVC)
    # /W4    = vysoká úroveň varování
    # /utf-8 = zdrojáky i výstup v UTF-8 (kvůli češtině)
    target_compile_options(c-memory-lab PRIVATE /W4 /utf-8)
    # MSVC jinak hlásí fopen/strcpy jako "unsafe" a nutí _s varianty
    target_compile_definitions(c-memory-lab PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(c-memory-lab PRIVATE -Wall -Wextra -pedantic)
endif()
