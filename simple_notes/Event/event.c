//
//  event.c
//  simple_notes
//
//  Created by Petr Yanenko on 1/7/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "event.h"
#include <ctype.h>
#include <string.h>
#include "commands.h"

struct _SimpleNotesEvent {
    GObject parent;

    GString *_command;
    GHashTable *_options;
};

G_DEFINE_TYPE(SimpleNotesEvent, simple_notes_event, G_TYPE_OBJECT)

static gboolean simple_notes_event_create_command (GString *command, gchar ** split, GError **error);
static gboolean simple_notes_event_fill_options_table (GHashTable *options, gchar ** split, GError **error);
static gboolean simple_notes_event_check_whitespaces (gchar *const buffer, GError **error);
static gchar **simple_notes_event_create_split (SimpleNotesEvent *object, gchar *strippedBuffer, GError **error);

void simple_notes_event_dispose (GObject *object) {
    SimpleNotesEvent *self = SIMPLE_NOTES_EVENT(object);
    if (self->_command) {
        g_string_free(self->_command, TRUE);
    }
    if (self->_options) {
        g_hash_table_unref(self->_options);
    }

    G_OBJECT_CLASS(simple_notes_event_parent_class)->dispose(object);
}

void simple_notes_event_class_init (SimpleNotesEventClass *klass) {
    GObjectClass *objectClass = G_OBJECT_CLASS(klass);
    objectClass->dispose = simple_notes_event_dispose;
}

void simple_notes_event_init (SimpleNotesEvent *object) {
    object->_command = g_string_sized_new(10);
    object->_options = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
}

SimpleNotesEvent *simple_notes_event_new (gchar *const buffer, gssize count, GError **error) {
    SimpleNotesEvent *event = g_object_new(SIMPLE_NOTES_TYPE_EVENT, NULL);

    gboolean success = TRUE;
    if (success) {
        gchar *newLine = g_strrstr(buffer, "\n");
        if (newLine) {
            *newLine = ' ';
        } else {
            success = FALSE;
            if (error) {
                *error = g_error_new(kSimpleNotesDomain, 23, "Invalid string format");
            }
        }
    }
    
    if (success) {
        success = simple_notes_event_check_whitespaces(buffer, error);
    }
    gchar *stripped = NULL;
    if (success) {
        stripped = g_strstrip(buffer);
    }
    success = stripped != NULL;
    
    gchar **split = NULL;
    if (success) {
        split = simple_notes_event_create_split(event, stripped, error);
    }
    success = split != NULL;
    if (success) {
        success = simple_notes_event_create_command(event->_command, split, error);
    }
    if (success) {
        success = simple_notes_event_fill_options_table(event->_options, split, error);
    }

    if (!success) {
        g_clear_object(&event);
    }

    if (split) {
        g_strfreev(split);
    }

    return event;
}

GString *simple_notes_event_get_command (SimpleNotesEvent *event) {
    return event->_command;
}

GHashTable *simple_notes_event_get_options (SimpleNotesEvent *event) {
    return event->_options;
}

gchar *simple_notes_event_get_value_for_option (SimpleNotesEvent *event, gchar *option) {
    return g_hash_table_lookup(event->_options, option);
}

static gboolean simple_notes_event_check_whitespaces (gchar *const buffer, GError **error) {
    for (glong i = 0; buffer[i] != '\0'; i++) {
        gboolean space = isspace(buffer[i]);
        if (space && buffer[i] != ' ') {
            if (error) {
                *error = g_error_new(kSimpleNotesDomain, 4, "All whitespaces except space aren't allowed");
            }
            return FALSE;
        }
    }
    return TRUE;
}

static gboolean simple_notes_event_create_command (GString *command, gchar **split, GError **error) {
    if (!split || !split[0]) {
        if (error) {
            *error = g_error_new(kSimpleNotesDomain, 5, "Empty command");
        }
        return FALSE;
    }
    g_string_append(command, split[0]);

    return TRUE;
}

static gboolean simple_notes_event_fill_options_table (GHashTable *options, gchar **split, GError **error) {
    gboolean option = TRUE;
    gchar *lastOption = NULL;
    for (glong i = 1; split[i] != NULL; i++) {
        gsize len = strlen(split[i]) + 1;
        gchar *buffer = g_malloc(len);
        strcpy(buffer, split[i]);
        if (option) {
            if (strlen(split[i]) <= strlen(kOptionKey) || !g_str_has_prefix(split[i], kOptionKey)) {
                if (error) {
                    *error = g_error_new(kSimpleNotesDomain, 7, "Invalid syntax");
                }
                return FALSE;
            }
            lastOption = buffer;
            option = FALSE;
        } else if (g_str_has_prefix(split[i], kOptionKey)) {
            g_hash_table_insert(options, lastOption, NULL);
            lastOption = buffer;
        } else {
            g_hash_table_insert(options, lastOption, buffer);
            lastOption = NULL;
            option = TRUE;
        }
    }
    if (lastOption) {
        g_hash_table_insert(options, lastOption, NULL);
    }
    return TRUE;
}

static gchar *simple_notes_event_find_quote (SimpleNotesEvent *object, gchar *search, size_t len) {
    gchar *quote = g_strstr_len(search, len, "\"");
    return quote;
}

static gboolean simple_notes_event_find_quotes (SimpleNotesEvent *object, gchar *search, size_t len, gchar **openQuote, gchar **closeQuote, GError **error) {
    gboolean result = TRUE;
    gchar *openQuoteSearch = simple_notes_event_find_quote(object, search, len);
    gchar *closeQuoteSearch = NULL;
    if (openQuoteSearch) {
        closeQuoteSearch = openQuoteSearch + 1;
        closeQuoteSearch = simple_notes_event_find_quote(object, closeQuoteSearch, len - (closeQuoteSearch - search));
        if (!closeQuoteSearch) {
            if (error) {
                *error = g_error_new(kSimpleNotesDomain, 20, "Invalid syntax");
            }
            result = FALSE;
        }
        if (closeQuoteSearch) {
            size_t remainder = len - (closeQuoteSearch - search);
            if (remainder > 1) {
                if (error) {
                    *error = g_error_new(kSimpleNotesDomain, 21, "Invalid syntax");
                }
                result = FALSE;
            }
        }
    }
    if (openQuote) {
        *openQuote = openQuoteSearch;
    }
    if (closeQuote) {
        *closeQuote = closeQuoteSearch;
    }
    return result;
}

static void simple_notes_event_copy_split_part (SimpleNotesEvent *object, gchar *part, GPtrArray *buff) {
    part = g_strdelimit(part, "\"", ' ');
    part = g_strstrip(part);
    g_ptr_array_add(buff, part);
}

static gchar **simple_notes_event_create_split (SimpleNotesEvent *object, gchar *strippedBuffer, GError **error) {
    gchar *search = NULL;
    gchar *previousSearch = strippedBuffer;
    size_t len = strlen(previousSearch);
    if (!len) {
        if (error) {
            *error = g_error_new(kSimpleNotesDomain, 22, "Empty command");
        }
        return NULL;
    }
    gchar *spaceBeforeQuote = previousSearch;
    gchar *openQuoteSearch = NULL, *closeQuoteSearch = NULL;
    gboolean quotes = simple_notes_event_find_quotes(object, previousSearch, len, &openQuoteSearch, &closeQuoteSearch, error);
    SN_RETURN_VAL_IF_FAIL(quotes, NULL, NULL);
    GPtrArray *splitBuff = g_ptr_array_new_with_free_func(g_free);
    while ((search = g_strstr_len(previousSearch, len, " "))) {
        if (!(openQuoteSearch != NULL && search > openQuoteSearch && search < closeQuoteSearch)) {
            gchar *previous = spaceBeforeQuote;
            if (*previous == ' ') {
                previous++;
            }
            size_t copyLen = search - previous;
            if (copyLen) {
                gchar *copy = g_malloc(sizeof(gchar *) * (copyLen + 1));
                strncpy(copy, previous, copyLen);
                copy[copyLen] = '\0';
                simple_notes_event_copy_split_part(object, copy, splitBuff);
            }
            spaceBeforeQuote = search;
        }
        search += 1;
        len -= search - previousSearch;
        previousSearch = search;
    }
    if (*spaceBeforeQuote == ' ') {
        spaceBeforeQuote++;
    }
    gchar *copy = g_malloc(sizeof(gchar *) * (strlen(spaceBeforeQuote) + 1));
    strcpy(copy, spaceBeforeQuote);
    simple_notes_event_copy_split_part(object, copy, splitBuff);
    g_ptr_array_add(splitBuff, NULL);
    gchar **split = (gchar **)g_ptr_array_free(splitBuff, FALSE);
    return split;
}
