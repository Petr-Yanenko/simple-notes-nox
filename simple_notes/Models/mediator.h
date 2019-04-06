//
//  mediator.h
//  simple_notes
//
//  Created by Petr Yanenko on 12/6/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_mediator____FILEEXTENSION___
#define simple_notes_mediator____FILEEXTENSION___

#include "PrefixHeader.pch"
#include "folders_model.h"
#include "notes_model.h"
#include "content_model.h"

#define SIMPLE_NOTES_TYPE_MEDIATOR simple_notes_mediator_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesMediator, simple_notes_mediator, SIMPLE_NOTES, MEDIATOR, GObject)

SimpleNotesMediator *simple_notes_mediator_new (void);

void simple_notes_mediator_model_changed (SimpleNotesMediator *object, SimpleNotesBaseModel *model);

SimpleNotesFoldersModel *simple_notes_mediator_get_folders_model (SimpleNotesMediator *object);
SimpleNotesNotesModel *simple_notes_mediator_get_notes_model (SimpleNotesMediator *object);
SimpleNotesContentModel *simple_notes_mediator_get_content_model (SimpleNotesMediator *object);

#endif
