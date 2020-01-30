//
//  statement_store.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/26/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_statement_store____FILEEXTENSION___
#define simple_notes_statement_store____FILEEXTENSION___

#include <sqlite3.h>
#include "simple_notes.h"

#define SN_TYPE_STATEMENT_STORE  sn_statement_store_get_type()
G_DECLARE_FINAL_TYPE(SNStatementStore, sn_statement_store, SN, STATEMENT_STORE, GObject)

SNStatementStore *
sn_statement_store_new(void);

gboolean
sn_statement_store_add(SNStatementStore *self, gchar *const key, gchar *const statement);

sqlite3_stmt *
sn_statement_store_bind(SNStatementStore *self, gchar *const key, glong paramCount, ...);

#endif
