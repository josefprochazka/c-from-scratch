# Visual Studio umí CMake projekt otevřít přímo: File > Open > Folder...
cmake_minimum_required(VERSION 3.21)

project(CFromScratch LANGUAGES C)

# Standard C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Knihovna "stack" ze stack.c (statická = .lib, vloží se přímo do .exe)
add_library(stack STATIC src/stack.c)

# Program "zasobnik.exe" z main.c, který používá knihovnu stack
add_executable(zasobnik src/main.c)
target_link_libraries(zasobnik PRIVATE stack)

if(MSVC)
    # /W4    = vysoká úroveň varování
    # /utf-8 = zdrojáky v UTF-8 (kvůli češtině)
    target_compile_options(stack PRIVATE /W4 /utf-8)
    target_compile_options(zasobnik PRIVATE /W4 /utf-8)
    # MSVC jinak hlásí scanf jako "unsafe" a nutí scanf_s
    target_compile_definitions(zasobnik PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
