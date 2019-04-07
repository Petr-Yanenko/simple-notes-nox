//
//  notes_model.c
//  simple_notes
//
//  Created by Petr Yanenko on 11/30/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "notes_model.h"
#include "model_utility.h"
#include "note.h"
#include "sqlite_controller.h"
#include <stdio.h>

struct _SimpleNotesNotesModel {
    SimpleNotesSelectedListModel parent;

    guint64 _folderID;
};

G_DEFINE_TYPE(SimpleNotesNotesModel, simple_notes_notes_model, SIMPLE_NOTES_TYPE_SELECTED_LIST_MODEL)

static void simple_notes_notes_model_real_save_objects (
        SimpleNotesSelectedListModel *object,
        SimpleNotesObject *items[],
        glong count
);
static GList *simple_notes_notes_model_real_create_items (SimpleNotesListModel *object);
static void simple_notes_notes_model_changed (SimpleNotesNotesModel *object);

static void simple_notes_notes_model_class_init (SimpleNotesNotesModelClass *klass) {
    SimpleNotesSelectedListModelClass *selectedModelClass = SIMPLE_NOTES_SELECTED_LIST_MODEL_CLASS(klass);
    selectedModelClass->save_objects = simple_notes_notes_model_real_save_objects;

    SimpleNotesListModelClass *listModelClass = SIMPLE_NOTES_LIST_MODEL_CLASS(klass);
    listModelClass->create_items = simple_notes_notes_model_real_create_items;
}

static void simple_notes_notes_model_init (SimpleNotesNotesModel *object) {
    object->_folderID = 0;
}

SimpleNotesNotesModel *simple_notes_notes_model_new (guint64 folderID, SimpleNotesMediator *mediator) {
    SimpleNotesNotesModel *instance = g_object_new(SIMPLE_NOTES_TYPE_NOTES_MODEL, NULL);
    simple_notes_notes_model_assign_folder_id(instance, folderID);
    simple_notes_selected_list_model_set_weak_ref_mediator(SIMPLE_NOTES_SELECTED_LIST_MODEL(instance), mediator);
    return instance;
}

void simple_notes_notes_model_insert_note (SimpleNotesNotesModel *object, guint8 *content) {
    if (object->_folderID) {
        time_t lastEdited = time(NULL);
        simple_notes_sqlite_controller_insert_note(content, object->_folderID, lastEdited);
        simple_notes_notes_model_changed(object);
    }
}

SimpleNotesLightNote **simple_notes_notes_model_copy_notes (SimpleNotesNotesModel *object, guint *out_count) {
    return (SimpleNotesLightNote **) SIMPLE_NOTES_SELECTED_LIST_MODEL_CLASS(simple_notes_notes_model_parent_class)->copy_items(
            SIMPLE_NOTES_SELECTED_LIST_MODEL(object), out_count
    );
}

void simple_notes_notes_model_delete_note (SimpleNotesNotesModel *object, guint64 identifier) {
  /*simple_notes_delete_object(SIMPLE_NOTES_SELECTED_LIST_MODEL(object), identifier, ^(SimpleNotesObject *deletedObject) {
        simple_notes_sqlite_controller_delete_note(SIMPLE_NOTES_NOTE(deletedObject));
        gchar path[kNotePathSymbols + 1];
        sprintf(path, kNotePathFormat, kNoteFolder, object->_folderID, identifier);
        simple_notes_trash_file(path);
	});*/
    simple_notes_notes_model_changed(object);
}

void simple_notes_notes_model_select_note (SimpleNotesNotesModel *object, guint64 identifier) {
    simple_notes_selected_list_model_assign_selected_object_id(SIMPLE_NOTES_SELECTED_LIST_MODEL(object), identifier);
    simple_notes_notes_model_changed(object);
}

void simple_notes_notes_model_change_note_content (SimpleNotesNotesModel *object, guint64 identifier, guint8 *newContent, guint len) {
    SimpleNotesObject *foundItem = simple_notes_selected_list_model_find_object(SIMPLE_NOTES_SELECTED_LIST_MODEL(object), identifier);
    if (foundItem) {
        SimpleNotesNote *foundNote = SIMPLE_NOTES_NOTE(foundItem);
        GByteArray *newContentArray = g_byte_array_new();
        g_byte_array_append(newContentArray, newContent, len);
        simple_notes_note_copy_content(foundNote, newContentArray);
        g_byte_array_unref(newContentArray);
        SimpleNotesObject *items[] = { foundItem };
        simple_notes_notes_model_real_save_objects(SIMPLE_NOTES_SELECTED_LIST_MODEL(object), items, 1);
        simple_notes_notes_model_changed(object);
    }
}

guint64 simple_notes_notes_model_get_folder_id (SimpleNotesNotesModel *object) {
    return object->_folderID;
}

void simple_notes_notes_model_assign_folder_id (SimpleNotesNotesModel *object, guint64 folderID) {
    object->_folderID = folderID;
}

static void simple_notes_notes_model_changed (SimpleNotesNotesModel *object) {
    simple_notes_model_changed(SIMPLE_NOTES_BASE_MODEL(object));
}

static void simple_notes_notes_model_real_save_objects (
        SimpleNotesSelectedListModel *object,
        SimpleNotesObject *items[],
        glong count
) {
  /*simple_notes_save_objects(object, items, count, ^(SimpleNotesObject *item) {
        SimpleNotesNote *note = SIMPLE_NOTES_NOTE(item);
        simple_notes_sqlite_controller_save_note(note);
	});*/
}

static GList *simple_notes_notes_model_real_create_items (SimpleNotesListModel *object) {
    SimpleNotesNotesModel *self = SIMPLE_NOTES_NOTES_MODEL(object);
    g_return_val_if_fail(self, NULL);
    return simple_notes_sqlite_controller_copy_notes(simple_notes_notes_model_get_folder_id(self));
}
