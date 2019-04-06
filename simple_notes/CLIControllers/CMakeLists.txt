add_library (controller
  base_controller.c
  list_controller.c
  folder_controller.c
  note_controller.c)

target_include_directories (controller PUBLIC ./)

target_link_libraries (controller model event)
