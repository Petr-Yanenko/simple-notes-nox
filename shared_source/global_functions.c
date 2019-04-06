//
//  global_functions.c
//  simple_notes
//
//  Created by Petr Yanenko on 12/16/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "global_functions.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifdef TESTING
gchar *const kFile = "test_simple_notes.data";
gchar *const kNoteFolder = "test_notes";
#else
gchar *const kFile = "simple_notes.data";
gchar *const kNoteFolder = "notes";
#endif

GQuark const kSimpleNotesDomain = 11111;
long const kLongLongSymbols = 21;
long const kSelectedSymbols = 2;

gchar *const kFolderPathFormat ="%s/folder_%llu";
gchar *const kNotePathFormat = "%s/folder_%llu/note_%llu.txt";
glong const kFolderPathSymbols = 39;
glong const kNotePathSymbols = 70;

void simple_notes_delete_test_db (void) {
    simple_notes_sqlite_controller_testing_unref_instance();
    if (simple_notes_trash_file(kFile)) {
        g_message("\n\nTest db deleted");
    }
}

gboolean simple_notes_trash_file(gchar *fileName) {
    GFile *file = g_file_new_for_path(fileName);
    gboolean success = g_file_trash(file, NULL, NULL);
    g_object_unref(file);
    return success;
}

void simple_notes_print_byte_array (GByteArray *array, GString *string, gchar *title) {
    if (array) {
        g_string_append_printf(string, "%s: 0x", title);
        for (glong i = 0; i < array->len; i++) {
            g_string_append_printf(string, "%x", array->data[i]);
        }
    } else {
        g_string_append_printf(string, "%s: 0x0", title);
    }
}

/*void simple_notes_copy (GType type, void (^setter)(GValue *value), void (^getter)(GValue *value)) {
    GValue value = G_VALUE_INIT;
    g_value_init(&value, type);
    setter(&value);
    getter(&value);
    g_value_unset(&value);
    }*/

void simple_notes_set_copy_byte_array (GByteArray *array, GByteArray **variable) {
    if (*variable) {
        g_byte_array_unref(*variable);
        *variable = NULL;
    }
    if (array) {
        *variable = g_byte_array_new();
        g_byte_array_append(*variable, array->data, array->len);
    }
}

GByteArray *simple_notes_get_copy_byte_array (GByteArray *variable) {
    GByteArray *copy = NULL;
    if (variable) {
        copy = g_byte_array_new();
        g_byte_array_append(copy, variable->data, variable->len);
    }
    return copy;
}

guint simple_notes_gulong_guint_cast (gulong value) {
    if (value <= UINT32_MAX) {
        return (guint)value;
    }
    
    g_warn_if_reached();
    return 0;
}

void simple_notes_create_error (GError **pError, gint code, gchar *const format, gint argsNumber, ...) {
    va_list args;
    va_start(args, argsNumber);
    SIMPLE_NOTES_CREATE_ERROR(pError, code, format, argsNumber, args);
    va_end(args);
}

gchar *simple_notes_create_string (gchar *const string) {
    gchar *mallocString = g_malloc(sizeof(gchar) * (strlen(string) + 1));
    strcpy(mallocString, string);
    return mallocString;
}

void simple_notes_print_guint64_value (gchar * buff, guint64 value) {
    sprintf(buff, "%llu", value);
}

void simple_notes_print_boolean_value (gchar *buff, gboolean value) {
    sprintf(buff, "%i", value);
}

void simple_notes_print_long_value (gchar *buff, glong value) {
    sprintf(buff, "%ld", value);
}

void simple_notes_free_objects_array(gpointer *array, gulong count) {
    for (glong i = 0; i < count; i++) {
        g_object_unref(array[i]);
    }
    if (array) {
        g_free(array);
    }
}
