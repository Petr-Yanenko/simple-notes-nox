cmake_minimum_required(VERSION 2.6)
project(simple-notes-nox C)

include_directories(
  include
  shared_source
  Libs/sqlite-amalgamation-3320200
  /usr/include
  /usr/include/glib-2.0
  /usr/include/glib-2.0/gio
  /usr/lib/x86_64-linux-gnu/glib-2.0/include
  )

add_subdirectory(shared_source)
add_subdirectory(Libs/sqlite-amalgamation-3320200)
add_subdirectory(simple_notes)
add_subdirectory(simple_notes_tests)
