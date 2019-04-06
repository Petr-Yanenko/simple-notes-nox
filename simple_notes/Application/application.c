//
//  application.c
//  simple_notes
//
//  Created by Petr Yanenko on 1/7/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include <string.h>
#include "application.h"
#include "commands.h"

struct _SimpleNotesApplication {
    SimpleNotesResponder parent;

    gboolean _proceed;
};

G_DEFINE_TYPE(SimpleNotesApplication, simple_notes_application, SIMPLE_NOTES_TYPE_RESPONDER)

static gboolean simple_notes_application_real_handle_event (SimpleNotesResponder *object, SimpleNotesEvent *event);

void simple_notes_application_class_init (SimpleNotesApplicationClass *klass) {
    SIMPLE_NOTES_RESPONDER_CLASS(klass)->handle_event = simple_notes_application_real_handle_event;
}

void simple_notes_application_init (SimpleNotesApplication *object) {
    object->_proceed = TRUE;
}

SimpleNotesApplication *simple_notes_application_new (SimpleNotesResponder *nextResponder) {
    SimpleNotesResponder *application = SIMPLE_NOTES_RESPONDER(g_object_new(SIMPLE_NOTES_TYPE_APPLICATION, NULL));
    simple_notes_responder_set_ref_next_responder(application, nextResponder);
    return SIMPLE_NOTES_APPLICATION(application);
}

gboolean simple_notes_application_get_proceed (SimpleNotesApplication *object) {
    return object->_proceed;
}

static gboolean simple_notes_application_real_handle_event (SimpleNotesResponder *object, SimpleNotesEvent *event) {
    gchar *command = simple_notes_event_get_command(event)->str;
    gboolean exit = !strcmp(command, kExitCommand);
    if (exit) {
        guint size = g_hash_table_size(simple_notes_event_get_options(event));
        if (size) {
            GError *error = g_error_new(kSimpleNotesDomain, 8, "Command exit have to be without options");
            simple_notes_responder_handle_error(object, error);
            g_clear_error(&error);
        } else {
            SIMPLE_NOTES_APPLICATION(object)->_proceed = !exit;
        }
        return TRUE;
    }
    return SIMPLE_NOTES_RESPONDER_CLASS(simple_notes_application_parent_class)->handle_event(object, event);
}
