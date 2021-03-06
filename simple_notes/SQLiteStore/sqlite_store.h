//
//  sqlite_store.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/26/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_sqlite_store____FILEEXTENSION___
#define simple_notes_sqlite_store____FILEEXTENSION___

#include <sqlite3.h>
#include "simple_notes.h"

#define SIMPLE_NOTES_TYPE_SQLITE_STORE  simple_notes_sqlite_store_get_type ()
G_DECLARE_FINAL_TYPE (SimpleNotesSQLiteStore, simple_notes_sqlite_store, SIMPLE_NOTES, SQLITE_STORE, GObject)

SimpleNotesSQLiteStore *simple_notes_sqlite_store_new (void);
gboolean simple_notes_sqlite_store_register_prepared_statement (
        SimpleNotesSQLiteStore *object,
        gchar *const key,
        gchar *const statement
);
gboolean simple_notes_sqlite_store_execute_prepared_statement (
        SimpleNotesSQLiteStore *object,
        gchar *const name,
        void (callback) (sqlite3_stmt *),
        glong paramCount,
        ...
                                                               );

#endif
