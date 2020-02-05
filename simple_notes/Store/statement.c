//
// statement.c
//

#include "statement.h"


struct _SNStatement {
  GObject _parent;

  sqlite3_stmt *_stmt;
  gint _code;
};


G_DEFINE_TYPE(SNStatement, sn_statement, G_TYPE_OBJECT)


static SNError const kError = SNErrorStatement;
  
  
static gboolean
sn_statement_perform(SNStatement *self, gint (*operation)(SNStatement *), gboolean (*check)(gint));

static gboolean
sn_statement_is_row(SNStatement *self);


static void
sn_statement_init(SNStatement *self)
{
  self->_stmt = NULL;
  self->_code = SQLITE_OK;
}

SNStatement *
sn_statement_new(sqlite3_stmt *stmt)
{
  SN_RETURN_VAL_IF_FAIL(stmt, NULL, NULL);

  SNStatement *statement = g_object_new(SN_TYPE_STATEMENT, NULL);
  statement->_stmt = stmt;

  return statement;
}

gboolean
sn_statement_is_valid(SNStatement *self)
{
  return self->_stmt;
}

void
sn_statement_invalidate(SNStatement *self)
{
  self->_stmt = NULL;
}

gboolean
sn_statement_reset(SNStatement *self)
{
  gint operation(SNStatement *self)
  {
    return sqlite3_reset(self->_stmt);
  }

  gboolean check(gint code)
  {
    return code == SQLITE_OK;
  }

  return sn_statement_perform(self, operation, check);
}

gboolean
sn_statement_step(SNStatement *self)
{
  gint operation(SNStatement *self)
  {
    return sqlite3_step(self->_stmt);
  }

  gboolean check(gint code)
  {
    return code == SQLITE_DONE || code == SQLITE_ROW;
  }

  return sn_statement_perform(self, operation, check);
}

gint
sn_statement_get_result_code(SNStatement *self)
{
  return self->_code;
}

gdouble
sn_statement_column_double(SNStatement *self, gint col)
{
  if (sn_statement_is_row(self))
    {
      return sqlite3_column_double(self->_stmt, col);
    }

  return 0;
}

gint
sn_statement_column_int(SNStatement *self, gint col)
{
  if (sn_statement_is_row(self))
    {
      return sqlite3_column_int(self->_stmt, col);
    }

  return 0;
}

gint64
sn_statement_column_int64(SNStatement *self, gint col)
{
  if (sn_statement_is_row(self))
    {
      return sqlite3_column_int64(self->_stmt, col);
    }

  return 0;
}

const guchar *
sn_statement_column_text(SNStatement *self, gint col)
{
  if (sn_statement_is_row(self))
    {
      return sqlite3_column_text(self->_stmt, col);
    }

  return NULL;
}

gint
sn_statement_column_bytes(SNStatement *self, gint col)
{
  if (sn_statement_is_row(self))
    {
      return sqlite3_column_bytes(self->_stmt, col);
    }

  return 0;
}

static gboolean
sn_statement_perform(SNStatement *self, gint (*operation)(SNStatement *), gboolean (*check)(gint))
{
  if (sn_statement_is_valid(self))
    {
      self->_code = operation(self);
      SN_RETURN_VAL_IF_FAIL(check(self->_code), FALSE, &kError);
      
      return TRUE;
    }

  return FALSE;
}

static gboolean
sn_statement_is_row(SNStatement *self)
{
  return sn_statement_is_valid(self) && self->_code == SQLITE_ROW;
}
