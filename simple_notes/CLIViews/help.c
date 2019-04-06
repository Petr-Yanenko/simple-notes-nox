//
//  help.c
//  simple_notes
//
//  Created by Petr Yanenko on 1/10/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "help.h"
#include <string.h>
#include "commands.h"

struct _SimpleNotesHelpWindow {
    SimpleNotesWindow parent;
};

G_DEFINE_TYPE(SimpleNotesHelpWindow, simple_notes_help_window, SIMPLE_NOTES_TYPE_WINDOW)

static gboolean simple_notes_help_window_real_handle_event (SimpleNotesResponder *object, SimpleNotesEvent *event);
static void simple_notes_help_window_real_draw_rect (SimpleNotesView *object, SimpleNotesRect rect);

void simple_notes_help_window_class_init (SimpleNotesHelpWindowClass *klass) {
    SIMPLE_NOTES_RESPONDER_CLASS(klass)->handle_event = simple_notes_help_window_real_handle_event;
    SIMPLE_NOTES_VIEW_CLASS(klass)->draw_rect = simple_notes_help_window_real_draw_rect;
}

void simple_notes_help_window_init (SimpleNotesHelpWindow *object) {
    simple_notes_view_assign_bounds(SIMPLE_NOTES_VIEW(object), simple_notes_make_rect(0, 0, 1, 1));
}

SimpleNotesHelpWindow *simple_notes_help_window_new (SimpleNotesResponder *next) {
    SimpleNotesResponder *help = simple_notes_responder_new (SIMPLE_NOTES_TYPE_HELP_WINDOW, next);
    return SIMPLE_NOTES_HELP_WINDOW(help);
}

static gboolean simple_notes_help_window_real_handle_event (SimpleNotesResponder *object, SimpleNotesEvent *event) {
    simple_notes_window_assign_active(SIMPLE_NOTES_WINDOW(object), TRUE);
    
    gchar *command = simple_notes_event_get_command(event)->str;
    gboolean needsHelp = !strcmp(command, kHelpCommand);
    if (needsHelp) {
        guint size = g_hash_table_size(simple_notes_event_get_options(event));
        if (size) {
            GError *error = g_error_new(kSimpleNotesDomain, 9, "Command help have to be without options");
            simple_notes_responder_handle_error(object, error);
            g_clear_error(&error);
        } else {
            simple_notes_view_display(SIMPLE_NOTES_VIEW(object));
        }
        simple_notes_window_assign_active(SIMPLE_NOTES_WINDOW(object), FALSE);
        return TRUE;
    }
    simple_notes_window_assign_active(SIMPLE_NOTES_WINDOW(object), FALSE);
    return SIMPLE_NOTES_RESPONDER_CLASS(simple_notes_help_window_parent_class)->handle_event(object, event);
}



static void simple_notes_help_window_real_draw_rect (SimpleNotesView *object, SimpleNotesRect rect) {
    gchar *help =
            "\nThis tool does not support unicode.\n"
            "\nUsage: command [option]\n\n"
            "commands:\n"
                    "   help - shows this help\n"
                    "   exit - exit from application\n"
                    "   folder - manages folders table\n"
                    "       options:\n"
                    "       --all - shows folders table\n"
                    "       --insert title - inserts new folder\n"
                    "       --delete id - deletes folder with id\n"
                    "       --select id - selects folders with id\n"
                    "   note - manages notes table\n"
                    "       options:\n"
                    "       --all - shows notes table, you must select one of folders\n"
                    "       --insert title - inserts new note\n"
                    "       --delete id - deletes note with id\n"
                    "       --select id - selects note with id\n"
                    "       --edit - starts vim to edit note content, you must select one of notes\n";
    GString *buff = g_string_new("\0");
    g_string_append_printf(buff, help, NULL);
    g_string_prepend(simple_notes_view_get_graphics_buffer_line(object, 0), buff->str);
    g_string_free(buff, TRUE);
}
