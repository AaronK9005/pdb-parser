cmake_minimum_required(VERSION 3.10.0)
project(pdb_parser VERSION 0.1.0 LANGUAGES C)

add_executable(pdb_parser
    src/main.c
    src/pdb_entry.c
    src/pdb_v2.c
    src/presentation.c
)

target_include_directories(pdb_parser PRIVATE include)