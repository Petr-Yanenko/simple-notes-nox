add_library (application application.c)

target_include_directories (application PUBLIC ./)

target_link_libraries (application responder)
