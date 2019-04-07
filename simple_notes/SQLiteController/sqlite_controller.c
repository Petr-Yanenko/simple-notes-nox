//
//  sqlite_controller.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/10/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "sqlite_controller.h"
#include "sqlite_store.h"
#include <stdio.h>

enum {
    CREATE_FOLDERS_TABLE_INDEX = 0,
    SELECT_FOLDERS_INDEX,
    UPDATE_FOLDER_INDEX,
    DELETE_FOLDER_INDEX,
    INSERT_FOLDER_INDEX,
    CREATE_NOTES_TABLE_INDEX,
    SELECT_NOTES_INDEX,
    UPDATE_NOTE_INDEX,
    DELETE_NOTE_INDEX,
    INSERT_NOTE_INDEX,
    BEGIN_TRANSACTION_INDEX,
    COMMIT_TRANSACTION_INDEX,
    NUMBER_OF_INDEXES
};

static gchar *kStatementNames[] = {
        "create-folders-table\0",
        "select-folders\0",
        "update-folder\0",
        "delete-folder\0",
        "insert-folder\0",
        "create-notes-table\0",
        "select-notes\0",
        "update-note\0",
        "delete-note\0",
        "insert-note\0",
        "begin-transaction\0",
        "commit-transaction\0"
};

static gchar *const kFoldersKey = "folders\0";
static gchar *const kIDKey = "id\0";
static gchar *const kTitleKey = "title\0";
static gchar *const kCountKey = "count\0";
static gchar *const kSelectedKey = "selected\0";
static gchar *const kNotesKey = "notes\0";
static gchar *const kContentKey = "content\0";
static gchar *const kLastEditedKey = "last_edited\0";
static gchar *const kFolderIDKey = "folder_id\0";

struct _SimpleNotesSQLiteController {
    GObject parent;

    SimpleNotesSQLiteStore *_store;
};

static SimpleNotesSQLiteController *simple_notes_sqlite_controller_new (void);
static GString **simple_notes_sqlite_controller_create_statements (SimpleNotesSQLiteController *object);
static void simple_notes_sqlite_controller_print_object_id (SimpleNotesObject *object, gchar *buff);
static void simple_notes_sqlite_controller_print_note_folder_id (SimpleNotesNote *object, gchar *buff);
static void simple_notes_sqlite_controller_print_note_date (SimpleNotesNote *object, gchar *buff);
static void simple_notes_sqlite_controller_print_object_selected (SimpleNotesObject *object, gchar *buff);
static void simple_notes_sqlite_controller_print_folder_count (SimpleNotesFolder *folder, gchar *buff);
static void simple_notes_sqlite_controller_delete_statements (GString **statements);
static void simple_notes_sqlite_controller_check_identifier (SimpleNotesSQLiteController *object, gint64 identifier);
static void simple_notes_sqlite_controller_delete_entity (SimpleNotesObject *entity, glong statementIndex);
static GByteArray *simple_notes_sqlite_controller_create_byte_array_for_column (gint column, sqlite3_stmt *statement);
/*static GList *simple_notes_sqlite_controller_copy_entities (
        SimpleNotesObject * (^create_entity)(void),
        void (^process_new_entity)(SimpleNotesObject *newEntity, sqlite3_stmt *statement),
        gboolean (^execute_prepared_statement) (SimpleNotesSQLiteStore *object, void (^callback)(sqlite3_stmt *)),
        gint selectedColumnIndex
	);*/
static void simple_notes_sqlite_controller_print_identifier (gchar *buff, guint64 identifier);
static void simple_notes_sqlite_controller_print_unix_time (gchar *buff, time_t time);

G_DEFINE_TYPE (SimpleNotesSQLiteController, simple_notes_sqlite_controller, G_TYPE_OBJECT)

static SimpleNotesSQLiteController *_instance = NULL;

SimpleNotesSQLiteController *simple_notes_sqlite_controller_get_instance (void) {
    if (!_instance) {
        _instance = simple_notes_sqlite_controller_new();
    }
    return _instance;
}

static void simple_notes_sqlite_controller_dispose (GObject *object) {
    SimpleNotesSQLiteController *controller = SIMPLE_NOTES_SQLITE_CONTROLLER(object);
    g_clear_object(&(controller->_store));
    G_OBJECT_CLASS(simple_notes_sqlite_controller_parent_class)->dispose(object);
}

static void simple_notes_sqlite_controller_finalize (GObject *object) {

    G_OBJECT_CLASS(simple_notes_sqlite_controller_parent_class)->finalize(object);
}

static void simple_notes_sqlite_controller_class_init (SimpleNotesSQLiteControllerClass *classObject) {
    GObjectClass *gClass = G_OBJECT_CLASS(classObject);
    gClass->dispose = simple_notes_sqlite_controller_dispose;
    gClass->finalize = simple_notes_sqlite_controller_finalize;
}

static void simple_notes_sqlite_controller_init (SimpleNotesSQLiteController *object) {
    object->_store = simple_notes_sqlite_store_new();
    GString **statements = simple_notes_sqlite_controller_create_statements(object);
    for (glong i = 0; i < NUMBER_OF_INDEXES; i++) {
        gboolean ok = simple_notes_sqlite_store_register_prepared_statement(
                object->_store,
                kStatementNames[i],
                statements[i]->str
        );
        if (i == CREATE_FOLDERS_TABLE_INDEX || i == CREATE_NOTES_TABLE_INDEX) {
	  /* ok &= simple_notes_sqlite_store_execute_prepared_statement(
                    object->_store,
                    kStatementNames[i],
                    NULL,
                    0,
                    NULL
		    );*/
        }
        g_warn_if_fail(ok);
        if (!ok) {
            break;
        }
    }
    simple_notes_sqlite_controller_delete_statements(statements);
}

GList *simple_notes_sqlite_controller_copy_folders(void) {
    gint selectedColumnIndex = 3;
    /* return simple_notes_sqlite_controller_copy_entities(^SimpleNotesObject * {
        SimpleNotesFolder *newFolder = simple_notes_folder_new();
        return SIMPLE_NOTES_OBJECT(newFolder);
    }, ^(SimpleNotesObject *newEntity, sqlite3_stmt *statement) {
        SimpleNotesFolder *newFolder = SIMPLE_NOTES_FOLDER(newEntity);
        GByteArray *title = simple_notes_sqlite_controller_create_byte_array_for_column(1, statement);
        if (title) {
            simple_notes_folder_copy_title(newFolder, title);
            g_byte_array_unref(title);
        }
        glong count = sqlite3_column_int(statement, 2);
        simple_notes_folder_assign_count(newFolder, count);
    }, ^gboolean (SimpleNotesSQLiteStore *object, void (^callback) (sqlite3_stmt *)) {
        return simple_notes_sqlite_store_execute_prepared_statement(object, kStatementNames[SELECT_FOLDERS_INDEX], callback, 0, NULL);
	}, selectedColumnIndex);*/
    return NULL;
}

void simple_notes_sqlite_controller_insert_folder (guint8 *const title) {
    SimpleNotesSQLiteController *object = simple_notes_sqlite_controller_get_instance();
    long parametersCount = 3;

    gboolean result = FALSE/*simple_notes_sqlite_store_execute_prepared_statement(
            object->_store,
            kStatementNames[INSERT_FOLDER_INDEX],
            NULL,
            parametersCount,
            title,
            "0\0",
            "0\0",
            NULL
	    )*/;
    g_return_if_fail(result);
}

void simple_notes_sqlite_controller_save_folder (SimpleNotesFolder *folder) {
    g_return_if_fail(folder);
    SimpleNotesSQLiteController *object = simple_notes_sqlite_controller_get_instance();
    SimpleNotesObject *folderParent = SIMPLE_NOTES_OBJECT(folder);
    glong parametersCount = 4;
    GByteArray *title = simple_notes_folder_get_copy_title(folder);
    gchar count[kLongLongSymbols];
    simple_notes_sqlite_controller_print_folder_count(folder, count);
    gchar selected[kSelectedSymbols];
    simple_notes_sqlite_controller_print_object_selected(folderParent, selected);
    gchar identifier[kLongLongSymbols];
    simple_notes_sqlite_controller_print_object_id(folderParent, identifier);
    gboolean result = FALSE/*simple_notes_sqlite_store_execute_prepared_statement(
            object->_store,
            kStatementNames[UPDATE_FOLDER_INDEX],
            NULL,
            parametersCount,
            title ? title->data : NULL,
            count,
            selected,
            identifier,
            NULL
	    )*/;
    if (title) {
        g_byte_array_unref(title);
    }
    g_return_if_fail(result);
}

void simple_notes_sqlite_controller_delete_folder (SimpleNotesFolder *folder) {
    simple_notes_sqlite_controller_delete_entity(SIMPLE_NOTES_OBJECT(folder), DELETE_FOLDER_INDEX);
}

GList *simple_notes_sqlite_controller_copy_notes (guint64 folderID) {
    gint selectedColumnIndex = 4;
    gchar buff[kLongLongSymbols];
    gchar *printedFolderID = buff;
    simple_notes_sqlite_controller_print_identifier(buff, folderID);
    /* return simple_notes_sqlite_controller_copy_entities(^SimpleNotesObject * {
        SimpleNotesNote *newNote = simple_notes_note_new();
        return SIMPLE_NOTES_OBJECT(newNote);
    }, ^(SimpleNotesObject *newEntity, sqlite3_stmt *statement) {
        SimpleNotesNote *newNote = SIMPLE_NOTES_NOTE(newEntity);
        gint64 folderID = sqlite3_column_int64(statement, 1);
        simple_notes_sqlite_controller_check_identifier(simple_notes_sqlite_controller_get_instance(), folderID);
        simple_notes_note_assign_folder_id(newNote, (guint64)folderID);
        GByteArray *content = simple_notes_sqlite_controller_create_byte_array_for_column(2, statement);
        if (content) {
            simple_notes_note_copy_content(newNote, content);
            g_byte_array_unref(content);
        }
        gint64 unixTime = sqlite3_column_int64(statement, 3);
        GDateTime *lastEdited = g_date_time_new_from_unix_utc(unixTime);
        simple_notes_note_copy_last_edited(newNote, lastEdited);
        g_date_time_unref(lastEdited);
    }, ^gboolean (SimpleNotesSQLiteStore *object, void (^callback) (sqlite3_stmt *)) {
        return simple_notes_sqlite_store_execute_prepared_statement(object, kStatementNames[SELECT_NOTES_INDEX], callback, 1, printedFolderID);
	}, selectedColumnIndex);*/
    return NULL;
}

void simple_notes_sqlite_controller_insert_note (guint8 *const content, guint64 folderID, time_t lastEdited) {
    SimpleNotesSQLiteController *object = simple_notes_sqlite_controller_get_instance();
    glong parametersCount = 4;

    gchar folderIDString[kLongLongSymbols];
    simple_notes_sqlite_controller_print_identifier(folderIDString, folderID);

    gchar lastEditedString[kLongLongSymbols];
    simple_notes_sqlite_controller_print_unix_time(lastEditedString, lastEdited);

    gboolean result = FALSE/*simple_notes_sqlite_store_execute_prepared_statement(
            object->_store,
            kStatementNames[INSERT_NOTE_INDEX],
            NULL,
            parametersCount,
            folderIDString,
            content,
            lastEditedString,
            "0\0",
            NULL
	    )*/;
    g_return_if_fail(result);
}

void simple_notes_sqlite_controller_save_note (SimpleNotesNote *note) {
    g_return_if_fail(note);
    SimpleNotesSQLiteController *object = simple_notes_sqlite_controller_get_instance();
    SimpleNotesObject *noteParent = SIMPLE_NOTES_OBJECT(note);
    glong parametersCount = 5;
    GByteArray *content = simple_notes_note_get_copy_content(note);
    gchar folderID[kLongLongSymbols];
    simple_notes_sqlite_controller_print_note_folder_id(note, folderID);
    gchar selected[kSelectedSymbols];
    simple_notes_sqlite_controller_print_object_selected(noteParent, selected);
    gchar identifier[kLongLongSymbols];
    simple_notes_sqlite_controller_print_object_id(noteParent, identifier);
    gchar lastEdited[kLongLongSymbols];
    simple_notes_sqlite_controller_print_note_date(note, lastEdited);
    gboolean result = FALSE/*simple_notes_sqlite_store_execute_prepared_statement(
            object->_store,
            kStatementNames[UPDATE_NOTE_INDEX],
            NULL,
            parametersCount,
            folderID,
            content ? content->data : NULL,
            lastEdited,
            selected,
            identifier,
            NULL
	    )*/;
    if (content) {
        g_byte_array_unref(content);
    }
    g_return_if_fail(result);
}

void simple_notes_sqlite_controller_delete_note (SimpleNotesNote *note) {
    simple_notes_sqlite_controller_delete_entity(SIMPLE_NOTES_OBJECT(note), DELETE_NOTE_INDEX);
}

void simple_notes_sqlite_controller_testing_unref_instance (void) {
    g_clear_object(&_instance);
}

void simple_notes_sqlite_controller_begin_transaction (void) {
  /*simple_notes_sqlite_store_execute_prepared_statement(
            simple_notes_sqlite_controller_get_instance()->_store,
            kStatementNames[BEGIN_TRANSACTION_INDEX],
            NULL,
            0,
            NULL
	    );*/
}

void simple_notes_sqlite_controller_commit_transaction (void) {
  /*simple_notes_sqlite_store_execute_prepared_statement(
            simple_notes_sqlite_controller_get_instance()->_store,
            kStatementNames[COMMIT_TRANSACTION_INDEX],
            NULL,
            0,
            NULL
	    );*/
}

static SimpleNotesSQLiteController *simple_notes_sqlite_controller_new (void) {
    return g_object_new(SIMPLE_NOTES_TYPE_SQLITE_CONTROLLER, NULL);
}

/*static GList *simple_notes_sqlite_controller_copy_entities (
        SimpleNotesObject * (^create_entity)(void),
        void (^process_new_entity)(SimpleNotesObject *newEntity, sqlite3_stmt *statement),
        gboolean (^execute_prepared_statement) (SimpleNotesSQLiteStore *object, void (^callback) (sqlite3_stmt *)),
        gint selectedColumnIndex
) {
    SimpleNotesSQLiteController *object = simple_notes_sqlite_controller_get_instance();
    __block GList *entities = NULL;
    void (^callback)(sqlite3_stmt *statement) = ^(sqlite3_stmt *statement) {
        SimpleNotesObject *newEntity = create_entity();
        gint64 identifier = sqlite3_column_int64(statement, 0);
        simple_notes_sqlite_controller_check_identifier(object, identifier);
        simple_notes_object_assign_id(newEntity, (guint64)identifier);
        process_new_entity(newEntity, statement);
        gboolean selected = sqlite3_column_int(statement, selectedColumnIndex);
        simple_notes_object_assign_selected(newEntity, selected);
        entities = g_list_append(entities, newEntity);
    };
    gboolean result = execute_prepared_statement(object->_store, callback);
    g_warn_if_fail(result);
    if (!result) {
        if (entities) {
            g_list_free_full(entities, g_object_unref);
            entities = NULL;
        }
    }

    return entities;
    }*/

static GByteArray *simple_notes_sqlite_controller_create_byte_array_for_column (gint column, sqlite3_stmt *statement) {
    guchar const *buff = sqlite3_column_text(statement, column);
    gint len = sqlite3_column_bytes(statement, column);
    g_return_val_if_fail(len >= 0, NULL);
    guint size = (guint)len;
    size = size ? size + 1 : size;
    GByteArray *array = g_byte_array_new();
    g_byte_array_append(array, buff, size);
    return array;
}

static void simple_notes_sqlite_controller_delete_entity (SimpleNotesObject *entity, glong statementIndex) {
    g_return_if_fail(entity);
    SimpleNotesSQLiteController *object = simple_notes_sqlite_controller_get_instance();
    glong parametersCount = 1;
    gchar id[kLongLongSymbols];
    simple_notes_sqlite_controller_print_object_id(entity, id);

    gboolean result = FALSE/*simple_notes_sqlite_store_execute_prepared_statement(
            object->_store,
            kStatementNames[statementIndex],
            NULL,
            parametersCount,
            id,
            NULL
	    )*/;
    g_return_if_fail(result);
}

static void simple_notes_sqlite_controller_print_identifier (gchar *buff, guint64 identifier) {
    simple_notes_print_guint64_value(buff, identifier);
}

static void simple_notes_sqlite_controller_print_object_id (SimpleNotesObject *object, gchar *buff) {
    simple_notes_sqlite_controller_print_identifier(buff, simple_notes_object_get_id(object));
}

static void simple_notes_sqlite_controller_print_note_folder_id (SimpleNotesNote *object, gchar *buff) {
    simple_notes_sqlite_controller_print_identifier(buff, simple_notes_note_get_folder_id(object));
}

static void simple_notes_sqlite_controller_print_unix_time (gchar *buff, time_t time) {
    simple_notes_print_long_value(buff, time);
}

static void simple_notes_sqlite_controller_print_note_date (SimpleNotesNote *object, gchar *buff) {
    GDateTime *date = simple_notes_note_get_copy_last_edited(object);
    simple_notes_sqlite_controller_print_unix_time(buff, date ? g_date_time_to_unix(date) : 0);
    if (date) {
        g_date_time_unref(date);
    }
}

static void simple_notes_sqlite_controller_print_object_selected (SimpleNotesObject *object, gchar *buff) {
    simple_notes_print_boolean_value(buff, simple_notes_object_get_selected(object));
}

static void simple_notes_sqlite_controller_print_folder_count (SimpleNotesFolder *folder, gchar *buff) {
    simple_notes_print_long_value(buff, simple_notes_folder_get_count(folder));
}

static GString **simple_notes_sqlite_controller_create_statements (SimpleNotesSQLiteController *object) {
    gchar *formats[NUMBER_OF_INDEXES] = {
            "CREATE TABLE IF NOT EXISTS %s (%s INTEGER PRIMARY KEY AUTOINCREMENT, %s TEXT, %s INT, %s INT)\0",
            "SELECT * FROM %s\0",
            "UPDATE %s SET %s = $TITLE, %s = $COUNT, %s = $SELECTED WHERE %s == $ID\0",
            "DELETE FROM %s WHERE %s == $ID\0",
            "INSERT INTO %s (%s, %s, %s) VALUES ($TITLE, $COUNT, $SELECTED)\0",
            "CREATE TABLE IF NOT EXISTS %s (%s INTEGER PRIMARY KEY AUTOINCREMENT, %s INT REFERENCES %s (%s) ON DELETE CASCADE, %s TEXT, %s INT, %s INT)\0",
            "SELECT * FROM %s WHERE %s == $FOLDER_ID\0",
            "UPDATE %s SET %s = $FOLDER_ID, %s = $CONTENT, %s = $LAST_EDITED, %s = $SELECTED WHERE %s == $NOTE_ID\0",
            "DELETE FROM %s WHERE %s == $NOTE_ID\0",
            "INSERT INTO %s (%s, %s, %s, %s) VALUES ($FOLDER_ID, $CONTENT, $LAST_EDITED, $SELECTED)\0",
            "BEGIN TRANSACTION\0",
            "COMMIT TRANSACTION\0"
    };
    GString **statements = g_malloc(sizeof(GString *) * NUMBER_OF_INDEXES);
    for (glong i = 0; i < NUMBER_OF_INDEXES; i++) {
        statements[i] = g_string_sized_new(100);
    }
    g_string_printf(
            statements[CREATE_FOLDERS_TABLE_INDEX],
            formats[CREATE_FOLDERS_TABLE_INDEX],
            kFoldersKey,
            kIDKey,
            kTitleKey,
            kCountKey,
            kSelectedKey
    );
    g_string_printf(statements[SELECT_FOLDERS_INDEX], formats[SELECT_FOLDERS_INDEX], kFoldersKey);
    g_string_printf(
            statements[UPDATE_FOLDER_INDEX],
            formats[UPDATE_FOLDER_INDEX],
            kFoldersKey,
            kTitleKey,
            kCountKey,
            kSelectedKey,
            kIDKey
    );
    g_string_printf(statements[DELETE_FOLDER_INDEX], formats[DELETE_FOLDER_INDEX], kFoldersKey, kIDKey);
    g_string_printf(statements[INSERT_FOLDER_INDEX], formats[INSERT_FOLDER_INDEX], kFoldersKey, kTitleKey, kCountKey, kSelectedKey);

    g_string_printf(
            statements[CREATE_NOTES_TABLE_INDEX],
            formats[CREATE_NOTES_TABLE_INDEX],
            kNotesKey,
            kIDKey,
            kFolderIDKey,
            kFoldersKey,
            kIDKey,
            kContentKey,
            kLastEditedKey,
            kSelectedKey
    );
    g_string_printf(statements[SELECT_NOTES_INDEX], formats[SELECT_NOTES_INDEX], kNotesKey, kFolderIDKey);
    g_string_printf(
            statements[UPDATE_NOTE_INDEX],
            formats[UPDATE_NOTE_INDEX],
            kNotesKey,
            kFolderIDKey,
            kContentKey,
            kLastEditedKey,
            kSelectedKey,
            kIDKey
    );
    g_string_printf(statements[DELETE_NOTE_INDEX], formats[DELETE_NOTE_INDEX], kNotesKey, kIDKey);
    g_string_printf(statements[INSERT_NOTE_INDEX], formats[INSERT_NOTE_INDEX], kNotesKey, kFolderIDKey, kContentKey, kLastEditedKey, kSelectedKey);

    g_string_printf(statements[BEGIN_TRANSACTION_INDEX], formats[BEGIN_TRANSACTION_INDEX], NULL);
    g_string_printf(statements[COMMIT_TRANSACTION_INDEX], formats[COMMIT_TRANSACTION_INDEX], NULL);

    return statements;
}

static void simple_notes_sqlite_controller_delete_statements (GString **statements) {
    g_return_if_fail(statements);
    for (glong i = 0; i < NUMBER_OF_INDEXES; i++) {
        if (statements[i]) {
            g_string_free(statements[i], TRUE);
        }
    }
    g_free(statements);
}

static void simple_notes_sqlite_controller_check_identifier (SimpleNotesSQLiteController *object, gint64 identifier) {
    gboolean identifierCheck = identifier > 0;
    g_warn_if_fail(identifierCheck);
}
