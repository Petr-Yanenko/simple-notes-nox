//
// store.c
//

#include "store.h"
#include "sql_controller.h"
#include <stdio.h>
#include <gio.h>


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

static gchar *const kNoteName = "simple_notes";

static SNError const kError = SNErrorStore;


static gboolean
sn_store_print_note_path(SNStore *self, gchar *buff, guint64 folderID);

static SNEntityIterator *
sn_store_create_iterator(SNStore *self,
			 SNStatement *stmt,
			 SNEntityIterator * (*constructor)(SNStatement *));

static gboolean
sn_store_assign_folder_changed(SNStore *self,
			       guint64 folderChanged,
			       gboolean changed);

static gboolean
sn_store_assign_note_changed(SNStore *self,
			     guint64 noteChanged,
			     gboolean changed,
			     gboolean deselect);

static gboolean
sn_store_assign_folder_selected(SNStore *self,
				guint64 folderSelected,
				gboolean changed);

static gboolean
sn_store_assign_note_selected(SNStore *self,
			      guint64 noteSelected,
			      gboolean changed);

static gboolean
sn_store_update_note_last_edited(SNStore *self);

static GFile *
sn_store_create_content_file(SNStore *self, SNError *error);

static GFile *
sn_store_create_tmp_file(SNStore *self);

static guint64
sn_store_get_selected_item(SNStore *self, SNEntityIterator *itr);


static void
sn_store_set_property(GObject *self,
		      guint propID,
		      const GValue *value,
		      GParamSpec *pSpec)
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
sn_store_get_property(GObject *self,
		      guint propID,
		      GValue *value,
		      GParamSpec *pSpec)
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
  self->_sql = sn_sql_controller_new();

  self->_folderChanged = 0;
  self->_folderSelected = 0;

  self->_noteChanged = 0;
  self->_noteSelected = 0;
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

  SNEntityIterator *constructor(SNStatement *stmt)
  {
    return SN_ENTITY_ITERATOR(sn_folder_iterator_new(stmt));
  }

  SNEntityIterator *itr = sn_store_create_iterator(self, stmt, constructor);
  SNFolderIterator *folders = SN_FOLDER_ITERATOR(itr);
  self->_folderSelected = sn_store_get_selected_item(self, itr);

  return folders;
}

gboolean
sn_store_update_folder(SNStore *self, guint64 id, gchar *title)
{
  gboolean success = sn_sql_controller_update_folder_title(self->_sql,
							   id,
							   title);
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
  gboolean edit = sn_store_end_editing(self);
  SN_RETURN_VAL_IF_FAIL(edit, FALSE, &kError);

  gchar buff[1000];
  sprintf(buff, kFolderPathFormat, id);
  GFile *folder = g_file_new_for_path(buff);
  GError *enumErr = NULL;
  GFileEnumerator *notes = g_file_enumerate_children(folder,
						     "standard::*",
						     G_FILE_QUERY_INFO_NONE,
						     NULL,
						     &enumErr);
  g_object_unref(folder);
  gboolean notFound(GError *error)
  {
    return error->domain == G_IO_ERROR && error->code == G_IO_ERROR_NOT_FOUND;
  }

  gboolean delete = TRUE;
  GError *deleteErr = NULL;
  if (!notes)
    {
      SN_RETURN_VAL_IF_FAIL(notFound(enumErr), FALSE, &kError);
    }
  else {
    GError *infoErr = NULL;
    GFileInfo *childInfo = g_file_enumerator_next_file(notes, NULL, &infoErr);
    while(childInfo)
      {
	gchar const *name = g_file_info_get_name(childInfo);
	glong folderLen = strlen(kFolderPathFormat) - strlen("%llu");
	folderLen += kLongLongSymbols;
	glong const noteLen = folderLen + strlen(name) + 1;
	gchar buff[noteLen];
	sprintf(buff, kFolderPathFormat, id);
	strcat(buff, name);
	GFile *note = g_file_new_for_path(buff);
	delete = g_file_delete(note, NULL, &deleteErr);

	g_object_unref(note);
	g_object_unref(childInfo);
	if(!delete) break;
	childInfo = g_file_enumerator_next_file(notes, NULL, &infoErr);
      }

    g_file_enumerator_close(notes, NULL, NULL);
    g_object_unref(notes);
    SN_RETURN_VAL_IF_FAIL(delete, FALSE, &kError);
    SN_RETURN_VAL_IF_FAIL(infoErr == NULL, FALSE, &kError);
  }

  GFile *deletedFolder = g_file_new_for_path(buff);
  delete = g_file_delete(deletedFolder, NULL, &deleteErr);
  g_object_unref(deletedFolder);
  if (!delete)
    {
      SN_RETURN_VAL_IF_FAIL(notFound(deleteErr), FALSE, &kError);
    }

  gboolean success = sn_sql_controller_delete_folder(self->_sql, id);
  if (self->_folderSelected == id)
    {
      sn_store_assign_folder_selected(self, kItemDeselected, success);
      self->_noteSelected = 0;
    }
  return sn_store_assign_folder_changed(self, id, success);
}

gboolean
sn_store_select_folder(SNStore *self, guint64 id)
{
  gboolean success = sn_sql_controller_update_folder_selected(self->_sql,
							      id,
							      TRUE);
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
  SNStatement *stmt = sn_sql_controller_select_notes(self->_sql,
						     self->_folderSelected);

  SNEntityIterator *constructor(SNStatement *stmt)
  {
    return SN_ENTITY_ITERATOR(sn_note_iterator_new(stmt));
  }

  SNEntityIterator *itr = sn_store_create_iterator(self, stmt, constructor);
  SNNoteIterator *notes = SN_NOTE_ITERATOR(itr);
  self->_noteSelected = sn_store_get_selected_item(self, itr);

  return notes;
}

gboolean
sn_store_update_note(SNStore *self, guint64 id, guint64 newFolderID)
{
  gboolean folder = self->_folderSelected;
  gboolean success = sn_sql_controller_update_note_folder_id(self->_sql,
							     id,
							     folder,
							     newFolderID);
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
  gboolean edit = sn_store_end_editing(self);
  SN_RETURN_VAL_IF_FAIL(edit, FALSE, &kError);

  SNError error = -1;
  GFile *content = sn_store_create_content_file(self, &error);
  if (content)
    {
      GError *deletingErr = NULL;
      gboolean deleted = g_file_delete(content, NULL, &deletingErr);
      g_object_unref(content);
      SN_RETURN_VAL_IF_FAIL(deleted, FALSE, &kError);
    }
  else
    {
      SN_RETURN_VAL_IF_FAIL(error == SNErrorNotSelected, FALSE, &error);
    }

  gboolean success = sn_sql_controller_delete_note(self->_sql,
						   id,
						   self->_folderSelected);

  gboolean changed = sn_store_assign_note_changed(self, id, success, TRUE);
  SN_RETURN_VAL_IF_FAIL(changed, FALSE, &kError);

  return TRUE;
}

gboolean
sn_store_select_note(SNStore *self, guint64 id)
{
  gboolean edit = sn_store_end_editing(self);
  SN_RETURN_VAL_IF_FAIL(edit, FALSE, &kError);

  guint64 folder = self->_folderSelected;
  gboolean success = sn_sql_controller_update_note_selected(self->_sql,
							    id,
							    folder,
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

  SNError contentError = -1;
  GFile *source = sn_store_create_content_file(self, &contentError);
  if (!source && contentError == SNErrorNotSelected) return NULL;
  SN_RETURN_VAL_IF_FAIL(source, NULL, &contentError);

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
  SNError error;
  gboolean saved = sn_store_save_note(self, &error);
  if (!saved && (error == SNErrorNotFound || error == SNErrorNotSelected))
    return TRUE;

  SN_RETURN_VAL_IF_FAIL(saved, FALSE, &error);
  GFile *tmpNote = sn_store_create_tmp_file(self);

  GError *deleteError = NULL;
  gboolean deleted = g_file_delete(tmpNote,
				   NULL,
				   &deleteError);
  g_object_unref(tmpNote);
  SN_RETURN_VAL_IF_FAIL(deleted, FALSE, &kError);

  return TRUE;
}

gboolean
sn_store_save_note(SNStore *self, SNError *error)
{
  GFile *source = sn_store_create_content_file(self, error);
  if (!source && error && *error == SNErrorNotSelected) return FALSE;
  SN_RETURN_VAL_IF_FAIL(source, FALSE, error);

  GFile *tmpNote = sn_store_create_tmp_file(self);

  GError *copyErr = NULL;
  gboolean copied = g_file_copy(tmpNote,
				source,
				G_FILE_COPY_OVERWRITE,
				NULL,
				NULL,
				NULL,
				&copyErr);
  g_object_unref(source);
  g_object_unref(tmpNote);
  if (!copied && error)
    {
      if (copyErr->code == G_IO_ERROR_NOT_FOUND)
	{
	  *error = SNErrorNotFound;
	}
      else
	{
	  *error = kError;
	}
    }
  SN_RETURN_VAL_IF_FAIL(copied, FALSE, error);

  gboolean time = sn_store_update_note_last_edited(self);
  if (!time && error) *error = kError;
  SN_RETURN_VAL_IF_FAIL(time, FALSE, error);

  return TRUE;
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

  gchar folderPath[1000];
  sprintf(folderPath, kFolderPathFormat, folderID);
  GFile *folder = g_file_new_for_path(folderPath);
  GError *folderErr = NULL;
  gboolean folderCreated = g_file_make_directory(folder, NULL, &folderErr);
  g_object_unref(folder);
  if (!folderCreated)
    {
      gboolean exists, gioDomain, existsErr;
      gioDomain = folderErr->domain == G_IO_ERROR;
      existsErr = folderErr->code == G_IO_ERROR_EXISTS;
      exists = gioDomain && existsErr;
      SN_RETURN_VAL_IF_FAIL(exists, FALSE, &kError);
    }

  gboolean success = FALSE;
  for (glong i = 0; !success && i < RAND_MAX; i++)
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
	  g_object_unref(fileStream);
	}

      g_object_unref(file);
    }

  if (success) return TRUE;

  SN_RETURN_VAL_IF_FAIL(success, FALSE, &kError);
}

static SNEntityIterator *
sn_store_create_iterator(SNStore *self,
			 SNStatement *stmt,
			 SNEntityIterator * (*constructor)(SNStatement *))
{
  SNEntityIterator *iterator = NULL;
  if (stmt)
    {
      iterator = constructor(stmt);
    }

  return iterator;
}

static gboolean
sn_store_assign_property(SNStore *self,
			 gchar *prop,
			 guint64 value,
			 gboolean changed)
{
  if (changed)
    {
      g_object_set(G_OBJECT(self), prop, value, NULL);
    }

  return changed;
}

static gboolean
sn_store_assign_folder_changed(SNStore *self,
			       guint64 folderChanged,
			       gboolean changed)
{
  return sn_store_assign_property(self, kFolderChanged, folderChanged, changed);
}

static gboolean
sn_store_assign_folder_selected(SNStore *self,
				guint64 folderSelected,
				gboolean changed)
{
  return sn_store_assign_property(self,
				  kFolderSelected,
				  folderSelected,
				  changed);
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
sn_store_assign_note_selected(SNStore *self,
			      guint64 noteSelected,
			      gboolean changed)
{
  gboolean select = sn_store_assign_property(self,
					     kNoteSelected,
					     noteSelected,
					     changed);

  return select;
}

static gint64
sn_store_get_last_edited(SNStore *self)
{
  GDateTime *now = g_date_time_new_now_local();
  gint64 unixNow = g_date_time_to_unix(now);
  g_date_time_unref(now);

  return unixNow;
}

static gboolean
sn_store_update_note_last_edited(SNStore *self)
{
  guint64 note = self->_noteSelected;
  guint64 folder = self->_folderSelected;
  gint64 lastEdited = sn_store_get_last_edited(self);
  gboolean success = sn_sql_controller_update_note_last_edited(self->_sql,
							       note,
							       folder,
							       lastEdited);

  return sn_store_assign_note_changed(self,
				      note,
				      success,
				      FALSE);
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

static void
sn_store_iterate(SNStore *self,
		 SNDataIterator *itr,
		 gboolean (*handler)(SNDataIterator *))
{
  SNIteratorResult result = sn_data_iterator_next(itr);
  while(result == SNIteratorResultRow)
    {
      if (handler(itr)) break;
      result = sn_data_iterator_next(itr);
    }

  gboolean reset = sn_data_iterator_reset(itr);
  SN_RETURN_IF_FAIL(reset, &kError);

  SN_RETURN_IF_FAIL(result != SNIteratorResultError, &kError);
}

static void
sn_store_iterate_entities(SNStore *self,
			  SNEntityIterator *itr,
			  gboolean (*condition)(SNEntityIterator *),
			  void (*handler)(SNEntityIterator *))
{
  SNDataIterator *dataItr = SN_DATA_ITERATOR(itr);

  gboolean find_handler(SNDataIterator *dataItr)
  {
    if (condition(itr))
      {
	handler(itr);
	return TRUE;
      }

    return FALSE;
  }

  sn_store_iterate(self, dataItr, find_handler);
}

static GFile *
sn_store_create_content_file(SNStore *self, SNError *error)
{
  SNNoteIterator *notes = sn_store_create_note_iterator(self);
  if (error) *error = kError;
  SN_RETURN_VAL_IF_FAIL(notes, NULL, error);

  SNEntityIterator *itr = SN_ENTITY_ITERATOR(notes);

  gboolean condition(SNEntityIterator *itr)
  {
    return sn_note_iterator_item_selected(notes);
  }

  GFile *content = NULL;
  void handler(SNEntityIterator *itr)
  {
    gchar *path = sn_note_iterator_item_content(notes);
    content = g_file_new_for_path(path);
  }

  sn_store_iterate_entities(self, itr, condition, handler);
  g_object_unref(notes);
  if (!content && error) *error = SNErrorNotSelected;

  return content;
}

static guint64
sn_store_get_selected_item(SNStore *self, SNEntityIterator *itr)
{
  SN_RETURN_VAL_IF_FAIL(itr, 0, &kError);

  gboolean condition(SNEntityIterator *itr)
  {
    return sn_entity_iterator_item_selected(itr);
  }

  guint64 selectedItem = 0;
  void handler(SNEntityIterator *itr)
  {
    selectedItem = sn_entity_iterator_item_id(itr);
  }

  sn_store_iterate_entities(self, itr, condition, handler);

  return selectedItem;
}
