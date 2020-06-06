//
//  folder_window.h
//  simple_notes
//
//  Created by Petr Yanenko on 1/21/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_folder_window____FILEEXTENSION___
#define simple_notes_folder_window____FILEEXTENSION___

#include "table_window.h"
#include "folders_model.h"


#define SIMPLE_NOTES_TYPE_FOLDER_WINDOW simple_notes_folder_window_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesFolderWindow, simple_notes_folder_window, SIMPLE_NOTES, FOLDER_WINDOW, SimpleNotesTableWindow)

SimpleNotesFolderWindow *simple_notes_folder_window_new (SimpleNotesResponder *next, SNFoldersModel *model);

#endif
