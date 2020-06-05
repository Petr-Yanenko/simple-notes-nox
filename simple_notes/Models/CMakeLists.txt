add_library (model
  model_utility.c
  base_model.c
  list_model.c
  selected_list_model.c
#  folders_model.c
#  notes_model.c
)

target_include_directories (model PUBLIC ./)

target_link_libraries (model presenters data_structures)
