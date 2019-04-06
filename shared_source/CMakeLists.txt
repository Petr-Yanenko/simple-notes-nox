add_library (shared global_functions.c commands.c)

target_include_directories (shared PUBLIC ./)
