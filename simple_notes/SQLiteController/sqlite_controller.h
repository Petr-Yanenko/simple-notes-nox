//
//  sqlite_controller.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/10/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_sqlite_controller____FILEEXTENSION___
#define simple_notes_sqlite_controller____FILEEXTENSION___

#include "../DataStructures/folder.h"
#include "../DataStructures/note.h"

#define SIMPLE_NOTES_TYPE_SQLITE_CONTROLLER  simple_notes_sqlite_controller_get_type ()
G_DECLARE_FINAL_TYPE (SimpleNotesSQLiteController, simple_notes_sqlite_controller, SIMPLE_NOTES, SQLITE_CONTROLLER, GObject)

SimpleNotesSQLiteController *simple_notes_sqlite_controller_get_instance (void);

GList *simple_notes_sqlite_controller_copy_folders (void);
void simple_notes_sqlite_controller_insert_folder (guint8 *const title);
void simple_notes_sqlite_controller_save_folder (SimpleNotesFolder *folder);
void simple_notes_sqlite_controller_delete_folder (SimpleNotesFolder *folder);

GList *simple_notes_sqlite_controller_copy_notes (guint64 folderID);
void simple_notes_sqlite_controller_insert_note (guint8 *const content, guint64 folderID, time_t lastEdited);
void simple_notes_sqlite_controller_save_note (SimpleNotesNote *note);
void simple_notes_sqlite_controller_delete_note (SimpleNotesNote *note);

void simple_notes_sqlite_controller_begin_transaction (void);
void simple_notes_sqlite_controller_commit_transaction (void);

//For testing only
void simple_notes_sqlite_controller_testing_unref_instance (void);

#endif
