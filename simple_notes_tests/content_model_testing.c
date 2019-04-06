//
//  content_model_testing.c
//  simple_notes
//
//  Created by Petr Yanenko on 12/12/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include <string.h>
#include "mediator.h"
#include "model_utility.h"

#define CONTENT_MODEL_TESTING

typedef struct {
    SimpleNotesMediator *_model;
    gchar *_folderName;
    guint64 _folderID;
    guint64 _noteID;
} SimpleNotesContentModelFixture;

static void
simple_notes_content_model_fixture_set_up (SimpleNotesContentModelFixture *fixture, gconstpointer user_data) {
    GError *error = NULL;
    GFile *noteFolder = g_file_new_for_path(kNoteFolder);
    if (g_file_trash(noteFolder, NULL, &error)) {
        g_message("\n\nNotes deleted\n", NULL);
    } else {
        SIMPLE_NOTES_PRINT_ERROR(&error);
        SIMPLE_NOTES_CLEAR_ERROR(&error);
    }
    g_object_unref(noteFolder);

    fixture->_folderName = "Test Folder\0";
    simple_notes_delete_test_db();
    fixture->_model = simple_notes_mediator_new();

    guint count = 0;

    SimpleNotesFoldersModel *folders = simple_notes_mediator_get_folders_model(fixture->_model);
    simple_notes_folders_model_insert_folder(folders, (guint8 *)fixture->_folderName);
    SimpleNotesLightFolder **newFolders = simple_notes_folders_model_copy_folders(folders, &count);
    guint64 folderIdentifier = simple_notes_light_folder_get_id(newFolders[0]);
    fixture->_folderID = folderIdentifier;
    simple_notes_free_objects_array((gpointer *)newFolders, count);
    simple_notes_folders_model_select_folder(folders, fixture->_folderID);

    SimpleNotesNotesModel *notes = simple_notes_mediator_get_notes_model(fixture->_model);
    simple_notes_notes_model_insert_note(notes, NULL);
    SimpleNotesLightNote **newNotes = simple_notes_notes_model_copy_notes(notes, &count);
    guint64 noteIdentifier = simple_notes_light_note_get_id(newNotes[0]);
    fixture->_noteID = noteIdentifier;
    simple_notes_free_objects_array((gpointer *)newNotes, count);
    simple_notes_notes_model_select_note(notes, fixture->_noteID);
}

static void
simple_notes_content_model_fixture_tear_down (SimpleNotesContentModelFixture *fixture, gconstpointer user_data) {
    g_clear_object(&(fixture->_model));
}

static void
test_simple_notes_content_model_all (SimpleNotesContentModelFixture *fixture, gconstpointer user_data) {
    SimpleNotesContentModel *model = simple_notes_mediator_get_content_model(fixture->_model);
    simple_notes_content_model_insert_text(model, NULL, 0, 0, 0, NULL);
    guint count = 0;
    SimpleNotesLightString **content = simple_notes_content_model_copy_items(model, &count);
    g_assert_cmpuint(count, ==, 1);
    GByteArray *contentData = simple_notes_light_string_get_text(content[0]);
    g_assert_cmpmem(contentData->data, contentData->len, NULL, 0);
    simple_notes_free_objects_array((gpointer *)content, count);

    simple_notes_content_model_insert_text(model, (guint8 *)"tst", 3, 0, 0, NULL);
    simple_notes_content_model_insert_text(model, (guint8 *)"e", 1, 1, 0, NULL);
    simple_notes_content_model_insert_text(model, (guint8 *)"This is a ", 10, 0, 0, NULL);
    simple_notes_content_model_insert_text(model, (guint8 *)".", 1, 14, 0, NULL);
    SimpleNotesLightString **newContent = simple_notes_content_model_copy_items(model, &count);
    GByteArray *newContentData = simple_notes_light_string_get_text(newContent[0]);
    g_assert_cmpmem(newContentData->data, newContentData->len, "This is a test.", 15);
    simple_notes_free_objects_array((gpointer *)newContent, count);

    simple_notes_content_model_insert_text(model, (guint8 *)"", 0, 0, 5, NULL);
    simple_notes_content_model_insert_text(model, (guint8 *)"", 0, 8, 1, NULL);
    simple_notes_content_model_insert_text(model, (guint8 *)"", 0, 2, 1, NULL);
    simple_notes_content_model_insert_text(model, (guint8 *)"", 0, 0, 8, NULL);
    SimpleNotesLightString **afterDelete = simple_notes_content_model_copy_items(model, &count);
    GByteArray *afterDeleteData = simple_notes_light_string_get_text(afterDelete[0]);
    g_assert_cmpmem(afterDeleteData->data, afterDeleteData->len, NULL, 0);
    simple_notes_free_objects_array((gpointer *)afterDelete, count);

    gchar *newTest = "It is a new test";
    simple_notes_content_model_insert_text(model, (guint8 *)newTest, 16, 0, 0, NULL);
    simple_notes_content_model_insert_text(model, (guint8 *)"This", 4, 0, 2, NULL);
    simple_notes_content_model_insert_text(model, (guint8 *)"test.", 5, 14, 4, NULL);
    simple_notes_content_model_insert_text(model, (guint8 *)"the", 3, 8, 1, NULL);
    SimpleNotesLightString **complex = simple_notes_content_model_copy_items(model, &count);
    GByteArray *complexData = simple_notes_light_string_get_text(complex[0]);
    g_assert_cmpmem(complexData->data, complexData->len, "This is the new test.", 21);
    simple_notes_free_objects_array((gpointer *)complex, count);
}
