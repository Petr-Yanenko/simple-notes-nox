//
//  responder_stub.c
//  simple_notes
//
//  Created by Petr Yanenko on 1/9/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "responder_stub.h"

struct _SimpleNotesResponderStub {
    SimpleNotesResponder parent;
};

G_DEFINE_TYPE(SimpleNotesResponderStub, simple_notes_responder_stub, SIMPLE_NOTES_TYPE_RESPONDER)

static gboolean simple_notes_responder_stub_real_handle_event (SimpleNotesResponder *object, SimpleNotesEvent *event);

void simple_notes_responder_stub_class_init (SimpleNotesResponderStubClass *klass) {
    SIMPLE_NOTES_RESPONDER_CLASS(klass)->handle_event = simple_notes_responder_stub_real_handle_event;
}

void simple_notes_responder_stub_init (SimpleNotesResponderStub *object) {
}

SimpleNotesResponderStub *simple_notes_responder_stub_new (void) {
    return g_object_new(SIMPLE_NOTES_TYPE_RESPONDER_STUB, NULL);
}

static gboolean simple_notes_responder_stub_real_handle_event (SimpleNotesResponder *object, SimpleNotesEvent *event) {
    GError *error = g_error_new(kSimpleNotesDomain, 3, "Command %s not recongnized, try command help", simple_notes_event_get_command(event)->str);
    simple_notes_responder_handle_error(object, error);
    g_clear_error(&error);

    return FALSE;
}
