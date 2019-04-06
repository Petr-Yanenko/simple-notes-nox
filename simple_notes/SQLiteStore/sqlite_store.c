//
//  sqlite_store.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/26/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "sqlite_store.h"
#include <string.h>

struct _SimpleNotesSQLiteStore {
    GObject parent;

    sqlite3 *_dataBase;
    GHashTable *_statements;
};

static void simple_notes_sqlite_store_dispose_sqlite (SimpleNotesSQLiteStore *object);
static gboolean simple_notes_sqlite_store_check_result (SimpleNotesSQLiteStore *object, gint result);
static void simple_notes_sqlite_store_finalize_statement (gpointer statement);
static gboolean simple_notes_sqlite_store_execute (
        sqlite3_stmt *statement,
        gchar *parameters[],
        glong count,
        void (^callback)(sqlite3_stmt *stmt)
);

G_DEFINE_TYPE (SimpleNotesSQLiteStore, simple_notes_sqlite_store, G_TYPE_OBJECT)

static void simple_notes_sqlite_store_dispose (GObject *object) {
    SimpleNotesSQLiteStore *manager = SIMPLE_NOTES_SQLITE_STORE(object);
    simple_notes_sqlite_store_dispose_sqlite(manager);
    G_OBJECT_CLASS(simple_notes_sqlite_store_parent_class)->dispose(object);
}

static void simple_notes_sqlite_store_finalize (GObject *object) {

    G_OBJECT_CLASS(simple_notes_sqlite_store_parent_class)->finalize(object);
}

static void simple_notes_sqlite_store_class_init (SimpleNotesSQLiteStoreClass *classObject) {
    GObjectClass *gClass = G_OBJECT_CLASS(classObject);
    gClass->dispose = simple_notes_sqlite_store_dispose;
    gClass->finalize = simple_notes_sqlite_store_finalize;
}

static void simple_notes_sqlite_store_init (SimpleNotesSQLiteStore *object) {
    object->_statements = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, simple_notes_sqlite_store_finalize_statement);
    gint result = sqlite3_open(kFile, &object->_dataBase);
    g_return_if_fail(simple_notes_sqlite_store_check_result(object, result));
}

gboolean simple_notes_sqlite_store_register_prepared_statement (
        SimpleNotesSQLiteStore *object,
        gchar *const key,
        gchar *const statement
) {
    g_return_val_if_fail(object->_dataBase, FALSE);
    g_return_val_if_fail(key, FALSE);
    g_return_val_if_fail(statement, FALSE);

    sqlite3_stmt *preparedStatement = NULL;
    gint statementResult = sqlite3_prepare_v2(object->_dataBase, statement, -1, &preparedStatement, NULL);

    gboolean ok = simple_notes_sqlite_store_check_result(object, statementResult);
    if (!ok) {
        if (preparedStatement) {
            sqlite3_finalize(preparedStatement);
        }
        g_return_val_if_reached(FALSE);
    }
    gchar *buff = g_malloc(sizeof(gchar) * (strlen(key) + 1));
    strcpy(buff, key);
    g_hash_table_insert(object->_statements, buff, preparedStatement);

    return TRUE;
}

gboolean simple_notes_sqlite_store_execute_prepared_statement (
        SimpleNotesSQLiteStore *object,
        gchar *const name,
        void (^callback) (sqlite3_stmt *),
        glong paramCount,
        ...
) {
    g_return_val_if_fail(object->_statements, FALSE);
    g_return_val_if_fail(name, FALSE);

    SimpleNotesSQLiteStore *store = object;
    sqlite3_stmt *statement = g_hash_table_lookup(store->_statements, name);
    gchar *parameters[paramCount];
    va_list a_list;
    va_start(a_list, paramCount);
    for (glong i = 0; i < paramCount; i++) {
        parameters[i] = va_arg(a_list, gchar *);
    }
    va_end(a_list);
    return simple_notes_sqlite_store_execute(statement, parameters, paramCount, callback);
}

SimpleNotesSQLiteStore *simple_notes_sqlite_store_new (void) {
    return g_object_new(SIMPLE_NOTES_TYPE_SQLITE_STORE, NULL);
}

static gboolean simple_notes_sqlite_store_check_result (SimpleNotesSQLiteStore *object, gint result) {
    if (result != SQLITE_OK) {
        simple_notes_sqlite_store_dispose_sqlite(object);
        return FALSE;
    }
    return TRUE;
}

static void simple_notes_sqlite_store_finalize_statement (gpointer statement) {
    g_warn_if_fail(statement);
    if (statement) {
        g_warn_if_fail(sqlite3_finalize(statement) == SQLITE_OK);
    }
}

static void simple_notes_sqlite_store_dispose_sqlite (SimpleNotesSQLiteStore *object) {
    if (object->_statements) {
        g_hash_table_unref(object->_statements);
        object->_statements = NULL;
    }
    if (object->_dataBase) {
        g_warn_if_fail(sqlite3_close(object->_dataBase) == SQLITE_OK);
        object->_dataBase = NULL;
    }
}

static gboolean simple_notes_sqlite_store_execute (
        sqlite3_stmt *statement,
        gchar *parameters[],
        glong count,
        void (^callback)(sqlite3_stmt *stmt)
) {
    g_return_val_if_fail(statement, FALSE);

    g_warn_if_fail(sqlite3_reset(statement) == SQLITE_OK);
    for (gint i = 0; i < count; i++) {
        g_return_val_if_fail(sqlite3_bind_text(statement, i + 1, parameters[i], -1, NULL) == SQLITE_OK, FALSE);
    }
    int result;
    while ((result = sqlite3_step(statement)) == SQLITE_ROW) {
        g_return_val_if_fail(callback, FALSE);
        callback(statement);
    }
    g_return_val_if_fail(result == SQLITE_DONE, FALSE);
    return TRUE;
}
