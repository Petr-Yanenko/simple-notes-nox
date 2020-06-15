//
//  note_window.h
//  simple_notes
//
//  Created by Petr Yanenko on 3/10/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_note_window____FILEEXTENSION___
#define simple_notes_note_window____FILEEXTENSION___

#include "table_window.h"
#include "notes_model.h"


#define SIMPLE_NOTES_TYPE_NOTE_WINDOW simple_notes_note_window_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesNoteWindow, simple_notes_note_window, SIMPLE_NOTES, NOTE_WINDOW, SimpleNotesTableWindow)

SimpleNotesNoteWindow *simple_notes_note_window_new (SimpleNotesResponder *next, SNNotesModel *model);

#endif
