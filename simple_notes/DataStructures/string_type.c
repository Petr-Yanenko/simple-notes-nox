//
//  symbol_type.c
//  simple_notes
//
//  Created by Petr Yanenko on 12/4/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "string_type.h"

typedef struct {
    SimpleNotesStringTypeIdentifier _identifier;
    gchar const *_name;
    gchar const *_description;
} SimpleNotesStringTypePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesStringType, simple_notes_string_type, G_TYPE_OBJECT)

static void simple_notes_string_type_setup_data (SimpleNotesStringType *object);

static void simple_notes_string_type_class_init (SimpleNotesStringTypeClass *klass) {
    g_type_add_instance_private(SIMPLE_NOTES_TYPE_STRING_TYPE, sizeof(SimpleNotesStringTypePrivate));
}

static void simple_notes_string_type_init (SimpleNotesStringType *object) {
    SimpleNotesStringTypePrivate *private = simple_notes_string_type_get_instance_private(object);
    private->_identifier = SimpleNotesStringTypeIdentifierNotDefined;
    private->_name = NULL;
    private ->_description = NULL;
}

SimpleNotesStringType *simple_notes_string_type_new (SimpleNotesStringTypeIdentifier identifier) {
    SimpleNotesStringType *instance = g_object_new(SIMPLE_NOTES_TYPE_STRING_TYPE, NULL);
    SimpleNotesStringTypePrivate *private = simple_notes_string_type_get_instance_private(instance);
    private->_identifier = identifier;
    simple_notes_string_type_setup_data(instance);
    return instance;
}

SimpleNotesStringTypeIdentifier simple_notes_string_type_get_identifier (SimpleNotesStringType *object) {
    SimpleNotesStringTypePrivate *private = simple_notes_string_type_get_instance_private(object);
    return private->_identifier;
}

gchar const *simple_notes_string_type_get_name (SimpleNotesStringType *object) {
    SimpleNotesStringTypePrivate *private = simple_notes_string_type_get_instance_private(object);
    return private->_name;
}

gchar const *simple_notes_string_type_get_description (SimpleNotesStringType *object) {
    SimpleNotesStringTypePrivate *private = simple_notes_string_type_get_instance_private(object);
    return private->_description;
}

static void simple_notes_string_type_setup_data (SimpleNotesStringType *object) {
    SimpleNotesStringTypePrivate *private = simple_notes_string_type_get_instance_private(object);
    switch (private->_identifier) {
        case SimpleNotesStringTypeIdentifierText:
            private->_name = "text\0";
            private->_description = "Text\0";
            break;

        default: break;
    }
}


