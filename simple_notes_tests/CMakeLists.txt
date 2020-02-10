add_executable(simple_notes_tests
  main.c
#  folders_model_testing.c
#  notes_model_testing.c
#  sqlite_controller_testing.c
  )

add_dependencies(simple_notes_tests simple_notes)

target_link_libraries(simple_notes_tests
  glib-2.0
  gobject-2.0
  gio-2.0
  sqlite3
  shared
#  sqlite_controller
#  model
  )
