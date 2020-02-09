//
// store.c
//

#include "store.h"
#include "data_base.h"
#include "folder_iterator.h"


struct _SNStore {
  GObject _parent;

  SNDataBase *_db;

  guint64 _folderChanged;
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


enum {
      PROP_FOLDER_CHANGED = 1,
      N_PROPERTIES
};


static GParamSpec *objProperties[N_PROPERTIES] = { NULL, };


gchar *const kFolderChanged = "folder-changed";
guint64 const kFolderInserted = 0;

static SNError const kError = SNErrorStore;


static void
sn_store_assign_folder_changed(SNStore *self, guint64 folderChanged);

static gboolean
sn_store_execute(SNStore *self, guint64 changedFolder, glong stmtIndex, glong paramCount, ...);


static void
sn_store_set_property(GObject *self, guint propID, const GValue *value, GParamSpec *pSpec)
{
  SNStore *store = SN_STORE(self);

  switch (propID)
    {
    case PROP_FOLDER_CHANGED:
      store->_folderChanged = g_value_get_uint64(value);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID(self, propID, pSpec);
      break;
    }
}

static void
sn_store_get_property(GObject *self, guint propID, GValue *value, GParamSpec *pSpec)
{
  SNStore *store = SN_STORE(self);

  switch (propID)
    {
    case PROP_FOLDER_CHANGED:
      g_value_set_uint64(value, store->_folderChanged);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID(self, propID, pSpec);
      break;
    }
}

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
  gClass->set_property = sn_store_set_property;
  gClass->get_property = sn_store_get_property;

  objProperties[PROP_FOLDER_CHANGED] = g_param_spec_uint64(kFolderChanged,
							   "Changed folder id",
							   "This is set when folder is changed",
							   0, /* default value */
							   G_PARAM_READWRITE);
  g_object_class_install_properties(gClass, N_PROPERTIES, objProperties);
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

  self->_folderChanged = 0;
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
                       gchar *title)
{
  gchar countString[kLongLongSymbols];
  sn_print_long_value(countString, count);

  gchar selectedString[kSelectedSymbols];
  sn_print_boolean_value(selectedString, selected);

  gchar idString[kLongLongSymbols];
  sn_print_guint64_value(idString, id);

  glong paramCount = 4;

  return sn_store_execute(self,
			  id,
			  UPDATE_FOLDER_INDEX,
			  paramCount,
			  title,
			  countString,
			  selectedSting,
			  idString,
			  NULL);
}

gboolean
sn_store_insert_folder(SNStore *self, gchar *title)
{
  glong paramCount = 3;

  return sn_store_execute(self,
			  kFolderInserted,
			  INSERT_FOLDER_INDEX,
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

  return sn_store_execute(self, id, DELETE_FOLDER_INDEX, paramCount, idString, NULL);
}

guintt64
sn_store_get_folder_changed(SNStore *self)
{
  guint64 value;
  g_object_get(G_OBJECT(self), kFolderChanged, &value, NULL);

  return value;
}

static void
sn_store_assign_folder_changed(SNStore *self, guint64 folderChanged)
{
  g_object_set(G_OBJECT(self), kFolderChanged, folderChanged, NULL);
}

static gboolean
sn_store_execute(SNStore *self, guint64 changedFolder,  glong stmtIndex, glong count, ...)
{
  va_list args;
  gbooelan success = sn_data_base_execute(self->_db
					  kStmtKeys[stmtIndex],
					  count,
					  args);
  if (success && stmtIndex >= UPDATE_FOLDER_INDEX && stmtIndex <= DELETE_FOLDER_INDEX)
    {
      sn_store_assign_folder_changed(self, changedFolder);
    }

  return success;
}
