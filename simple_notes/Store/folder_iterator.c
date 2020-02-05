//
// folder_iterator.c
//

#include "folder_iterator.h"
#include "data_iterator_private.h"


struct _SNFolderIterator {
  SNDataIterator _parent;
};


G_DEFINE_TYPE(SNFolderIterator, sn_folder_iterator, SN_TYPE_DATA_ITERATOR)


static SNError kError = SNErrorFolderIterator;


#define GET_STATEMENT(self, stmtPointer)		\
  {\
    SN_RETURN_VAL_IF_FAIL(stmtPointer, NULL, &kError);\
    SNStatement *stmt = sn_data_iterator_private_get_statement(SN_DATA_ITERATOR(self));\
    SN_RETURN_VAL_IF_FAIL(sn_statement_is_valid(stmt), NULL, &kError);\
    *stmtPointer = stmt;\
  }


static void
sn_folder_iterator_init(SNFolderIterator *self)
{
}

SNFolderIterator *
sn_folder_iterator_new(SNStatement *stmt)
{
  SNFolderIterator *instance = g_object_new(SN_TYPE_FOLDER_ITERATOR, NULL);
  sn_data_iterator_private_set_statement(SN_DATA_ITERATOR(instance), stmt);
  
  return instance;
}

guint64
sn_folder_iterator_item_id(SNFolderIterator *self)
{
  glong idColumn = 0;
  
  SNStatement *stmt = NULL;
  GET_STATEMENT(self, &stmt);
  
  return (guint64)sn_statement_column_int64(stmt, idColumn);
}

gchar *
sn_folder_iterator_item_title(SNFolderIterator *self)
{
  glong titleColumn = 1;

  SNStatement *stmt = NULL;
  GET_STATEMENT(self, &stmt);

  return (gchar *)sn_statement_column_text(stmt, titleColumn);
}

glong
sn_folder_iterator_item_count(SNFolderIterator *self)
{
  glong countColumn = 2;

  SNStatement *stmt = NULL;
  GET_STATEMENT(self, &stmt);

  return sn_statement_column_int(stmt, countColumn);
}

gboolean
sn_folder_iterator_item_selected(SNFolderIterator *self)
{
  glong selectedColumn = 3;

  SNStatement *stmt = NULL;
  GET_STATEMENT(self, &stmt);

  return sn_statement_column_int(stmt, selectedColumn);
}
