add_library (view
  help.c
  view_utility.c
  view.c
  window.c
  table_window.c
  folder_window.c
  note_window.c
  table_view.c
  table_view_cell.c
  label.c
  label_cell.c)

target_include_directories (view PUBLIC ./)

target_link_libraries (view controller responder)
