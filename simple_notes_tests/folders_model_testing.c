//
//  folders_model_testing.c
//  simple_notes
//
//  Created by Petr Yanenko on 11/18/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "mediator.h"
#include <string.h>

#define FOLDERS_MODEL_TESTING

typedef struct {
    SimpleNotesMediator *_model;
} SimpleNotesFoldersModelFixture;

static void
simple_notes_folders_model_fixture_set_up (SimpleNotesFoldersModelFixture *fixture, gconstpointer user_data) {
    simple_notes_delete_test_db();
    fixture->_model = simple_notes_mediator_new();
}

static void
simple_notes_folders_model_fixture_tear_down (SimpleNotesFoldersModelFixture *fixture, gconstpointer user_data) {
    g_clear_object(&(fixture->_model));
}

static void
test_simple_notes_folders_model_all (SimpleNotesFoldersModelFixture *fixture, gconstpointer user_data) {
    guint count = 0;
    SimpleNotesLightFolder **empty = simple_notes_folders_model_copy_folders(simple_notes_mediator_get_folders_model(fixture->_model), &count);
    g_assert_cmpint(count, ==, 0);
    g_free(empty);

    gchar *const name = "Новая Папка\0";
    simple_notes_folders_model_insert_folder(simple_notes_mediator_get_folders_model(fixture->_model), (guint8 *)name);
    simple_notes_folders_model_insert_folder(simple_notes_mediator_get_folders_model(fixture->_model), NULL);
    SimpleNotesLightFolder **newFolders = simple_notes_folders_model_copy_folders(simple_notes_mediator_get_folders_model(fixture->_model), &count);
    g_assert_cmpint(count, ==, 2);

    GByteArray *title = simple_notes_light_folder_get_copy_title(newFolders[0]);
    g_assert_cmpstr((gchar *)title->data, ==, name);
    g_byte_array_unref(title);

    GByteArray *nullTitle = simple_notes_light_folder_get_copy_title(newFolders[1]);
    g_assert_cmpstr((gchar *)nullTitle->data, ==, NULL);
    g_byte_array_unref(nullTitle);

    simple_notes_folders_model_delete_folder(simple_notes_mediator_get_folders_model(fixture->_model), simple_notes_light_folder_get_id(newFolders[1]));
    SimpleNotesLightFolder **afterDelete = simple_notes_folders_model_copy_folders(simple_notes_mediator_get_folders_model(fixture->_model), &count);
    g_assert_cmpuint(count, ==, 1);
    
    guint64 identifier = simple_notes_light_folder_get_id(afterDelete[0]);
    g_assert_cmpuint(identifier, ==, simple_notes_light_folder_get_id(newFolders[0]));
    
    g_object_unref(newFolders[0]);
    g_object_unref(newFolders[1]);
    g_free(newFolders);
    g_object_unref(afterDelete[0]);
    g_free(afterDelete);

    simple_notes_folders_model_select_folder(simple_notes_mediator_get_folders_model(fixture->_model), identifier);
    SimpleNotesLightFolder **selectedFolder = simple_notes_folders_model_copy_folders(simple_notes_mediator_get_folders_model(fixture->_model), &count);
    guint64 selectedID = simple_notes_light_folder_get_id(selectedFolder[0]);
    g_assert_cmpuint(count, ==, 1);
    g_assert_cmpuint(identifier, ==, selectedID);
    g_assert_cmpint(simple_notes_light_folder_get_selected(selectedFolder[0]), ==, TRUE);
    g_object_unref(selectedFolder[0]);
    g_free(selectedFolder);

    gchar *const newName = "Новая\0";
    simple_notes_folders_model_change_folder_title(simple_notes_mediator_get_folders_model(fixture->_model), selectedID, (guint8 *)newName, (guint)strlen(newName) + 2);
    SimpleNotesLightFolder **newNameFolder = simple_notes_folders_model_copy_folders(simple_notes_mediator_get_folders_model(fixture->_model), &count);
    GByteArray *newTitle = simple_notes_light_folder_get_copy_title(newNameFolder[0]);
    g_assert_cmpuint(count, ==, 1);
    g_assert_cmpstr((gchar *)newTitle->data, ==, newName);
    g_object_unref(newNameFolder[0]);
    g_free(newNameFolder);
    g_byte_array_unref(newTitle);
}
