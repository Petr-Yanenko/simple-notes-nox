//
//  folder.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/15/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_folder____FILEEXTENSION___
#define simple_notes_folder____FILEEXTENSION___

#include "object.h"

#define SIMPLE_NOTES_TYPE_FOLDER simple_notes_folder_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesFolder, simple_notes_folder, SIMPLE_NOTES, FOLDER, SimpleNotesObject);

SimpleNotesFolder *simple_notes_folder_new (void);

GString *simple_notes_folder_create_description (SimpleNotesFolder *object);

guint64 simple_notes_folder_get_id (SimpleNotesFolder *object);
void simple_notes_folder_assign_id (SimpleNotesFolder *object, guint64 id);

gboolean simple_notes_folder_get_selected (SimpleNotesFolder *object);
void simple_notes_folder_assign_selected (SimpleNotesFolder *object, gboolean selected);

GByteArray *simple_notes_folder_get_copy_title (SimpleNotesFolder *object);
void simple_notes_folder_copy_title (SimpleNotesFolder *object, GByteArray *title);

glong simple_notes_folder_get_count (SimpleNotesFolder *object);
void simple_notes_folder_assign_count (SimpleNotesFolder *object, glong count);

#endif
