add_library(presenters
  imodel.c
  ientity_model.c
  entity_presenter.c
  folder_presenter.c
  note_presenter.c)

target_include_directories(presenters PUBLIC ./)

target_link_libraries(presenters store data_structures)
