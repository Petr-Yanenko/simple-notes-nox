add_library(store
  data_base.c
  statement.c
  store.c
  data_iterator.c
  folder_iterator.c
  )

target_include_directories(store PUBLIC ./)
