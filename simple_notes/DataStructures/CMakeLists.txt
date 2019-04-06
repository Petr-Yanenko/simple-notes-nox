add_library (data_structures
  object.c
  folder.c
  note.c
  light_folder.c
  light_note.c
  )

target_include_directories (data_structures PUBLIC ./)
