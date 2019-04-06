//
//  symbol.c
//  simple_notes
//
//  Created by Petr Yanenko on 12/4/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "notes_string.h"
#include "light_notes_string.h"

struct _SimpleNotesString {
    GObject parent;

    GByteArray *_string;

    SimpleNotesStringType *_type;
    SimpleNotesStringStyle *_style;
};

static void simple_notes_light_string_interface_init (SimpleNotesLightStringInterface *iface);
static GByteArray *simple_notes_light_string_real_get_text (SimpleNotesLightString *object);
static SimpleNotesStringType *simple_notes_light_string_real_get_string_type (SimpleNotesLightString *object);
static SimpleNotesStringStyle *simple_notes_light_string_real_get_string_style (SimpleNotesLightString *object);

G_DEFINE_TYPE_WITH_CODE(SimpleNotesString, simple_notes_string, G_TYPE_OBJECT,
        G_IMPLEMENT_INTERFACE(SIMPLE_NOTES_TYPE_LIGHT_STRING, simple_notes_light_string_interface_init)
)

static void simple_notes_string_dispose (GObject *object) {
    SimpleNotesString *self = SIMPLE_NOTES_STRING(object);
    if (self->_string)
        g_byte_array_unref(self->_string);
    if (self->_type)
        g_object_unref(self->_type);
    if (self->_style)
        g_object_unref(self->_style);

    G_OBJECT_CLASS(simple_notes_string_parent_class)->dispose(object);
}

static void simple_notes_light_string_interface_init (SimpleNotesLightStringInterface *iface) {
    iface->get_text = simple_notes_light_string_real_get_text;
    iface->get_string_type = simple_notes_light_string_real_get_string_type;
    iface->get_string_style = simple_notes_light_string_real_get_string_style;
}

static void simple_notes_string_class_init (SimpleNotesStringClass *klass) {
    GObjectClass *objectClass = G_OBJECT_CLASS(klass);
    objectClass->dispose = simple_notes_string_dispose;
}

static void simple_notes_string_init (SimpleNotesString *object) {
    object->_string = NULL;
    object->_type = NULL;
    object->_style = NULL;
}

SimpleNotesString *simple_notes_string_new (SimpleNotesStringTypeIdentifier type) {
    SimpleNotesString *instance = g_object_new(SIMPLE_NOTES_TYPE_STRING, NULL);
    instance->_string = g_byte_array_new();
    instance->_type = simple_notes_string_type_new(type);
    return instance;
}

GByteArray *simple_notes_string_get_text (SimpleNotesString *object) {
    return object->_string;
}

void simple_notes_string_ref_text (SimpleNotesString *object, GByteArray *text) {
    if (object->_string != text) {
        if (object->_string) {
            g_byte_array_unref(object->_string);
            object->_string = NULL;
        }
        if (text) {
            object->_string = g_byte_array_ref(text);
        }
    }
}

SimpleNotesStringType *simple_notes_string_get_string_type (SimpleNotesString *object) {
    return object->_type;
}

SimpleNotesStringStyle *simple_notes_string_get_string_style (SimpleNotesString *object) {
    return object->_style;
}

gchar *simple_notes_string_create_xml (SimpleNotesString *object) {
    if (!object->_string->len) {
        return g_markup_printf_escaped(
                                      "<simple_notes_string type=\"%d\"/>",
                                      simple_notes_string_type_get_identifier(object->_type)
                                      );
    }
    gchar *data = g_base64_encode(object->_string->data, object->_string->len);
    gchar *xml = g_markup_printf_escaped(
            "<simple_notes_string type=\"%d\">"
                    "<text>%s</text>"
                    "<style/>"
                    "</simple_notes_string>",
            simple_notes_string_type_get_identifier(object->_type),
            data
    );
    g_free(data);
    return xml;
}

static GByteArray *simple_notes_light_string_real_get_text (SimpleNotesLightString *object) {
    return simple_notes_string_get_text(SIMPLE_NOTES_STRING(object));
}

static SimpleNotesStringType *simple_notes_light_string_real_get_string_type (SimpleNotesLightString *object) {
    return simple_notes_string_get_string_type(SIMPLE_NOTES_STRING(object));
}

static SimpleNotesStringStyle *simple_notes_light_string_real_get_string_style (SimpleNotesLightString *object) {
    return simple_notes_string_get_string_style(SIMPLE_NOTES_STRING(object));
}
