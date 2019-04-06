//
//  responder.h
//  simple_notes
//
//  Created by Petr Yanenko on 1/7/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_responder____FILEEXTENSION___
#define simple_notes_responder____FILEEXTENSION___

#include "event.h"

#define SIMPLE_NOTES_TYPE_RESPONDER simple_notes_responder_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesResponder, simple_notes_responder, SIMPLE_NOTES, RESPONDER, GObject)

struct _SimpleNotesResponderClass {
    GObjectClass parent;

    gboolean (*handle_event) (SimpleNotesResponder *object, SimpleNotesEvent *event);
    void (*handle_error) (SimpleNotesResponder *object, GError *error);
};

SimpleNotesResponder *simple_notes_responder_new (GType type, SimpleNotesResponder *next);

void simple_notes_responder_set_ref_next_responder (SimpleNotesResponder *object, SimpleNotesResponder *next);

gboolean simple_notes_responder_handle_event (SimpleNotesResponder *object, SimpleNotesEvent *event);
void simple_notes_responder_handle_error (SimpleNotesResponder *object, GError *error);

#endif
