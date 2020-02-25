//
// store.c
//

#include "store.h"
#include "sql_controller.h"
#include <stdio.h>


struct _SNStore {
  GObject _parent;

  SNSQLController *_sql;

  guint64 _folderChanged;
  guint64 _folderSelected;

  guint64 _noteChanged;
  guint64 _noteSelected;
};


G_DEFINE_TYPE(SNStore, sn_store, G_TYPE_OBJECT)


enum {
      PROP_FOLDER_CHANGED = 1,
      PROP_FOLDER_SELECTED = 2,
      PROP_NOTE_CHANGED = 3,
      PROP_NOTE_SELECTED = 4,
      N_PROPERTIES
};


static GParamSpec *objProperties[N_PROPERTIES] = { NULL, };


gchar *const kFolderChanged = "folder-changed";
gchar *const kFolderSelected = "folder-selected";

gchar *const kNoteChanged = "note-changed";
gchar *const kNoteSelected = "note-selected";

guint64 const kItemInserted = 0;
guint64 const kItemDeselected = 0;
gint64 const kNotEdited = 0;

static SNError const kError = SNErrorStore;


static gint64
sn_store_get_last_edited(SNStore *self);

static gboolean
sn_store_print_note_path(SNStore *self, gchar *buff, guint64 folderID);

static SNDataIterator *
sn_store_create_iterator(SNStore *self,
			 SNStatement *stmt,
			 SNDataIterator * (*constructor)(SNStatement *));

static gboolean
sn_store_assign_folder_changed(SNStore *self, guint64 folderChanged, gboolean changed);

static gboolean
sn_store_assign_note_changed(SNStore *self,
			     guint64 noteChanged,
			     gboolean changed,
			     gboolean deselect);

static gboolean
sn_store_assign_folder_selected(SNStore *self, guint64 folderSelected, gboolean changed);

static gboolean
sn_store_assign_note_selected(SNStore *self, guint64 noteSelected, gboolean changed);


static void
sn_store_set_property(GObject *self, guint propID, const GValue *value, GParamSpec *pSpec)
{
  SNStore *store = SN_STORE(self);

  switch (propID)
    {
    case PROP_FOLDER_CHANGED:
      store->_folderChanged = g_value_get_uint64(value);
      break;

    case PROP_FOLDER_SELECTED:
      store->_folderSelected = g_value_get_uint64(value);
      break;

    case PROP_NOTE_CHANGED:
      store->_noteChanged = g_value_get_uint64(value);
      break;

    case PROP_NOTE_SELECTED:
      store->_noteSelected = g_value_get_uint64(value);
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

    case PROP_FOLDER_SELECTED:
      g_value_set_uint64(value, store->_folderSelected);
      break;

    case PROP_NOTE_CHANGED:
      g_value_set_uint64(value, store->_noteChanged);
      break;

    case PROP_NOTE_SELECTED:
      g_value_set_uint64(value, store->_noteSelected);
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
  g_clear_object(&store->_sql);

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
							   0, /* min value */
							   G_MAXUINT64,
							   0, /* default value */
							   G_PARAM_READWRITE);
  objProperties[PROP_FOLDER_SELECTED] = g_param_spec_uint64(kFolderSelected,
							    "Selected folder id",
							    "This is set to select folder",
							    0,
							    G_MAXUINT64,
							    0,
							    G_PARAM_READWRITE);
  objProperties[PROP_NOTE_CHANGED] = g_param_spec_uint64(kNoteChanged,
							 "Changed note id",
							 "This is set when note is changed",
							 0,
							 G_MAXUINT64,
							 0,
							 G_PARAM_READWRITE);
  objProperties[PROP_NOTE_SELECTED] = g_param_spec_uint64(kNoteSelected,
							  "Selected note id",
							  "This is set to select note",
							  0,
							  G_MAXUINT64,
							  0,
							  G_PARAM_READWRITE);
  g_object_class_install_properties(gClass, N_PROPERTIES, objProperties);
}

static void
sn_store_init(SNStore *self)
{
  self->_folderChanged = 0;
  self->_folderSelected = 0;
  self->_noteChanged = 0;
  self->_noteSelected = 0;
  self->_sql = sn_sql_controller_new();
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
  SNStatement *stmt = sn_sql_controller_select_folders(self->_sql);

  SNDataIterator *constructor(SNStatement *stmt)
  {
    return SN_DATA_ITERATOR(sn_folder_iterator_new(stmt));
  }
  
  return SN_FOLDER_ITERATOR(sn_store_create_iterator(self, stmt, constructor));
}

gboolean
sn_store_update_folder(SNStore *self, guint64 id, gchar *title)
{
  gboolean success = sn_sql_controller_update_folder_title(self->_sql, id, title);
  return sn_store_assign_folder_changed(self, id, success);
}

gboolean
sn_store_insert_folder(SNStore *self, gchar *title)
{
  gboolean success = sn_sql_controller_insert_folder(self->_sql, title, FALSE);
  return sn_store_assign_folder_changed(self, kItemInserted, success);
}

gboolean
sn_store_delete_folder(SNStore *self, guint64 id)
{
  gboolean success = sn_sql_controller_delete_folder(self->_sql, id);
  if (self->_folderSelected == id)
    {
      sn_store_assign_folder_selected(self, kItemDeselected, success);
    }
  return sn_store_assign_folder_changed(self, id, success);
}

gboolean
sn_store_select_folder(SNStore *self, guint64 id)
{
  gboolean success = sn_sql_controller_update_folder_selected(self->_sql, id, TRUE);
  return sn_store_assign_folder_selected(self, id, success);
}

guint64
sn_store_get_folder_changed(SNStore *self)
{
  guint64 value;
  g_object_get(G_OBJECT(self), kFolderChanged, &value, NULL);

  return value;
}

guint64
sn_store_get_folder_selected(SNStore *self)
{
  guint64 value;
  g_object_get(G_OBJECT(self), kFolderSelected, &value, NULL);

  return value;
}

SNNoteIterator *
sn_store_create_note_iterator(SNStore *self)
{
  if (!self->_folderSelected) return NULL;
  SNStatement *stmt = sn_sql_controller_select_notes(self->_sql, self->_folderSelected);

  SNDataIterator *constructor(SNStatement *stmt)
  {
    return SN_DATA_ITERATOR(sn_note_iterator_new(stmt));
  }

  return SN_NOTE_ITERATOR(sn_store_create_iterator(self, stmt, constructor));
}

gboolean
sn_store_update_note(SNStore *self, guint64 id, guint64 folderID)
{
  gboolean success = sn_sql_controller_update_note_folder_id(self->_sql, id, folderID);
  return sn_store_assign_note_changed(self, id, success, TRUE);
}

gboolean
sn_store_update_note_last_edited(SNStore *self, guint64 id)
{
  gint64 lastEdited = sn_store_get_last_edited(self);
  gboolean success = sn_sql_controller_update_note_last_edited(self->_sql, id, lastEdited);

  return sn_store_assign_note_changed(self, id, success, FALSE);
}

gboolean
sn_store_insert_note(SNStore *self)
{
  if (self->_folderSelected)
    {
      gint64 lastEdited = kNotEdited;
      gchar content[1000];

      if (sn_store_print_note_path(self, content, self->_folderSelected))
	{
	  gboolean insert = sn_sql_controller_insert_note(self->_sql,
							  self->_folderSelected,
							  content,
							  lastEdited,
							  FALSE);
	  return sn_store_assign_note_changed(self, kItemInserted, insert, FALSE);
	}
    }
  
  return FALSE;
}

gboolean
sn_store_delete_note(SNStore *self, guint64 id)
{
  gboolean success = sn_sql_controller_delete_note(self->_sql, id, self->_folderSelected);

  return sn_store_assign_note_changed(self, id, success, TRUE);
}

gboolean
sn_store_select_note(SNStore *self, guint64 id)
{
  gboolean success = sn_sql_controller_update_note_selected(self->_sql,
							    id,
							    self->_folderSelected,
							    TRUE);

  return sn_store_assign_note_selected(self, id, success);
}

guint64
sn_store_get_note_changed(SNStore *self)
{
  guint64 value;
  g_object_get(G_OBJECT(self), kNoteChanged, &value, NULL);

  return value;
}

guint64
sn_store_get_note_selected(SNStore *self)
{
  guint64 value;
  g_object_get(G_OBJECT(self), kNoteSelected, &value, NULL);

  return value;
}

static gint64
sn_store_get_last_edited(SNStore *self)
{
  GDateTime *lastEdited = g_date_time_new_now_local();
  gint64 unixTime = g_date_time_to_unix(lastEdited);
  g_date_time_unref(lastEdited);

  return unixTime;
}

static gboolean
sn_store_print_note_path(SNStore *self, gchar *buff, guint64 folderID)
{
  static gboolean seeded = FALSE;
  if (!seeded)
    {
      srand(time(0));
      seeded = TRUE;
    }

  gboolean success = FALSE;
  for (glong i = 0; !success, i < RAND_MAX; i++)
    {
      glong id = rand();
      sprintf(buff, kNotePathFormat, folderID, id);
      GFile *file = g_file_new_for_path(buff);
      GError *error = NULL;
      GFileOutputStream *fileStream = g_file_create(file,
						    G_FILE_CREATE_NONE,
						    NULL,
						    &error);
      if (fileStream)
	{
	  success = TRUE;
	}

      g_object_unref(file);
      g_object_unref(fileStream);
    }

  if (success) return TRUE;
  
  buff[0] = '\0';
  SN_RETURN_VAL_IF_FAIL(success, FALSE, &kError);
}

static SNDataIterator *
sn_store_create_iterator(SNStore *self,
			 SNStatement *stmt,
			 SNDataIterator * (*constructor)(SNStatement *))
{
  SNDataIterator *iterator = NULL;
  if (stmt)
    {
      iterator = constructor(stmt);
    }

  return iterator;
}

static gboolean
sn_store_assign_property(SNStore *self, gchar *prop, guint64 value, gboolean changed)
{
  if (changed)
    {
      g_object_set(G_OBJECT(self), prop, value, NULL);
    }

  return changed;
}

static gboolean
sn_store_assign_folder_changed(SNStore *self, guint64 folderChanged, gboolean changed)
{
  return sn_store_assign_property(self, kFolderChanged, folderChanged, changed);
}

static gboolean
sn_store_assign_folder_selected(SNStore *self, guint64 folderSelected, gboolean changed)
{
  return sn_store_assign_property(self, kFolderSelected, folderSelected, changed);
}

static gboolean
sn_store_assign_note_changed(SNStore *self,
			     guint64 noteChanged,
			     gboolean changed,
			     gboolean deselect)
{
  if (deselect && noteChanged && self->_noteSelected == noteChanged)
    {
      sn_store_assign_note_selected(self, kItemDeselected, changed);
    }
  return sn_store_assign_property(self, kNoteChanged, noteChanged, changed);
}

static gboolean
sn_store_assign_note_selected(SNStore *self, guint64 noteSelected, gboolean changed)
{
  return sn_store_assign_property(self, kNoteSelected, noteSelected, changed);
}
