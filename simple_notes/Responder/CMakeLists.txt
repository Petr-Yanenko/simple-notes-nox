add_library (responder responder.c)

target_include_directories (responder PUBLIC ./)

target_link_libraries (responder event)

add_subdirectory (ResponderStub)
