add_library (responder_stub responder_stub.c)

target_include_directories (responder_stub PUBLIC ./)

target_link_libraries (responder_stub responder)
