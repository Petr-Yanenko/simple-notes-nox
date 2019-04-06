//
//  light_notes_string.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/7/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "light_notes_string.h"

G_DEFINE_INTERFACE(SimpleNotesLightString, simple_notes_light_string, G_TYPE_OBJECT)

static void simple_notes_light_string_default_init (SimpleNotesLightStringInterface *iface) {

}

GByteArray *simple_notes_light_string_get_text (SimpleNotesLightString *object) {
    SimpleNotesLightStringInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object, &iface, get_text, SimpleNotesLightString, SIMPLE_NOTES, LIGHT_STRING, NULL);
    return iface->get_text(object);
}

SimpleNotesStringType *simple_notes_light_string_get_string_type (SimpleNotesLightString *object) {
    SimpleNotesLightStringInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object, &iface, get_string_type, SimpleNotesLightString, SIMPLE_NOTES, LIGHT_STRING, NULL);
    return iface->get_string_type(object);
}

SimpleNotesStringStyle *simple_notes_light_string_get_string_style (SimpleNotesLightString *object) {
    SimpleNotesLightStringInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object, &iface, get_string_style, SimpleNotesLightString, SIMPLE_NOTES, LIGHT_STRING, NULL);
    return iface->get_string_style(object);
}
