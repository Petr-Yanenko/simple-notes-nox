//
// store.c
//

#include "store.h"
#include "data_base.h"
#include "folder_iterator.h"


struct _SNStore {
  GObject _parent;

  SNDataBase *_db;
};


G_DEFINE_TYPE(SNStore, sn_store, G_TYPE_OBJECT)


enum {
      CREATE_FOLDERS_INDEX = 0,
      SELECT_FOLDERS_INDEX,
      UPDATE_FOLDER_INDEX,
      INSERT_FOLDER_INDEX,
      DELETE_FOLDER_INDEX,
      CREATE_NOTES_INDEX,
      SELECT_NOTES_INDEX,
      UPDATE_NOTE_INDEX,
      INSERT_NOTE_INDEX,
      DELETE_NOTE_INDEX,
      BEGIN_TRANSACTION_INDEX,
      COMMIT_TRANSACTION_INDEX,
      INDEXES_NUMBER
};

static gchar *const kStmtKeys[] = {
				   "create_folders",
				   "select-folders",
				   "update-folder",
				   "insert-folder",
				   "delete-folder",
				   "create-notes",
				   "select-notes",
				   "update-note",
				   "insert-note",
				   "delete-note",
				   "begin-transaction",
				   "commit-transaction"
};

static SNError const kError = SNErrorStore;


static void
sn_store_dispose(GObject *self)
{
  SNStore *store = SN_STORE(self);
  g_clear_object(&store->_db);

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
  gchar *stmts[] = {
		    "CREATE TABLE IF NOT EXIST folders (id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT, count TEXT, selected INT)",
		    "SELECT * FROM folders",
		    "UPDATE folders SET title = $TITLE, count = $COUNT, selected = $SELECTED WHERE id == $ID",
		    "INSERT INTO folders (title, count, selected) VALUES ($TITLE, $COUNT, $SELECTED)"
		    "DELETE FROM folders WHERE id == $ID",
		    "CREATE TABLE IF NOT EXIST notes (id INTEGER PRIMARY KEY AUTOINCREMENT, folder_id INT REFERENCES folders (id) ON DELETE CASCADE, content TEXT, last_edited, INT, selected INT)",
		    "UPDATE notes SET folder_id = $FOLDER_ID, content = $CONTENT, last_edited = $LAST_EDITED, selected = $SELECTED WHERE id == $NOTE_ID"
		    "INSERT INTO notes (folder_id, content, last_edited, selected) VALUES ($FOLDER_ID, $CONTENT, $LAST_EDITED, $SELECTED)"
		    "DELETE FROM notes WHERE id == $NOTE_ID"
		    "BEGIN TRANSACTION",
		    "COMMIT TRANSACTION"
  };
  
  self->_db = sn_data_base_new();

  for (glong i = 0; i < INDEXES_NUMBER; i++)
    {
      gboolean success = sn_data_base_add(self->_db, kStmtKeys[i], stmts[i]);
      SN_RETURN_IF_FAIL(success, &kError);
    }

  gboolean folders = sn_data_base_execute(self->_db, kStmtKeys[CREATE_FOLDERS], 0, NULL);
  gboolean notes = sn_data_base_execute(self->_db, kStmtKeys[CREATE_NOTES], 0, NULL);
  SN_RETURN_IF_FAIL(folders && notes, &kError);
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
  SNStatement *stmt = sn_data_base_bind(self->_db, kStmtKeys[SELECT_FOLDERS_INDEX], 0, NULL);
  SN_RETURN_VAL_IF_FAIL(stmt, NULL, &kError);
  
  SNFolderIterator *folders = sn_folder_iterator_new(stmt);

  return folders;
}

gboolean
sn_store_update_folder(SNStore *self,
                       guint64 id,
                       gboolean selected,
                       glong count,
                       guint8 *title)
{
  gchar countString[kLongLongSymbols];
  sn_print_long_value(countString, count);

  gchar selectedString[kSelectedSymbols];
  sn_print_boolean_value(selectedString, selected);

  gchar idString[kLongLongSymbols];
  sn_print_guint64_value(idString, id);

  glong paramCount = 4;

  return sn_data_base_execute(self->_db,
			      kStmtKeys[UPDATE_FOLDER_INDEX],
			      paramCount,
			      title,
			      countString,
			      selectedSting,
			      idString,
			      NULL);
}

gboolean
sn_store_insert_folder(SNStore *self, guint8 *title)
{
  glong paramCount = 3;

  return sn_data_base_execute(self->_db,
			      kStmtKeys[INSERT_FOLDER_INDEX],
			      paramCount,
			      title,
			      "0",
			      "0",
			      NULL);
}

gboolean
sn_store_delete_folder(SNStore *self, guint64 id)
{
  glong paramCount = 1;

  gchar idString[kLongLongSymbols];
  sn_print_guint64_value(idString, id);

  return sn_data_base_execute(self->_db, kStmtKeys[DELETE_FOLDER_INDEX], paramCount, idString, NULL);
}
