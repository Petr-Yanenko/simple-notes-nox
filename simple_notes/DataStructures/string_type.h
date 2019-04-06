//
//  symbol_type.h
//  simple_notes
//
//  Created by Petr Yanenko on 12/4/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_string_type____FILEEXTENSION___
#define simple_notes_string_type____FILEEXTENSION___

#include <glib-object.h>

typedef enum _SimpleNotesStringTypeIdentifier {
    SimpleNotesStringTypeIdentifierNotDefined,
    SimpleNotesStringTypeIdentifierText,
    SimpleNotesStringTypeIdentifierImage,
    SimpleNotesStringTypeIdentifierVideo
} SimpleNotesStringTypeIdentifier;

#define SIMPLE_NOTES_TYPE_STRING_TYPE simple_notes_string_type_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesStringType, simple_notes_string_type, SIMPLE_NOTES, STRING_TYPE, GObject)

struct _SimpleNotesStringTypeClass {
    GObjectClass parent;
};

SimpleNotesStringType *simple_notes_string_type_new (SimpleNotesStringTypeIdentifier identifier);

SimpleNotesStringTypeIdentifier simple_notes_string_type_get_identifier (SimpleNotesStringType *object);
gchar const *simple_notes_string_type_get_name (SimpleNotesStringType *object);
gchar const *simple_notes_string_type_get_description (SimpleNotesStringType *object);

#endif
