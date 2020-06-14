//
//  data_base.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/26/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "data_base.h"
#include <string.h>


struct _SNDataBase {
  GObject _parent;

  sqlite3 *_db;
  GHashTable *_stmts;
  GHashTable *_bindingStmts;
};


G_DEFINE_TYPE(SNDataBase, sn_data_base, G_TYPE_OBJECT)


static SNError const kError = SNErrorDataBase;


static sqlite3_stmt *
sn_data_base_sqlite_bind(SNDataBase *self,
			 gchar *const key,
			 glong paramCount,
			 va_list args);

static void
sn_data_base_dispose_sqlite(SNDataBase *self);

static gboolean
sn_data_base_check_result(SNDataBase *self, gint result);

static void
sn_data_base_finalize_statement(gpointer statement);

static void
sn_data_base_unref(gpointer bindingStmt);


static void
sn_data_base_dispose(GObject *self)
{
    SNDataBase *db = SN_DATA_BASE(self);
    sn_data_base_dispose_sqlite(db);

    G_OBJECT_CLASS(sn_data_base_parent_class)->dispose(self);
}

static void
sn_data_base_finalize(GObject *self)
{
    G_OBJECT_CLASS(sn_data_base_parent_class)->finalize(self);
}

static void
sn_data_base_class_init(SNDataBaseClass *class)
{
    GObjectClass *gClass = G_OBJECT_CLASS(class);
    gClass->dispose = sn_data_base_dispose;
    gClass->finalize = sn_data_base_finalize;
}

static void
sn_data_base_init(SNDataBase *self)
{
    self->_stmts = g_hash_table_new_full(g_str_hash,
					 g_str_equal,
					 g_free,
					 sn_data_base_finalize_statement);
    self->_bindingStmts = g_hash_table_new_full(g_str_hash,
						g_str_equal,
						g_free,
						sn_data_base_unref);

    gint result = sqlite3_open(kFile, &self->_db);
    SN_RETURN_IF_FAIL(sn_data_base_check_result(self, result), &kError);
}

gboolean
sn_data_base_add(SNDataBase *self, gchar *const key, gchar *const stmt)
{
  SN_RETURN_VAL_IF_FAIL(key, FALSE, NULL);
  SN_RETURN_VAL_IF_FAIL(stmt, FALSE, NULL);
  SN_RETURN_VAL_IF_FAIL(self->_db, FALSE, &kError);
  SN_RETURN_VAL_IF_FAIL(self->_stmts, FALSE, &kError);

  sqlite3_stmt *preparedStmt = NULL;
  gint stmtResult = sqlite3_prepare_v2(self->_db,
				       stmt,
				       -1,
				       &preparedStmt,
				       NULL);

  gboolean ok = sn_data_base_check_result(self, stmtResult);
  SN_RETURN_VAL_IF_FAIL(ok, FALSE, &kError);

  gchar *keyCopy = sn_copy_string(key);
  g_hash_table_insert(self->_stmts, keyCopy, preparedStmt);

  return TRUE;
}

SNStatement *
sn_data_base_bind(SNDataBase *self, gchar *const key, glong paramCount, ...)
{
  SN_RETURN_VAL_IF_FAIL(key, NULL, NULL);
  SN_RETURN_VAL_IF_FAIL(self->_bindingStmts, NULL, &kError);

  va_list args;
  va_start(args, paramCount);
  sqlite3_stmt *stmt = sn_data_base_sqlite_bind(self, key, paramCount, args);
  va_end(args);
  if (!stmt) return NULL;

  SNStatement *bindingStmt = sn_statement_new(stmt);
  gchar *keyCopy = sn_copy_string(key);
  g_hash_table_insert(self->_bindingStmts, keyCopy, bindingStmt);

  return bindingStmt;
}

gboolean
sn_data_base_execute(SNDataBase *self, gchar *const key, glong paramCount, ...)
{
  va_list args;
  va_start(args, paramCount);
  gboolean execute = sn_data_base_vexecute(self, key, paramCount, args);
  va_end(args);

  return execute;
}

gboolean
sn_data_base_vexecute(SNDataBase *self,
		      gchar *const key,
		      glong paramCount,
		      va_list args)
{
  SN_RETURN_VAL_IF_FAIL(key, FALSE, NULL);

  sqlite3_stmt *stmt = sn_data_base_sqlite_bind(self, key, paramCount, args);
  if (!stmt) return FALSE;

  SN_RETURN_VAL_IF_FAIL(sqlite3_step(stmt) == SQLITE_DONE, FALSE, &kError);

  return TRUE;
}

SNDataBase *
sn_data_base_new(void)
{
  return g_object_new(SN_TYPE_DATA_BASE, NULL);
}

static sqlite3_stmt *
sn_data_base_sqlite_bind(SNDataBase *self,
			 gchar *const key,
			 glong paramCount,
			 va_list args)
{
  SN_RETURN_VAL_IF_FAIL(self->_stmts, NULL, &kError);
  SN_RETURN_VAL_IF_FAIL(self->_bindingStmts, NULL, &kError)
  SN_RETURN_VAL_IF_FAIL(key, NULL, NULL);

  g_hash_table_remove(self->_bindingStmts, key);

  sqlite3_stmt *stmt = g_hash_table_lookup(self->_stmts, key);
  SN_RETURN_VAL_IF_FAIL(stmt, NULL, &kError);

  SN_RETURN_VAL_IF_FAIL(sqlite3_reset(stmt) == SQLITE_OK, NULL, &kError);

  gint result = SQLITE_OK;
  for (glong i = 0; i < paramCount; i++)
    {
      gchar *arg = va_arg(args, gchar *);
      result = sqlite3_bind_text(stmt, i + 1, arg, -1, NULL);
      if (result != SQLITE_OK) break;
    }

  SN_RETURN_VAL_IF_FAIL(result == SQLITE_OK, NULL, &kError);

  return stmt;
}

static gboolean
sn_data_base_check_result(SNDataBase *self, gint result)
{
  if (result != SQLITE_OK)
    {
      sn_data_base_dispose_sqlite(self);
      return FALSE;
    }
  return TRUE;
}

static void
sn_data_base_finalize_statement(gpointer statement)
{
  SN_RETURN_IF_FAIL(statement, &kError);
  SN_RETURN_IF_FAIL(sqlite3_finalize(statement) == SQLITE_OK, &kError);
}

static void
sn_data_base_unref(gpointer bindingStmt)
{
  SNStatement *stmt = (SNStatement *)bindingStmt;
  sn_statement_invalidate(stmt);
  g_object_unref(stmt);
}

static void
sn_data_base_dispose_sqlite(SNDataBase *self)
{
  if (self->_bindingStmts)
    {
      g_hash_table_unref(self->_bindingStmts);
      self->_bindingStmts = NULL;
    }
  if (self->_stmts)
    {
      g_hash_table_unref(self->_stmts);
      self->_stmts = NULL;
    }
  if (self->_db)
    {
      SN_RETURN_IF_FAIL(sqlite3_close(self->_db) == SQLITE_OK, &kError);
      self->_db = NULL;
    }
}
