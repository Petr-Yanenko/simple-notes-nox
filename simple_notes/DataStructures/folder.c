//
//  folder.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/15/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "folder.h"
#include "light_folder.h"

struct _SimpleNotesFolder {
    SimpleNotesObject parent;

    GByteArray *_title;
    glong _count;
};

static void simple_notes_light_folder_interface_init (SimpleNotesLightFolderInterface *iface);
static GString *simple_notes_light_folder_real_create_description (SimpleNotesLightFolder *object);
static guint64 simple_notes_light_folder_real_get_id (SimpleNotesLightFolder *object);
static gboolean simple_notes_light_folder_real_get_selected (SimpleNotesLightFolder *object);
static GByteArray *simple_notes_light_folder_real_get_copy_title (SimpleNotesLightFolder *object);
static glong simple_notes_light_folder_real_get_count (SimpleNotesLightFolder *object);

G_DEFINE_TYPE_WITH_CODE(SimpleNotesFolder, simple_notes_folder, SIMPLE_NOTES_TYPE_OBJECT,
G_IMPLEMENT_INTERFACE (
        SIMPLE_NOTES_TYPE_LIGHT_FOLDER,
        simple_notes_light_folder_interface_init)
);

static GString *simple_notes_folder_real_create_description (SimpleNotesObject *object) {
    SimpleNotesFolder *folder = SIMPLE_NOTES_FOLDER(object);
    GString *description = SIMPLE_NOTES_OBJECT_CLASS(simple_notes_folder_parent_class)->create_description(object);
    simple_notes_print_byte_array(folder->_title, description, "Title\0");
    g_string_append_printf(description, "\nCount: %ld\n", folder->_count);
    return description;
}

static void simple_notes_folder_dispose (GObject *object) {
    SimpleNotesFolder *folder = SIMPLE_NOTES_FOLDER(object);
    if (folder->_title) {
        g_byte_array_unref(folder->_title);
    }

    G_OBJECT_CLASS(simple_notes_folder_parent_class)->dispose(object);
}

static void simple_notes_light_folder_interface_init (SimpleNotesLightFolderInterface *iface) {
    iface->create_description = simple_notes_light_folder_real_create_description;
    iface->get_id = simple_notes_light_folder_real_get_id;
    iface->get_selected = simple_notes_light_folder_real_get_selected;
    iface->get_copy_title = simple_notes_light_folder_real_get_copy_title;
    iface->get_count = simple_notes_light_folder_real_get_count;
}

static void simple_notes_folder_class_init (SimpleNotesFolderClass *classObject) {
    GObjectClass *gClassObject = G_OBJECT_CLASS(classObject);
    SimpleNotesObjectClass *parent = SIMPLE_NOTES_OBJECT_CLASS(classObject);
    gClassObject->dispose = simple_notes_folder_dispose;
    parent->create_description = simple_notes_folder_real_create_description;
}

static void simple_notes_folder_init (SimpleNotesFolder *object) {
    object->_title = NULL;
    object->_count = 0;
}

SimpleNotesFolder *simple_notes_folder_new (void) {
    return g_object_new(SIMPLE_NOTES_TYPE_FOLDER, NULL);
}

guint64 simple_notes_folder_get_id (SimpleNotesFolder *object) {
    return simple_notes_object_get_id(SIMPLE_NOTES_OBJECT(object));
}

void simple_notes_folder_assign_id (SimpleNotesFolder *object, guint64 id) {
    simple_notes_object_assign_id(SIMPLE_NOTES_OBJECT(object), id);
}

gboolean simple_notes_folder_get_selected (SimpleNotesFolder *object) {
    return simple_notes_object_get_selected(SIMPLE_NOTES_OBJECT(object));
}

void simple_notes_folder_assign_selected (SimpleNotesFolder *object, gboolean selected) {
    simple_notes_object_assign_selected(SIMPLE_NOTES_OBJECT(object), selected);
}

GByteArray *simple_notes_folder_get_copy_title (SimpleNotesFolder *object) {
    return simple_notes_get_copy_byte_array(object->_title);
}

void simple_notes_folder_copy_title (SimpleNotesFolder *object, GByteArray *title) {
    simple_notes_set_copy_byte_array(title, &object->_title);
}

glong simple_notes_folder_get_count (SimpleNotesFolder *object) {
    return object->_count;
}

void simple_notes_folder_assign_count (SimpleNotesFolder *object, glong count) {
    object->_count = count;
}

GString *simple_notes_folder_create_description (SimpleNotesFolder *object) {
    SimpleNotesObjectClass *klass;
    g_return_val_if_fail(SIMPLE_NOTES_IS_FOLDER(object), NULL);
    klass = SIMPLE_NOTES_OBJECT_CLASS(G_OBJECT(object)->g_type_instance.g_class);
    g_return_val_if_fail(klass->create_description != NULL, NULL);
    return klass->create_description(SIMPLE_NOTES_OBJECT(object));
}

static GString *simple_notes_light_folder_real_create_description (SimpleNotesLightFolder *object) {
    return simple_notes_folder_create_description(SIMPLE_NOTES_FOLDER(object));
}

static guint64 simple_notes_light_folder_real_get_id (SimpleNotesLightFolder *object) {
    return simple_notes_folder_get_id(SIMPLE_NOTES_FOLDER(object));
}

static gboolean simple_notes_light_folder_real_get_selected (SimpleNotesLightFolder *object) {
    return simple_notes_folder_get_selected(SIMPLE_NOTES_FOLDER(object));
}

static GByteArray *simple_notes_light_folder_real_get_copy_title (SimpleNotesLightFolder *object) {
    return simple_notes_folder_get_copy_title(SIMPLE_NOTES_FOLDER(object));
}

static glong simple_notes_light_folder_real_get_count (SimpleNotesLightFolder *object) {
    return simple_notes_folder_get_count(SIMPLE_NOTES_FOLDER(object));
}
