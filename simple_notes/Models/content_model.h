//
//  content_model.h
//  simple_notes
//
//  Created by Petr Yanenko on 12/1/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_content_model____FILEEXTENSION___
#define simple_notes_content_model____FILEEXTENSION___

#include "list_model.h"
#include "light_note.h"
#include "light_notes_string.h"

typedef struct _SimpleNotesMediator SimpleNotesMediator;

#define SIMPLE_NOTES_TYPE_CONTENT_MODEL simple_notes_content_model_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesContentModel, simple_notes_content_model, SIMPLE_NOTES, CONTENT_MODEL, SimpleNotesListModel)

SimpleNotesContentModel *simple_notes_content_model_new (
                                                         guint64 noteID,
                                                         guint64 folderID,
                                                         SimpleNotesMediator *mediator
                                                         );

void simple_notes_content_model_insert_text (
        SimpleNotesContentModel *object,
        guint8 *text,
        gulong textLen,
        gulong location,
        gulong len,
        SimpleNotesStringStyle *style
);
SimpleNotesLightString **simple_notes_content_model_copy_items (SimpleNotesContentModel *object, guint *out_count);

guint64 simple_notes_content_model_get_note_id (SimpleNotesContentModel *object);
void simple_notes_content_model_change_note_id (
                                                SimpleNotesContentModel *object,
                                                guint64 noteID,
                                                guint64 folderID
                                                );

#endif
