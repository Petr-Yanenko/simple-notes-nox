//
//  main.c
//  simple_notes_tests
//
//  Created by Petr Yanenko on 10/21/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include <locale.h>
#include <stdio.h>

#include "sqlite_controller_testing.c"
#include "folders_model_testing.c"
#include "notes_model_testing.c"
#include "content_model_testing.c"

int main(int argc, char **argv)
{
    setlocale (LC_ALL, "");
    
//    for (glong i = 0; i < 1000; i++) {
//
//        printf("\n%ld\n", i);
//
//        SimpleNotesSQLiteControllerFixture sqliteController;
//        simple_notes_sqlite_controller_fixture_set_up(&sqliteController, NULL);
//        test_simple_notes_sqlite_controller_folders(&sqliteController, NULL);
//        simple_notes_sqlite_controller_fixture_tear_down(&sqliteController, NULL);
//
//        simple_notes_sqlite_controller_fixture_set_up(&sqliteController, NULL);
//        test_simple_notes_sqlite_controller_notes(&sqliteController, NULL);
//        simple_notes_sqlite_controller_fixture_tear_down(&sqliteController, NULL);
//
//        SimpleNotesFoldersModelFixture foldersModel;
//        simple_notes_folders_model_fixture_set_up(&foldersModel, NULL);
//        test_simple_notes_folders_model_all(&foldersModel, NULL);
//        simple_notes_folders_model_fixture_tear_down(&foldersModel, NULL);
//
//        SimpleNotesNotesModelFixture notesModel;
//        simple_notes_notes_model_fixture_set_up(&notesModel, NULL);
//        test_simple_notes_notes_model_all(&notesModel, NULL);
//        simple_notes_notes_model_fixture_tear_down(&notesModel, NULL);
//
//        SimpleNotesContentModelFixture contentModel;
//        simple_notes_content_model_fixture_set_up(&contentModel, NULL);
//        test_simple_notes_content_model_all(&contentModel, NULL);
//        simple_notes_content_model_fixture_tear_down(&contentModel, NULL);
//    }
//    return 0;
    
    g_test_init(&argc, &argv, NULL);

#ifdef SQLITE_CONTROLLER_TESTING
    g_test_add(
               "/simple-notes-sqlite-controller/folders",
               SimpleNotesSQLiteControllerFixture,
               NULL,
               simple_notes_sqlite_controller_fixture_set_up,
               test_simple_notes_sqlite_controller_folders,
               simple_notes_sqlite_controller_fixture_tear_down
               );
    g_test_add(
               "/simple-notes-sqlite-controller/notes",
               SimpleNotesSQLiteControllerFixture,
               NULL,
               simple_notes_sqlite_controller_fixture_set_up,
               test_simple_notes_sqlite_controller_notes,
               simple_notes_sqlite_controller_fixture_tear_down
               );
#endif

#ifdef FOLDERS_MODEL_TESTING
    g_test_add(
               "/simple-notes-folders-model/all",
               SimpleNotesFoldersModelFixture,
               NULL,
               simple_notes_folders_model_fixture_set_up,
               test_simple_notes_folders_model_all,
               simple_notes_folders_model_fixture_tear_down
               );
#endif

#ifdef NOTES_MODEL_TESTING
    g_test_add(
                "/simple-notes-notes-model/all",
                SimpleNotesNotesModelFixture,
                NULL,
                simple_notes_notes_model_fixture_set_up,
                test_simple_notes_notes_model_all,
                simple_notes_notes_model_fixture_tear_down
    );
#endif

#ifdef CONTENT_MODEL_TESTING
    g_test_add(
               "/simple-notes-content-model/all",
               SimpleNotesContentModelFixture,
               NULL,
               simple_notes_content_model_fixture_set_up,
               test_simple_notes_content_model_all,
               simple_notes_content_model_fixture_tear_down
    );
#endif

    return g_test_run();
}

