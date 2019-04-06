//
//  notes_model.h
//  simple_notes
//
//  Created by Petr Yanenko on 11/30/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_notes_model____FILEEXTENSION___
#define simple_notes_notes_model____FILEEXTENSION___

#include "selected_list_model.h"
#include "light_note.h"

#define SIMPLE_NOTES_TYPE_NOTES_MODEL simple_notes_notes_model_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesNotesModel, simple_notes_notes_model, SIMPLE_NOTES, NOTES_MODEL, SimpleNotesSelectedListModel)

SimpleNotesNotesModel *simple_notes_notes_model_new (guint64 folderID, SimpleNotesMediator *mediator);

void simple_notes_notes_model_insert_note (SimpleNotesNotesModel *object, guint8 *content);
SimpleNotesLightNote **simple_notes_notes_model_copy_notes (SimpleNotesNotesModel *object, guint *out_count);
void simple_notes_notes_model_delete_note (SimpleNotesNotesModel *object, guint64 identifier);
void simple_notes_notes_model_select_note (SimpleNotesNotesModel *object, guint64 identifier);
void simple_notes_notes_model_change_note_content (SimpleNotesNotesModel *object, guint64 identifier, guint8 *newContent, guint len);

guint64 simple_notes_notes_model_get_folder_id (SimpleNotesNotesModel *object);
void simple_notes_notes_model_assign_folder_id (SimpleNotesNotesModel *object, guint64 folderID);

#endif
