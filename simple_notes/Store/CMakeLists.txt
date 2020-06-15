add_library(store
  data_base.c
  statement.c
  sql_controller.c
  store.c
  data_iterator.c
  entity_iterator.c
  folder_iterator.c
  note_iterator.c
  )

target_include_directories(store PUBLIC ./)
target_link_libraries(store sqlite3)
