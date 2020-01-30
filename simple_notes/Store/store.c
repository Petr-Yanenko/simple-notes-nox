//
// store.c
//

#include "store.h"
#include "statement_store.h"
#include "folder_iterator.h"


struct _SNStore {
  GObject parent;

  SNStatementStore *_statements;
};


G_DEFINE_TYPE(SNStore, sn_store, G_TYPE_OBJECT)


static gchar *const kSelectFoldersKey = "select-folders";
static gchar *const kUpdateFolderKey = "update-folder";
static gchar *const kInsertFolderKey = "insert-folder";
static gchar *const kDeleteFolderKey = "delete-folder";


static gboolean
sn_store_execute_statement(SNStore *self, gchar *key, glong count, ...);


static void
sn_store_dispose(GObject *self)
{
  SNStore *store = SN_STORE(self);
  g_clear_object(&store->_statements);

  G_OBJECT_CLASS(sn_store_parent_class)->dispose(self);
}

static void
sn_store_class_init(SNStoreClass *class)
{
  GObjectClass *gClass = G_OBJECT_CLASS(class);
  gClass->dispose = sn_store_dispose;
}

static void
sn_store_init(SNStore *self)
{
  self->_statements = sn_statement_store_new();
}

SNStore *
sn_store_get_instance(void)
{
  static SNStore *instance = NULL;

  if (!instance)
    {
      instance = g_object_new(SN_TYPE_STORE, NULL);
    }

  return instance;
}

SNFolderIterator *
sn_store_create_folder_iterator(SNStore *self)
{
  SN_RETURN_VAL_IF_FAIL(self->_statements, NULL, NULL);

  sqlite3_stmt *stmt = sn_statement_store_bind(self->_statements, kSelectFoldersKey, 0, NULL);
  SNFolderIterator *folders = sn_folder_iterator_new(stmt);

  return folders;
}

gboolean
sn_store_update_folder(
                       SNStore *self,
                       guint64 id,
                       gboolean selected,
                       glong count,
                       guint8 *title
                       )
{
  gchar countString[kLongLongSymbols];
  sn_print_long_value(countString, count);

  gchar selectedString[kSelectedSymbols];
  sn_print_boolean_value(selectedString, selected);

  gchar idString[kLongLongSymbols];
  sn_print_guint64_value(idString, id);

  glong paramCount = 4;

  return sn_store_execute_statement(
                                    self,
                                    kUpdateFolderKey,
                                    paramCount,
                                    title,
                                    countString,
                                    selectedSting,
                                    idString,
                                    NULL
                                    );
}

gboolean
sn_store_insert_folder(SNStore *self, guint8 *title)
{
  glong paramCount = 3;

  return sn_store_execute_statement(
                                    self,
                                    kInsertFolderKey,
                                    paramCount,
                                    title,
                                    "0",
                                    "0",
                                    NULL
                                    );
}

gboolean
sn_store_delete_folder(SNStore *self, guint64 id)
{
  glong paramCount = 1;

  gchar idString[kLongLongSymbols];
  sn_print_guint64_value(idString, id);

  return sn_store_execute_statement(self, kDeleteFolderKey, paramCount, idString, NULL);
}

static gboolean
sn_store_execute_statement(SNStore *self, ghar *key, glong count, ...)
{
  SNError *error = SNErrorStore;

  va_list args;

  SN_RETURN_VAL_IF_FAIL(self->_statements, FALSE, NULL);
  sqlite3_stmt *stmt = sn_statement_store_bind(self->_statements, key, count, args);

  SN_RETURN_VAL_IF_FAIL(stmt, FALSE, &error);
  SN_RETURN_VAL_IF_FAIL(sqlite3_step(stmt) == SQLITE_DONE, FALSE, &error);

  return TRUE;
}
