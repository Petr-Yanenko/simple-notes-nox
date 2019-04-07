add_library (responder responder.c)

target_link_libraries (responder event)

target_include_directories (responder PUBLIC ./)

add_subdirectory (ResponderStub)
