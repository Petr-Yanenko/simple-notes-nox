//
//  sqlite_controller_testing.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/18/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "sqlite_controller.h"
#include <gio.h>
#include <string.h>


#define SQLITE_CONTROLLER_TESTING


typedef struct
{

} SimpleNotesSQLiteControllerFixture;

static void
simple_notes_sqlite_controller_fixture_set_up
(SimpleNotesSQLiteControllerFixture *fixture, gconstpointer user_data)
{
  simple_notes_sqlite_controller_delete_test_db ();
}

static void
simple_notes_sqlite_controller_fixture_tear_down
(SimpleNotesSQLiteControllerFixture *fixture, gconstpointer user_data)
{

}

static void
simple_notes_sqlite_controller_check_other_folder_fields
(SimpleNotesFolder *folder)
{
  g_assert_cmpuint (simple_notes_object_get_id (SIMPLE_NOTES_OBJECT (folder)),
                    >, 0);
  g_assert_cmpint (simple_notes_folder_get_count (folder), ==, 0);
  g_assert_false (simple_notes_object_get_selected
                  (SIMPLE_NOTES_OBJECT (folder)));
}

static void
simple_notes_sqlite_controller_check_title (GList *item, gchar value[])
{
  GByteArray *title = simple_notes_folder_get_copy_title (item->data);
  g_assert_cmpstr ((gchar *)title->data, ==, value);
  g_byte_array_unref (title);
}

static void
simple_notes_sqlite_controller_check_folder (GList *item, gchar title[])
{
  simple_notes_sqlite_controller_check_other_folder_fields (item->data);
  simple_notes_sqlite_controller_check_title (item, title);
}

static void
simple_notes_sqlite_controller_check_folders (GList *folders,
                                              gchar *titleValues[])
{
  glong i = 0;
  while (folders)
    {
      simple_notes_sqlite_controller_check_folder (folders, titleValues[i]);
      i++;
      folders = folders->next;
    }
}

static void
test_simple_notes_sqlite_controller_folders
(SimpleNotesSQLiteControllerFixture *fixture, gconstpointer user_data)
{

  GList *emptyList = simple_notes_sqlite_controller_copy_folders ();
  g_assert_null (emptyList);

  gchar *first = "first\0", *second = "second\0", *empty = "\0";
  simple_notes_sqlite_controller_insert_folder ((guint8 *)first);
  simple_notes_sqlite_controller_insert_folder ((guint8 *)second);
  simple_notes_sqlite_controller_insert_folder (NULL);
  simple_notes_sqlite_controller_insert_folder ((guint8 *)empty);

  GList *folders = simple_notes_sqlite_controller_copy_folders ();
  gchar *titleValues[] =
    {
      first,
      second,
      NULL,
      NULL
    };
  simple_notes_sqlite_controller_check_folders (folders, titleValues);

  gulong newCount = 10;
  SimpleNotesFolder *updatedFolder = folders->data;
  GByteArray *newTitle = g_byte_array_new ();
  gchar *newString = "Новая Папка\0";
  g_byte_array_append (newTitle, (const guint8 *)newString,
                       (guint)strlen (newString) + 2);
  simple_notes_folder_assign_count (updatedFolder, newCount);
  simple_notes_folder_copy_title (updatedFolder, newTitle);
  simple_notes_object_assign_selected (SIMPLE_NOTES_OBJECT (updatedFolder),
                                       TRUE);
  simple_notes_sqlite_controller_save_folder (updatedFolder);
  GList *newFolders = simple_notes_sqlite_controller_copy_folders ();
  SimpleNotesFolder *newFolder = newFolders->data;
  g_assert_cmpuint
    (simple_notes_object_get_id (SIMPLE_NOTES_OBJECT (newFolder)),
     ==,
     simple_notes_object_get_id (SIMPLE_NOTES_OBJECT (updatedFolder)));
  g_assert_cmpint (simple_notes_folder_get_count (newFolder), ==, newCount);
  GByteArray *cmpTitle = simple_notes_folder_get_copy_title (newFolder);
  g_assert_cmpstr ((gchar *)cmpTitle->data, ==, newString);

  simple_notes_sqlite_controller_delete_folder (newFolder);
  guint64 deletedID
    = simple_notes_object_get_id (SIMPLE_NOTES_OBJECT (newFolder));
  GList *foldersAfterDelete = simple_notes_sqlite_controller_copy_folders ();
  GList *item = foldersAfterDelete;
  while (item)
    {
      guint64 itemID
        = simple_notes_object_get_id (SIMPLE_NOTES_OBJECT (item->data));
      g_assert_cmpuint (itemID, !=, deletedID);
      item = item->next;
    }
  g_byte_array_unref (cmpTitle);
  g_byte_array_unref (newTitle);
  g_list_free_full (newFolders, g_object_unref);
  g_list_free_full (foldersAfterDelete, g_object_unref);
  g_list_free_full (folders, g_object_unref);
}

static void
test_simple_notes_sqlite_controller_notes
(SimpleNotesSQLiteControllerFixture *fixture, gconstpointer user_data)
{
  guint64 folderID = 1;
  gchar *content = "New content\0";
  time_t lastEdited = time (NULL);
  simple_notes_sqlite_controller_insert_note ((guint8 *)content,
                                              folderID, lastEdited);

  GList *notes = simple_notes_sqlite_controller_copy_notes (folderID);
  SimpleNotesNote *note = notes->data;
  g_assert_cmpuint (simple_notes_note_get_id (note), ==, 1);
  g_assert_cmpuint (simple_notes_note_get_folder_id (note), ==, folderID);
  GDateTime *noteTime = simple_notes_note_get_copy_last_edited (note);
  g_assert_cmpint (g_date_time_to_unix (noteTime), ==, lastEdited);
  g_date_time_unref (noteTime);
  GByteArray *contentArray = simple_notes_note_get_copy_content (note);
  g_assert_cmpstr ((gchar *)contentArray->data, ==, content);
  g_byte_array_unref (contentArray);
  g_assert_false (simple_notes_note_get_selected (note));

  SimpleNotesNote *updatedNote = notes->data;
  GByteArray *newContent = g_byte_array_new ();
  gchar *newString = "Новый контент\0";
  g_byte_array_append (newContent, (const guint8 *)newString,
                       (guint)strlen (newString) + 2);
  simple_notes_note_copy_content (updatedNote, newContent);
  g_byte_array_unref (newContent);
  simple_notes_note_assign_selected (updatedNote, TRUE);
  simple_notes_sqlite_controller_save_note (updatedNote);
  GList *newNotes = simple_notes_sqlite_controller_copy_notes (folderID);
  SimpleNotesNote *newNote = newNotes->data;

  GByteArray *cmpContent = simple_notes_note_get_copy_content (newNote);
  g_assert_cmpstr ((gchar *)cmpContent->data, ==, newString);
  g_byte_array_unref (cmpContent);
  g_assert_true (simple_notes_note_get_selected (newNote));

  g_list_free_full (newNotes, g_object_unref);
  g_list_free_full (notes, g_object_unref);
}
