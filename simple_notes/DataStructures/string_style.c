//
//  symbol_style.c
//  simple_notes
//
//  Created by Petr Yanenko on 12/4/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "string_style.h"

typedef struct {
    glong _identifier;
} SimpleNotesStringStylePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesStringStyle, simple_notes_string_style, G_TYPE_OBJECT)

static void simple_notes_string_style_class_init (SimpleNotesStringStyleClass *klass) {
    g_type_add_instance_private(SIMPLE_NOTES_TYPE_STRING_STYLE, sizeof(SimpleNotesStringStylePrivate));
}

static void simple_notes_string_style_init (SimpleNotesStringStyle *object) {

}
