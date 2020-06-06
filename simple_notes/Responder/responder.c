//
//  responder.c
//  simple_notes
//
//  Created by Petr Yanenko on 1/7/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "responder.h"

typedef struct {
    SimpleNotesResponder *_nextResponder;
} SimpleNotesResponderPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesResponder, simple_notes_responder, G_TYPE_OBJECT)

static gboolean simple_notes_responder_real_handle_event (SimpleNotesResponder *object, SimpleNotesEvent *event);
static void simple_notes_responder_real_handle_error (SimpleNotesResponder *object, GError *error);

void simple_notes_responder_dispose (GObject  *object) {
    SimpleNotesResponderPrivate *private = simple_notes_responder_get_instance_private(SIMPLE_NOTES_RESPONDER(object));
    if (private->_nextResponder) {
        g_object_unref(private->_nextResponder);
    }

    G_OBJECT_CLASS(simple_notes_responder_parent_class)->dispose(object);
}

void simple_notes_responder_class_init (SimpleNotesResponderClass *klass) {
    g_type_add_instance_private(SIMPLE_NOTES_TYPE_RESPONDER, sizeof(SimpleNotesResponderPrivate));

    G_OBJECT_CLASS(klass)->dispose = simple_notes_responder_dispose;
    klass->handle_event = simple_notes_responder_real_handle_event;
    klass->handle_error = simple_notes_responder_real_handle_error;
}

void simple_notes_responder_init (SimpleNotesResponder *object) {
}

SimpleNotesResponder *simple_notes_responder_new (GType type, SimpleNotesResponder *next) {
    SimpleNotesResponder *responder = SIMPLE_NOTES_RESPONDER(g_object_new(type, NULL));
    simple_notes_responder_set_ref_next_responder(responder, next);
    return responder;
}

void simple_notes_responder_set_ref_next_responder (SimpleNotesResponder *object, SimpleNotesResponder *next) {
    SimpleNotesResponderPrivate *private = simple_notes_responder_get_instance_private(object);

    private->_nextResponder = g_object_ref(next);
}

gboolean simple_notes_responder_handle_event (SimpleNotesResponder *object, SimpleNotesEvent *event) {
    SimpleNotesResponderClass *klass = NULL;
    SN_GET_CLASS_OR_RETURN_VAL(object, &klass, handle_event, SimpleNotesResponder, SIMPLE_NOTES, RESPONDER, FALSE);
    return klass->handle_event(object, event);
}

void simple_notes_responder_handle_error (SimpleNotesResponder *object, GError *error) {
    SimpleNotesResponderClass *klass = NULL;
    SN_GET_CLASS(object, &klass, handle_error, SimpleNotesResponder, SIMPLE_NOTES, RESPONDER);
    klass->handle_error(object, error);
}

static gboolean simple_notes_responder_real_handle_event (SimpleNotesResponder *object, SimpleNotesEvent *event) {
    SimpleNotesResponderPrivate *private = simple_notes_responder_get_instance_private(object);

    return simple_notes_responder_handle_event(private->_nextResponder, event);
}

static void simple_notes_responder_real_handle_error (SimpleNotesResponder *object, GError *error) {
    g_printerr("%s %d %d\n", error->message, error->code, error->domain);
}
