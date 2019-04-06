add_executable (simple_notes_tests
  main.c
  folders_model_testing.c
  notes_model_testing.c
  sqlite_controller_testing.c
  )

add_dependencies (simple_notes_tests simple_notes)
