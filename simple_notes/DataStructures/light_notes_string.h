//
//  light_notes_string.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/7/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef __simple_notes__light_notes_string__
#define __simple_notes__light_notes_string__

#include <glib-object.h>
#import "string_type.h"
#import "string_style.h"

#define SIMPLE_NOTES_TYPE_LIGHT_STRING simple_notes_light_string_get_type ()
G_DECLARE_INTERFACE(SimpleNotesLightString, simple_notes_light_string, SIMPLE_NOTES, LIGHT_STRING, GObject)

struct _SimpleNotesLightStringInterface {
    GTypeInterface parent;

    GByteArray * (*get_text) (SimpleNotesLightString *object);
    SimpleNotesStringType * (*get_string_type) (SimpleNotesLightString *object);
    SimpleNotesStringStyle * (*get_string_style) (SimpleNotesLightString *object);
};

GByteArray *simple_notes_light_string_get_text (SimpleNotesLightString *object);
SimpleNotesStringType *simple_notes_light_string_get_string_type (SimpleNotesLightString *object);
SimpleNotesStringStyle *simple_notes_light_string_get_string_style (SimpleNotesLightString *object);

#endif /* defined(__simple_notes__light_notes_string__) */
