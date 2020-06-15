add_library(sqlite3 sqlite3.c)

target_include_directories(sqlite3 PUBLIC ./)
target_link_libraries(sqlite3 c pthread dl)
