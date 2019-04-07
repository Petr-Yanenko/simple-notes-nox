add_library (sqlite_controller sqlite_controller.c)

target_link_libraries (sqlite_controller sqlite_store data_structures)

target_include_directories (sqlite_controller PUBLIC ./)
