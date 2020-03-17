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

static gchar *const kNoteName = "/simple_notes";

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

static gboolean
sn_store_update_note_last_edited(SNStore *self, guint64 id);

static GFile *
sn_store_create_content_file(SNStore *self);

static GFile *
sn_store_create_tmp_file(SNStore *self);

static guint64
sn_store_get_selected_folder(SNStore *self);

static guint64
sn_store_get_selected_note(SNStore *self);

static void
sn_store_iterate(SNStore *self, SNDataIterator *itr, gboolean (*handler)(SNDataIterator *));


static void
sn_store_set_property(GObject *self, guint propID, const GValue *value, GParamSpec *pSpec)
{
  SNStore *store = SN_STORE(self);
  SN_RETURN_IF_FAIL(store, &kError);

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
  SN_RETURN_IF_FAIL(store, &kError);

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
  self->_folderSelected = sn_store_get_selected_folder(self);
  self->_noteChanged = 0;
  self->_noteSelected = sn_store_get_selected_note(self);
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
sn_store_insert_note(SNStore *self)
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

GFile *
sn_store_create_note_for_editing(SNStore *self)
{
  gboolean end = sn_store_end_editing(self);
  SN_RETURN_VAL_IF_FAIL(end, NULL, &kError);
  SN_RETURN_VAL_IF_FAIL(self->_noteSelected, NULL, &kError);

  GFile *source = sn_store_create_content_file(self);
  SN_RETURN_VAL_IF_FAIL(source, NULL, &kError);

  GFile *tmpNote = sn_store_create_tmp_file(self);

  GError *error = NULL;
  gboolean copied = g_file_copy(source,
				tmpNote,
				G_FILE_COPY_OVERWRITE,
				NULL,
				NULL,
				NULL,
				&error);
  if (!copied)
    {
      g_clear_object(&tmpNote);
    }
  
  g_clear_object(&source);

  return tmpNote;
}

gboolean
sn_store_end_editing(SNStore *self)
{
  gboolean saved = sn_store_save_note(self);
  SN_RETURN_VAL_IF_FAIL(saved, FALSE, &kError);
  GFile *tmpNote = sn_store_create_tmp_file(self);

  GError *error = NULL;
  gboolean deleted = g_file_delete(tmpNote,
				   NULL,
				   &error);
  SN_RETURN_VAL_IF_FAIL(deleted, FALSE, &error);

  return TRUE;
}

gboolean
sn_store_save_note(SNStore *self)
{
  GFile *source = sn_store_create_content_file(self);
  SN_RETURN_VAL_IF_FAIL(source, FALSE, &kError);

  GFile *tmpNote = sn_store_create_tmp_file(self);

  GError *error = NULL;
  gboolean copied = g_file_copy(tmpNote,
				source,
				G_FILE_COPY_OVERWRITE,
				NULL,
				NULL,
				NULL,
				&error);
  g_object_unref(source);
  g_object_unref(tmpNote);
  SN_RETURN_VAL_IF_FAIL(copied, FALSE, &error);

  return TRUE;
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
  if (!folderID) return FALSE;
  
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
  if (deselect && self->_noteSelected == noteChanged)
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

static gboolean
sn_store_update_note_last_edited(SNStore *self, guint64 id)
{
  gint64 lastEdited = sn_store_get_last_edited(self);
  gboolean success = sn_sql_controller_update_note_last_edited(self->_sql, id, lastEdited);

  return sn_store_assign_note_changed(self, id, success, FALSE);
}

static GFile *
sn_store_create_content_file(SNStore *self)
{
  SNNoteIterator *notes = sn_store_create_note_iterator(self);
  SN_RETURN_VAL_IF_FAIL(notes, NULL, &kError);

  SNDataIterator *itr = SN_DATA_ITERATOR(notes);
  
  GFile *content = NULL;
  gboolean handler(SNDataIterator *itr)
  {
    if (sn_note_iterator_item_selected(notes) == TRUE)
      {
	gchar *path = sn_note_iterator_item_content(notes);
	content = g_file_new_for_path(path);
	
	return TRUE;
      }

    return FALSE;
  }

  sn_store_iterate(self, itr, handler);

  return content;
}

static GFile *
sn_store_create_tmp_file(SNStore *self)
{
  gchar const *tmp = g_get_tmp_dir();
  glong const tmpLen = strlen(tmp) + strlen(kNoteName) + 1;
  gchar path[tmpLen];
  strcat(path, tmp);
  strcat(path, kNoteName);
  GFile *tmpNote = g_file_new_for_path(path);

  return tmpNote;
}

static guint64
sn_store_get_selected_folder(SNStore *self)
{
  SNFolderIterator *folders = sn_store_create_folder_iterator(self);
  SN_RETURN_VAL_IF_FAIL(folders, 0, &kError);
  SNDataIterator *itr = SN_DATA_ITERATOR(folders);

  guint64 selectedFolder = 0;
  gboolean handler(SNDataIterator *itr)
  {
    if (sn_folder_iterator_item_selected(folders) == TRUE)
      {
	selectedFolder = sn_folder_iterator_item_id(folders);
	return TRUE;
      }

    return FALSE;
  }
  
  sn_store_iterate(self, itr, handler);
  g_object_unref(folders);
  
  return selectedFolder;
}

static guint64
sn_store_get_selected_note(SNStore *self)
{
  SNNoteIterator *notes = sn_store_create_note_iterator(self);
  SN_RETURN_VAL_IF_FAIL(notes, 0, &kError);
  SNDataIterator *itr = SN_DATA_ITERATOR(notes);

  guint64 selectedNote = 0;
  gboolean handler(SNDataIterator *itr)
  {
    if (sn_note_iterator_item_selected(notes) == TRUE)
      {
	selectedNote = sn_note_iterator_item_id(notes);
	
	return TRUE;
      }

    return FALSE;
  }

  sn_store_iterate(self, itr, handler);

  g_object_unref(notes);

  return selectedNote;
}

static void
sn_store_iterate(SNStore *self, SNDataIterator *itr, gboolean (*handler)(SNDataIterator *))
{
  SNIteratorResult result = sn_data_iterator_next(itr);
  while(result == SNIteratorResultRow)
    {
      if (handler(itr)) break;
      result = sn_data_iterator_next(itr);
    }

  SN_RETURN_IF_FAIL(result != SNIteratorResultError, &kError);
}
