//
//  note_controller.h
//  simple_notes
//
//  Created by Petr Yanenko on 3/17/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_note_controller____FILEEXTENSION___
#define simple_notes_note_controller____FILEEXTENSION___

#include "list_controller.h"
#include "notes_model.h"


#define SIMPLE_NOTES_TYPE_NOTE_CONTROLLER simple_notes_note_controller_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesNoteController, simple_notes_note_controller, SIMPLE_NOTES, NOTE_CONTROLLER, SimpleNotesListController)

SimpleNotesNoteController *simple_notes_note_controller_new (SNNotesModel *model);

#endif
