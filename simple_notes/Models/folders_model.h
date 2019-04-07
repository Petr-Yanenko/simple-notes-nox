//
//  folders_model.h
//  simple_notes
//
//  Created by Petr Yanenko on 11/11/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_folders_model____FILEEXTENSION___
#define simple_notes_folders_model____FILEEXTENSION___

#include "selected_list_model.h"
#include "light_folder.h"

#define SIMPLE_NOTES_TYPE_FOLDERS_MODEL simple_notes_folders_model_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesFoldersModel, simple_notes_folders_model, SIMPLE_NOTES, FOLDERS_MODEL, SimpleNotesSelectedListModel)

SimpleNotesFoldersModel *simple_notes_folders_model_new (SimpleNotesMediator *mediator);

void simple_notes_folders_model_insert_folder (SimpleNotesFoldersModel *object, guint8 *name);
SimpleNotesLightFolder **simple_notes_folders_model_copy_folders (SimpleNotesFoldersModel *object, guint *out_count);
void simple_notes_folders_model_delete_folder (SimpleNotesFoldersModel *object, guint64 identifier);
void simple_notes_folders_model_select_folder (SimpleNotesFoldersModel *object, guint64 identifier);
void simple_notes_folders_model_change_folder_title (SimpleNotesFoldersModel *object, guint64 identifier, guint8 *newTitle, guint len);

#endif
