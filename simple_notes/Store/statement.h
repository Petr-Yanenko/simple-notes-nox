//
// statement.h
//

#ifndef sn_statement____FILEEXTENSION___
#define sn_statement____FILEEXTENSION___

#include <sqlite3.h>
#include "simple_notes.h"

#define SN_TYPE_STATEMENT sn_statement_get_type()
G_DECLARE_FINAL_TYPE(SNStatement, sn_statement, SN, STATEMENT, GObject)

  
SNStatement *
sn_statement_new(sqlite3_stmt *stmt);


gboolean
sn_statement_is_valid(SNStatement *self);

void
sn_statement_invalidate(SNStatement *self);


gboolean
sn_statement_reset(SNStatement *self);

gboolean
sn_statement_step(SNStatement *self);

gint
sn_statement_get_result_code(SNStatement *self);


gdouble
sn_statement_column_double(SNStatement *self, gint col);

gint
sn_statement_column_int(SNStatement *self, gint col);

gint64
sn_statement_column_int64(SNStatement *self, gint col);

const guchar *
sn_statement_column_text(SNStatement *self, gint col);


gint
sn_statement_column_bytes(SNStatement *self, gint col);


#endif
