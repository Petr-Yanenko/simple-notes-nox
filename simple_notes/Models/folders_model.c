//
//  folders_model.c
//  simple_notes
//
//  Created by Petr Yanenko on 11/11/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "folders_model.h"
#include "model_utility.h"
#include "mediator.h"
#include <stdio.h>
#include <string.h>

struct _SimpleNotesFoldersModel {
    SimpleNotesSelectedListModel parent;
};

G_DEFINE_TYPE(SimpleNotesFoldersModel, simple_notes_folders_model, SIMPLE_NOTES_TYPE_SELECTED_LIST_MODEL)

static void simple_notes_folders_model_real_save_objects (
        SimpleNotesSelectedListModel *object,
        SimpleNotesObject *items[],
        glong count
);
static GList *simple_notes_folders_model_real_create_items (SimpleNotesListModel *object);
static void simple_notes_folders_model_changed (SimpleNotesFoldersModel *object);
static void simple_notes_folders_model_save_objects (
        SimpleNotesFoldersModel *object,
        SimpleNotesObject *items[],
        glong count
);

static void simple_notes_folders_model_class_init (SimpleNotesFoldersModelClass *klass) {
    SimpleNotesSelectedListModelClass *selectedModelClass = SIMPLE_NOTES_SELECTED_LIST_MODEL_CLASS(klass);
    selectedModelClass->save_objects = simple_notes_folders_model_real_save_objects;

    SimpleNotesListModelClass *listModelClass = SIMPLE_NOTES_LIST_MODEL_CLASS(klass);
    listModelClass->create_items = simple_notes_folders_model_real_create_items;
}

static void simple_notes_folders_model_init (SimpleNotesFoldersModel *object) {

}

SimpleNotesFoldersModel *simple_notes_folders_model_new (SimpleNotesMediator *mediator) {
    SimpleNotesFoldersModel *instance = g_object_new(SIMPLE_NOTES_TYPE_FOLDERS_MODEL, NULL);
    simple_notes_selected_list_model_set_weak_ref_mediator(SIMPLE_NOTES_SELECTED_LIST_MODEL(instance), mediator);
    return instance;
}

void simple_notes_folders_model_insert_folder (SimpleNotesFoldersModel *object, guint8 *name) {
    simple_notes_sqlite_controller_insert_folder(name);
    simple_notes_folders_model_changed(object);
}

SimpleNotesLightFolder **simple_notes_folders_model_copy_folders (SimpleNotesFoldersModel *object, guint *out_count) {
    return (SimpleNotesLightFolder **) SIMPLE_NOTES_SELECTED_LIST_MODEL_CLASS(simple_notes_folders_model_parent_class)->copy_items(
            SIMPLE_NOTES_SELECTED_LIST_MODEL(object), out_count
    );
}

void simple_notes_folders_model_delete_folder (SimpleNotesFoldersModel *object, guint64 identifier) {
    simple_notes_delete_object(SIMPLE_NOTES_SELECTED_LIST_MODEL(object), identifier, ^(SimpleNotesObject *deletedObject) {
        simple_notes_sqlite_controller_delete_folder(SIMPLE_NOTES_FOLDER(deletedObject));
        gchar path[kFolderPathSymbols + 1];
        sprintf(path, kFolderPathFormat, kNoteFolder, identifier);
        simple_notes_trash_file(path);
        
    });
    simple_notes_folders_model_changed(object);
}

void simple_notes_folders_model_select_folder (SimpleNotesFoldersModel *object, guint64 identifier) {
    simple_notes_selected_list_model_assign_selected_object_id(SIMPLE_NOTES_SELECTED_LIST_MODEL(object), identifier);
    simple_notes_folders_model_changed(object);
}

void simple_notes_folders_model_change_folder_title (SimpleNotesFoldersModel *object, guint64 identifier, guint8 *newTitle, guint len) {
    SimpleNotesObject *foundItem = simple_notes_selected_list_model_find_object(SIMPLE_NOTES_SELECTED_LIST_MODEL(object), identifier);
    if (foundItem) {
        SimpleNotesFolder *foundFolder = SIMPLE_NOTES_FOLDER(foundItem);
        GByteArray *newTitleArray = g_byte_array_new();
        g_byte_array_append(newTitleArray, newTitle, len);
        simple_notes_folder_copy_title(foundFolder, newTitleArray);
        g_byte_array_unref(newTitleArray);
        SimpleNotesObject *items[] = { foundItem };
        simple_notes_folders_model_save_objects(object, items, 1);
        simple_notes_folders_model_changed(object);
    }
}

static void simple_notes_folders_model_changed (SimpleNotesFoldersModel *object) {
    simple_notes_model_changed(SIMPLE_NOTES_BASE_MODEL(object));
}

static void simple_notes_folders_model_save_objects (
        SimpleNotesFoldersModel *object,
        SimpleNotesObject *items[],
        glong count
) {
    SimpleNotesSelectedListModelClass *klass = SIMPLE_NOTES_SELECTED_LIST_MODEL_CLASS(G_OBJECT(object)->g_type_instance.g_class);
    klass->save_objects(SIMPLE_NOTES_SELECTED_LIST_MODEL(object), items, count);
}

static void simple_notes_folders_model_real_save_objects (
        SimpleNotesSelectedListModel *object,
        SimpleNotesObject *items[],
        glong count
                                                          ) {
    simple_notes_save_objects(object, items, count, ^(SimpleNotesObject *item) {
        SimpleNotesFolder *folder = SIMPLE_NOTES_FOLDER(item);
        simple_notes_sqlite_controller_save_folder(folder);
    });
}

static GList *simple_notes_folders_model_real_create_items (SimpleNotesListModel *object) {
    return simple_notes_sqlite_controller_copy_folders();
}
