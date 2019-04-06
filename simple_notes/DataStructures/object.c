//
//  object.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/15/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "object.h"

typedef struct {
    guint64 _id;
    gboolean _selected;
} SimpleNotesObjectPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesObject, simple_notes_object, G_TYPE_OBJECT);

static GString *simple_notes_object_real_create_description (SimpleNotesObject *object) {
    SimpleNotesObjectPrivate *objectPrivate = simple_notes_object_get_instance_private(object);
    GString *buff = g_string_sized_new(50);
    g_string_printf(buff, "\n\n<%p> ID: %llu\nSelected: %i\n", object, objectPrivate->_id, objectPrivate->_selected);
    return buff;
}

static void simple_notes_object_class_init (SimpleNotesObjectClass *classObject) {
    classObject->create_description = simple_notes_object_real_create_description;
    g_type_add_instance_private(SIMPLE_NOTES_TYPE_OBJECT, sizeof(SimpleNotesObjectPrivate));
}

static void simple_notes_object_init (SimpleNotesObject *object) {
    SimpleNotesObjectPrivate *objectPrivate = simple_notes_object_get_instance_private(object);
    objectPrivate->_id = 0;
    objectPrivate->_selected = FALSE;
}

SimpleNotesObject *simple_notes_object_new (void) {
    return g_object_new(SIMPLE_NOTES_TYPE_OBJECT, NULL);
}

guint64 simple_notes_object_get_id (SimpleNotesObject *object) {
    SimpleNotesObjectPrivate *objectPrivate = simple_notes_object_get_instance_private(object);
    return objectPrivate->_id;
}

void simple_notes_object_assign_id (SimpleNotesObject *object, guint64 id) {
    SimpleNotesObjectPrivate *objectPrivate = simple_notes_object_get_instance_private(object);
    objectPrivate->_id = id;
}

gboolean simple_notes_object_get_selected (SimpleNotesObject *object) {
    SimpleNotesObjectPrivate *objectPrivate = simple_notes_object_get_instance_private(object);
    return objectPrivate->_selected;
}

void simple_notes_object_assign_selected (SimpleNotesObject *object, gboolean selected) {
    SimpleNotesObjectPrivate *objectPrivate = simple_notes_object_get_instance_private(object);
    objectPrivate->_selected = selected;
}

GString *simple_notes_object_create_description (SimpleNotesObject *object) {
    SimpleNotesObjectClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC_WITH_RETURN_VAL(object, &klass, create_description, SimpleNotesObject, SIMPLE_NOTES, OBJECT, NULL);
    return klass->create_description(object);
}
