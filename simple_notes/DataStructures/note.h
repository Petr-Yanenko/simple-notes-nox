//
//  note.h
//  simple_notes
//
//  Created by Petr Yanenko on 11/23/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_note____FILEEXTENSION___
#define simple_notes_note____FILEEXTENSION___

#include "object.h"

#define SIMPLE_NOTES_TYPE_NOTE simple_notes_note_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesNote, simple_notes_note, SIMPLE_NOTES, NOTE, SimpleNotesObject)

SimpleNotesNote *simple_notes_note_new (void);

GString *simple_notes_note_create_description (SimpleNotesNote *object);

guint64 simple_notes_note_get_id (SimpleNotesNote *object);
void simple_notes_note_assign_id (SimpleNotesNote *object, guint64 id);

gboolean simple_notes_note_get_selected (SimpleNotesNote *object);
void simple_notes_note_assign_selected (SimpleNotesNote *object, gboolean selected);

GByteArray *simple_notes_note_get_copy_content (SimpleNotesNote *object);
void simple_notes_note_copy_content (SimpleNotesNote *object, GByteArray *content);

guint64 simple_notes_note_get_folder_id (SimpleNotesNote *object);
void simple_notes_note_assign_folder_id (SimpleNotesNote *object, guint64 folderID);

GDateTime *simple_notes_note_get_copy_last_edited (SimpleNotesNote *object);
void simple_notes_note_copy_last_edited (SimpleNotesNote *object, GDateTime *time);

#endif
