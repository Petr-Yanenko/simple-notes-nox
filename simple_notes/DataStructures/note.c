//
//  note.c
//  simple_notes
//
//  Created by Petr Yanenko on 11/23/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "note.h"
#include "light_note.h"

struct _SimpleNotesNote {
    SimpleNotesObject parent;

    guint64 _folderID;
    GByteArray *_content;
    GDateTime *_lastEdited;
};

static void simple_notes_light_note_interface_init (SimpleNotesLightNoteInterface *iface);
static GString *simple_notes_light_note_real_create_description (SimpleNotesLightNote *object);
static guint64 simple_notes_light_note_real_get_id (SimpleNotesLightNote *object);
static gboolean simple_notes_light_note_real_get_selected (SimpleNotesLightNote *object);
static GByteArray *simple_notes_light_note_real_get_copy_content (SimpleNotesLightNote *object);
static guint64 simple_notes_light_note_real_get_folder_id (SimpleNotesLightNote *object);
static GDateTime *simple_notes_light_note_real_get_copy_last_edited (SimpleNotesLightNote *object);

G_DEFINE_TYPE_WITH_CODE(SimpleNotesNote, simple_notes_note, SIMPLE_NOTES_TYPE_OBJECT,
G_IMPLEMENT_INTERFACE (
        SIMPLE_NOTES_TYPE_LIGHT_NOTE,
        simple_notes_light_note_interface_init)
);

static GString *simple_notes_note_real_create_description (SimpleNotesObject *object) {
    SimpleNotesNote *note = SIMPLE_NOTES_NOTE(object);
    GString *description = SIMPLE_NOTES_OBJECT_CLASS(simple_notes_note_parent_class)->create_description(object);
    simple_notes_print_byte_array(note->_content, description, "Content\0");
    g_string_append_printf(description, "\nFolder id: %llu\n", note->_folderID);
    if (note->_lastEdited) {
        gchar *lastEdited = g_date_time_format(note->_lastEdited, "%c");
        g_string_append_printf(description, "\nLast edited: %s\n", lastEdited);
        g_free(lastEdited);
    } else {
        g_string_append(description, "\nLast edited: NULL\n");
    }
    return description;
}

static void simple_notes_note_dispose (GObject *object) {
    SimpleNotesNote *note = SIMPLE_NOTES_NOTE(object);
    if (note->_content) {
        g_byte_array_unref(note->_content);
    }
    if (note->_lastEdited) {
        g_date_time_unref(note->_lastEdited);
    }

    G_OBJECT_CLASS(simple_notes_note_parent_class)->dispose(object);
}

static void simple_notes_light_note_interface_init (SimpleNotesLightNoteInterface *iface) {
    iface->create_description = simple_notes_light_note_real_create_description;
    iface->get_id = simple_notes_light_note_real_get_id;
    iface->get_selected = simple_notes_light_note_real_get_selected;
    iface->get_copy_content = simple_notes_light_note_real_get_copy_content;
    iface->get_folder_id = simple_notes_light_note_real_get_folder_id;
    iface->get_copy_last_edited = simple_notes_light_note_real_get_copy_last_edited;
}

static void simple_notes_note_class_init (SimpleNotesNoteClass *klass) {
    GObjectClass *gClassObject = G_OBJECT_CLASS(klass);
    SimpleNotesObjectClass *parent = SIMPLE_NOTES_OBJECT_CLASS(klass);
    gClassObject->dispose = simple_notes_note_dispose;
    parent->create_description = simple_notes_note_real_create_description;
}

static void simple_notes_note_init (SimpleNotesNote *object) {
    object->_folderID = 0;
    object->_content = NULL;
    object->_lastEdited = NULL;
}

SimpleNotesNote *simple_notes_note_new (void) {
    return g_object_new(SIMPLE_NOTES_TYPE_NOTE, NULL);
}

GString *simple_notes_note_create_description (SimpleNotesNote *object) {
    SimpleNotesObjectClass *klass;
    g_return_val_if_fail(SIMPLE_NOTES_IS_NOTE(object), NULL);
    klass = SIMPLE_NOTES_OBJECT_CLASS(G_OBJECT(object)->g_type_instance.g_class);
    g_return_val_if_fail(klass->create_description != NULL, NULL);
    return klass->create_description(SIMPLE_NOTES_OBJECT(object));
}

guint64 simple_notes_note_get_id (SimpleNotesNote *object) {
    return simple_notes_object_get_id(SIMPLE_NOTES_OBJECT(object));
}

void simple_notes_note_assign_id (SimpleNotesNote *object, guint64 id) {
    simple_notes_object_assign_id(SIMPLE_NOTES_OBJECT(object), id);
}

gboolean simple_notes_note_get_selected (SimpleNotesNote *object) {
    return simple_notes_object_get_selected(SIMPLE_NOTES_OBJECT(object));
}

void simple_notes_note_assign_selected (SimpleNotesNote *object, gboolean selected) {
    simple_notes_object_assign_selected(SIMPLE_NOTES_OBJECT(object), selected);
}

GByteArray *simple_notes_note_get_copy_content (SimpleNotesNote *object) {
    return simple_notes_get_copy_byte_array(object->_content);
}

void simple_notes_note_copy_content (SimpleNotesNote *object, GByteArray *content) {
    simple_notes_set_copy_byte_array(content, &object->_content);
}

guint64 simple_notes_note_get_folder_id (SimpleNotesNote *object) {
    return object->_folderID;
}

void simple_notes_note_assign_folder_id (SimpleNotesNote *object, guint64 folderID) {
    object->_folderID = folderID;
}

GDateTime *simple_notes_note_get_copy_last_edited (SimpleNotesNote *object) {
    GDateTime *copy = NULL;

    void setter (GValue *value) {
      g_value_set_boxed(value, object->_lastEdited);
    }

    void getter (GValue *value) {
      if (g_value_get_boxed(value)) {
        copy = g_value_dup_boxed(value);
      }
	}

    simple_notes_copy(G_TYPE_DATE_TIME, setter, getter);

    return copy;
}

void simple_notes_note_copy_last_edited (SimpleNotesNote *object, GDateTime *time) {
    if (object->_lastEdited) {
        g_date_time_unref(object->_lastEdited);
        object->_lastEdited = NULL;
    }
    if (time) {
      void setter (GValue *value) {
        g_value_set_boxed(value, time);
      }

      void getter (GValue *value) {
        object->_lastEdited = g_value_dup_boxed(value);
      }

      simple_notes_copy(G_TYPE_DATE_TIME, setter, getter);
    }
}

static GString *simple_notes_light_note_real_create_description (SimpleNotesLightNote *object) {
    return simple_notes_note_create_description(SIMPLE_NOTES_NOTE(object));
}

static guint64 simple_notes_light_note_real_get_id (SimpleNotesLightNote *object) {
    return simple_notes_note_get_id(SIMPLE_NOTES_NOTE(object));
}

static gboolean simple_notes_light_note_real_get_selected (SimpleNotesLightNote *object) {
    return simple_notes_note_get_selected(SIMPLE_NOTES_NOTE(object));
}

static GByteArray *simple_notes_light_note_real_get_copy_content (SimpleNotesLightNote *object) {
    return simple_notes_note_get_copy_content(SIMPLE_NOTES_NOTE(object));
}

static guint64 simple_notes_light_note_real_get_folder_id (SimpleNotesLightNote *object) {
    return simple_notes_note_get_folder_id(SIMPLE_NOTES_NOTE(object));
}

static GDateTime *simple_notes_light_note_real_get_copy_last_edited (SimpleNotesLightNote *object) {
    return simple_notes_note_get_copy_last_edited(SIMPLE_NOTES_NOTE(object));
}
