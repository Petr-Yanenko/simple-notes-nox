//
//  statement_store.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/26/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "statement_store.h"
#include <string.h>


struct _SNStatementStore {
    GObject parent;

    sqlite3 *_dataBase;
    GHashTable *_statements;
};


G_DEFINE_TYPE(SNStatementStore, sn_statement_store, G_TYPE_OBJECT)


static void
sn_statement_store_dispose_sqlite(SNStatementStore *self);

static gboolean
sn_statement_store_check_result(SNStatementStore *self, gint result);

static void
sn_statement_store_finalize_statement(gpointer statement);


static void
sn_statement_store_dispose(GObject *self)
{
    SNStatementStore *store = SN_STATEMENT_STORE(self);
    sn_statement_store_dispose_sqlite(store);

    G_OBJECT_CLASS(sn_statement_store_parent_class)->dispose(self);
}

static void
sn_statement_store_finalize(GObject *self)
{
    G_OBJECT_CLASS(sn_statement_store_parent_class)->finalize(self);
}

static void
sn_statement_store_class_init(SNStatementStoreClass *class)
{
    GObjectClass *gClass = G_OBJECT_CLASS(class);
    gClass->dispose = simple_notes_sqlite_store_dispose;
    gClass->finalize = simple_notes_sqlite_store_finalize;
}

static void
sn_statement_store_init(SNStatementStore *self)
{
    self->_statements = g_hash_table_new_full(
                                              g_str_hash,
                                              g_str_equal,
                                              g_free,
                                              sn_statement_store_finalize_statement
                                              );

    SNError error = SNErrorStatementStore;
    gint result = sqlite3_open(kFile, &self->_dataBase);
    SN_RETURN_IF_FAIL(sn_statement_store_check_result(self, result), &error);
}

gboolean
sn_statement_store_add(SNStatementStore *self, gchar *const key, gchar *const statement)
{
  SNError error = SNErrorStatementStore;
  SN_RETURN_VAL_IF_FAIL(key, FALSE, NULL);
  SN_RETURN_VAL_IF_FAIL(statement, FALSE, NULL);
  SN_RETURN_VAL_IF_FAIL(self->_dataBase, FALSE, NULL);
  SN_RETURN_VAL_IF_FAIL(self->_statements, FALSE, NULL);

  sqlite3_stmt *preparedStatement = NULL;
  gint statementResult = sqlite3_prepare_v2(
                                            self->_dataBase,
                                            statement,
                                            -1,
                                            &preparedStatement,
                                            NULL
                                            );

  gboolean ok = sn_statement_store_check_result(self, statementResult);
  SN_RETURN_VAL_IF_FAIL(ok, FALSE, &error);

  gchar *buff = g_malloc(sizeof(gchar) * (strlen(key) + 1));
  strcpy(buff, key);
  g_hash_table_insert(self->_statements, buff, preparedStatement);

  return TRUE;
}

sqlte3_stmt *
sn_statement_store_bind(SNStatementStore *self, gchar *const key, glong paramCount, ...)
{
  SNError error = SNErrorStatementStore;
  SN_RETURN_VAL_IF_FAIL(self->_statements, NULL, NULL);
  SN_RETURN_VAL_IF_FAIL(key, NULL, NULL);

  sqlite3_stmt *statement = g_hash_table_lookup(self->_statements, key);
  SN_RETURN_VAL_IF_FAIL(statement, NULL, &error);

  SN_RETURN_VAL_IF_FAIL(sqlite3_reset(statement) == SQLITE_OK, NULL, &error);

  gint result = SQLITE_OK;
  va_list args;
  va_start(args, paramCount);
  for (glong i = 0; i < paramCount; i++)
    {
      result = sqlite3_bind_text(statement, i + 1, va_arg(args, gchar *), -1, NULL);
      if (result != SQLITE_OK) break;
    }
  va_end(args);

  SN_RETURN_VAL_IF_FAIL(result == SQLITE_OK, NULL, &error);

  return statement;
}

SNStatementStore *
sn_statement_store_new(void)
{
  return g_object_new(SN_TYPE_STATEMENT_STORE, NULL);
}

static gboolean
sn_statement_store_check_result(SNStatementStore *self, gint result)
{
  if (result != SQLITE_OK)
    {
      sn_statement_store_dispose_sqlite(self);
      return FALSE;
    }
  return TRUE;
}

static void
sn_statement_store_finalize_statement(gpointer statement)
{
  SNError error = SNErrorStatementStore;
  SN_RETURN_IF_FAIL(statement, &error);
  SN_RETURN_IF_FAIL(sqlite3_finalize(statement) == SQLITE_OK, &error);
}

static void
sn_statement_store_dispose_sqlite(SNStatementStore *self)
{
  SNError error = SNErrorStatementStore;

  if (self->_statements)
    {
      g_hash_table_unref(self->_statements);
      self->_statements = NULL;
    }
  if (self->_dataBase)
    {
      sqlite3 *data = self->_dataBase;
      self->_dataBase = NULL;
      SN_RETURN_IF_FAIL(sqlite3_close(data) == SQLITE_OK, &error);
    }
}
