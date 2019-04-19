//
//  notes_model_testing.c
//  simple_notes
//
//  Created by Petr Yanenko on 12/1/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include <string.h>
#include "mediator.h"
#include "sqlite_controller.h"


#define NOTES_MODEL_TESTING


typedef struct
{
  SimpleNotesMediator *_model;
  guint64 _folderID;
} SimpleNotesNotesModelFixture;

static void
simple_notes_notes_model_fixture_set_up (SimpleNotesNotesModelFixture *fixture,
                                         gconstpointer user_data)
{
  simple_notes_sqlite_controller_delete_test_db ();
  fixture->_folderID = 1;
  fixture->_model = simple_notes_mediator_new ();
  SimpleNotesFoldersModel *folders
    = simple_notes_mediator_get_folders_model (fixture->_model);
  simple_notes_folders_model_insert_folder (folders,
                                            (guint8 *)"New Folder\0");
  simple_notes_folders_model_select_folder (folders, fixture->_folderID);
}

static void
simple_notes_notes_model_fixture_tear_down
(SimpleNotesNotesModelFixture *fixture, gconstpointer user_data)
{
  g_clear_object (&(fixture->_model));
  fixture->_folderID = 0;
}

static void
test_simple_notes_notes_model_all (SimpleNotesNotesModelFixture *fixture,
                                   gconstpointer user_data)
{
  guint count;
  gchar *const contentString = "Новый Контент\0";
  time_t start = time (NULL);
  SimpleNotesNotesModel *model
    = simple_notes_mediator_get_notes_model (fixture->_model);
  simple_notes_notes_model_insert_note (model,
                                        (guint8 *)contentString);
  time_t finish = time (NULL);
  SimpleNotesLightNote **newNotes
    = simple_notes_notes_model_copy_notes (model, &count);
  g_assert_cmpint (count, ==, 1);

  GByteArray *newContent
    = simple_notes_light_note_get_copy_content (newNotes[0]);
  g_assert_cmpstr ((gchar *)newContent->data, ==, contentString);
  g_byte_array_unref (newContent);

  guint64 identifier = simple_notes_light_note_get_id (newNotes[0]);
  g_assert_cmpuint (identifier, ==, 1);
  g_assert_cmpuint (simple_notes_light_note_get_folder_id (newNotes[0]),
                    ==,
                    fixture->_folderID);

  GDateTime *lastEdited
    = simple_notes_light_note_get_copy_last_edited (newNotes[0]);
  gint64 lastEditedUnix = g_date_time_to_unix (lastEdited);
  g_assert_cmpint (lastEditedUnix, >=, start);
  g_assert_cmpint (lastEditedUnix, <=, finish);
  g_date_time_unref (lastEdited);
  g_assert_false (simple_notes_light_note_get_selected (newNotes[0]));

  g_object_unref (newNotes[0]);
  g_free (newNotes);

  simple_notes_notes_model_select_note (model, identifier);
  SimpleNotesLightNote **selectedNote
    = simple_notes_notes_model_copy_notes (model, &count);
  guint64 selectedID = simple_notes_light_note_get_id (selectedNote[0]);
  g_assert_cmpuint (count, ==, 1);
  g_assert_cmpuint (identifier, ==, selectedID);
  g_assert_cmpint (simple_notes_light_note_get_selected (selectedNote[0]),
                   ==,
                   TRUE);
  g_object_unref (selectedNote[0]);
  g_free (selectedNote);

  gchar *const updatedContentString = "Обновление контента\0";
  simple_notes_notes_model_change_note_content
    (model,
     selectedID,
     (guint8 *)updatedContentString,
     (guint)strlen (updatedContentString) + 2);
  SimpleNotesLightNote **updatedContentNote
    = simple_notes_notes_model_copy_notes (model, &count);
  GByteArray *updatedContent
    = simple_notes_light_note_get_copy_content (updatedContentNote[0]);
  g_assert_cmpuint (count, ==, 1);
  g_assert_cmpuint (simple_notes_light_note_get_id (updatedContentNote[0]),
                    ==,
                    identifier);
  g_assert_cmpstr ((gchar *)updatedContent->data, ==, updatedContentString);
  g_byte_array_unref (updatedContent);
  g_object_unref (updatedContentNote[0]);
  g_free (updatedContentNote);

  simple_notes_notes_model_delete_note (model, identifier);
  SimpleNotesLightNote **afterDelete
    = simple_notes_notes_model_copy_notes (model, &count);
  g_assert_cmpuint (count, ==, 0);
  g_assert_false (afterDelete);
}
