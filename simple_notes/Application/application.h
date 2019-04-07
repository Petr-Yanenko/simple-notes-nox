//
//  application.h
//  simple_notes
//
//  Created by Petr Yanenko on 1/7/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_application____FILEEXTENSION___
#define simple_notes_application____FILEEXTENSION___

#include "../Responder/responder.h"

#define SIMPLE_NOTES_TYPE_APPLICATION simple_notes_application_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesApplication, simple_notes_application, SIMPLE_NOTES, APPLICATION, SimpleNotesResponder)

SimpleNotesApplication *simple_notes_application_new (SimpleNotesResponder *nextResponder);

gboolean simple_notes_application_get_proceed (SimpleNotesApplication *object);

#endif
