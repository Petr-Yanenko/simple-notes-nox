add_executable(simple_notes main.c)

#add_subdirectory(SQLiteStore)
#add_subdirectory(DataStructures)
#add_subdirectory(SQLiteController)
#add_subdirectory(Models)
#add_subdirectory(Event)
#add_subdirectory(CLIControllers)
#add_subdirectory(Responder)
#add_subdirectory(CLIViews)
#add_subdirectory(Application)
add_subdirectory(Store)

target_link_libraries(simple_notes
  glib-2.0
  gobject-2.0
  gio-2.0
  sqlite3
 # model
 # application
 # view
 # responder_stub
  )
