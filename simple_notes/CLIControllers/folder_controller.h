#ifndef simple_notes_folder_controller____FILEEXTENSION___
#define simple_notes_folder_controller____FILEEXTENSION___

#include "list_controller.h"
#include "folders_model.h"

#define SIMPLE_NOTES_TYPE_FOLDER_CONTROLLER simple_notes_folder_controller_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesFolderController, simple_notes_folder_controller, SIMPLE_NOTES, FOLDER_CONTROLLER, SimpleNotesListController)

SimpleNotesFolderController *simple_notes_folder_controller_new (SNFoldersModel *model);

#endif
