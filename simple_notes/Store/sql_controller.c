//
// sql_controller.c
//

#include "sql_controller.h"
#include "data_base.h"


struct _SNSQLController {
  GObject _parent;

  SNDataBase *_db;
};


G_DEFINE_TYPE(SNSQLController, sn_sql_controller, G_TYPE_OBJECT)


enum {
      CREATE_FOLDERS_INDEX = 0,
      SELECT_FOLDERS_INDEX,
      UPDATE_FOLDER_TITLE_INDEX,
      UPDATE_FOLDER_COUNT_INDEX,
      DESELECT_FOLDERS_INDEX,
      UPDATE_FOLDER_SELECTED_INDEX,
      INSERT_FOLDER_INDEX,
      DELETE_FOLDER_INDEX,
      CREATE_NOTES_INDEX,
      SELECT_NOTES_INDEX,
      UPDATE_NOTE_FOLDER_INDEX,
      UPDATE_NOTE_DATE_INDEX,
      DESELECT_NOTES_INDEX,
      UPDATE_NOTE_SELECTED_INDEX,
      INSERT_NOTE_INDEX,
      DELETE_NOTE_INDEX,
      BEGIN_TRANSACTION_INDEX,
      COMMIT_TRANSACTION_INDEX,
      INDEXES_NUMBER
};

static gchar *const kStmtKeys[] = {
				   "create_folders",
				   "select-folders",
				   "update-folder-title",
				   "update_folder-count",
				   "deselect_folders",
				   "update-folder-selected",
				   "insert-folder",
				   "delete-folder",
				   "create-notes",
				   "select-notes",
				   "update-note-folder",
				   "update-note-date",
				   "deselect-notes",
				   "update-note-selected",
				   "insert-note",
				   "delete-note",
				   "begin-transaction",
				   "commit-transaction"
};


static SNError const kError = SNErrorSQLController;


static gboolean
sn_sql_controller_select(SNSQLController *self,
			 guint64 id,
			 gboolean selected,
			 gboolean (*select)(gchar *idStr, gchar *selectedStr),
			 gboolean (*deselect)(void));

static gboolean
sn_sql_controller_perform_transaction(SNSQLController *self, gboolean (*operation)(void));

static gboolean
sn_sql_controller_update_folder_count(SNSQLController *self, gchar *id, glong increment);

static gboolean
sn_sql_controller_execute(SNSQLController *self, gchar *stmt, glong paramCount, ...);


static void
sn_sql_controller_dispose(GObject *self)
{
  SNSQLController *controller = SN_SQL_CONTROLLER(self);
  g_clear_object(&controller->_db);

  G_OBJECT_CLASS(sn_sql_controller_parent_class)->dispose(self);
}

static void
sn_sql_controller_class_init(SNSQLControllerClass *class)
{
  GObjectClass *gClass = G_OBJECT_CLASS(class);
  gClass->dispose = sn_sql_controller_dispose;
}

static void
sn_sql_controller_init(SNSQLController *self)
{
  gchar *stmts[] = {
		    "CREATE TABLE IF NOT EXIST folders (id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT, count TEXT, selected INT)",
		    "SELECT * FROM folders",
		    "UPDATE folders SET title = $TITLE WHERE id == $ID",
		    "UPDATE folders SET count = count + $INCREMENT WHERE id == $ID",
		    "UPDATE folders SET selected = 0",
		    "UPDATE folders SET selected = $SELECTED WHERE id == $ID",
		    "INSERT INTO folders (title, count, selected) VALUES ($TITLE, 0, $SELECTED)",
		    "DELETE FROM folders WHERE id == $ID",
		    "CREATE TABLE IF NOT EXIST notes (id INTEGER PRIMARY KEY AUTOINCREMENT, folder_id INT REFERENCES folders (id) ON DELETE CASCADE, content TEXT, last_edited INT, selected INT)",
		    "SELECT * FROM notes WHERE folder_id == $FOLDER_ID",
		    "UPDATE notes SET folder_id = $FOLDER_ID selected = 0 WHERE id == $NOTE_ID",
		    "UPDATE notes SET last_edited = $LAST_EDITED WHERE id == $NOTE_ID",
		    "UPDATE notes SET selected = 0 WHERE folder_id == $FOLDER_ID",
		    "UPDATE notes SET selected = $SELECTED WHERE id == $NOTE_ID AND folder_id == FOLDER_ID",
		    "INSERT INTO notes (folder_id, content, last_edited, selected) VALUES ($FOLDER_ID, $CONTENT, $LAST_EDITED, $SELECTED)",
		    "DELETE FROM notes WHERE id == $NOTE_ID AND folder_id == $FOLDER_ID",
		    "BEGIN TRANSACTION",
		    "COMMIT TRANSACTION"
  };

  self->_db = sn_data_base_new();

  for (glong i = 0; i < INDEXES_NUMBER; i++)
    {
      gboolean success = sn_data_base_add(self->_db, kStmtKeys[i], stmts[i]);
      SN_RETURN_IF_FAIL(success, &kError);
    }

  gboolean folders = sn_data_base_execute(self->_db, kStmtKeys[CREATE_FOLDERS_INDEX], 0, NULL);
  gboolean notes = sn_data_base_execute(self->_db, kStmtKeys[CREATE_NOTES_INDEX], 0, NULL);
  SN_RETURN_IF_FAIL(folders && notes, &kError);
}

SNSQLController *
sn_sql_controller_new(void)
{
  return g_object_new(SN_TYPE_SQL_CONTROLLER, NULL);
}

SNStatement *
sn_sql_controller_select_folders(SNSQLController *self)
{  
  SNStatement *stmt = sn_data_base_bind(self->_db, kStmtKeys[SELECT_FOLDERS_INDEX], 0, NULL);
  SN_RETURN_VAL_IF_FAIL(stmt, NULL, &kError);
  
  return stmt;
}

gboolean
sn_sql_controller_update_folder_title(SNSQLController *self, guint64 id, gchar *title)
{
  gchar idString[kLongLongSymbols];
  sn_print_guint64_value(idString, id);

  glong paramCount = 2;

  return sn_sql_controller_execute(self,
				   kStmtKeys[UPDATE_FOLDER_TITLE_INDEX],
				   paramCount,
				   title,
				   idString,
				   NULL);
}

gboolean
sn_sql_controller_update_folder_selected(SNSQLController *self, guint64 id, gboolean selected)
{
  gboolean select(gchar *idStr, gchar *selectedStr)
  {
    glong paramCount = 2;
    
    return sn_data_base_execute(self->_db,
				kStmtKeys[UPDATE_FOLDER_SELECTED_INDEX],
				paramCount,
				selectedStr,
				idStr,
				NULL);
  }
  
  gboolean deselect(void)
  {
    glong deselectParams = 0;
    return sn_data_base_execute(self->_db,
				kStmtKeys[DESELECT_FOLDERS_INDEX],
				deselectParams,
				NULL);
  }

  return sn_sql_controller_select(self, id, selected, select, deselect);
}

gboolean
sn_sql_controller_insert_folder(SNSQLController *self, gchar *title, gboolean selected)
{
  gchar selectedString[kSelectedSymbols];
  sn_print_boolean_value(selectedString, selected);
  
  glong paramCount = 2;

  return sn_sql_controller_execute(self,
				   kStmtKeys[INSERT_FOLDER_INDEX],
				   paramCount,
				   title,
				   selectedString,
				   NULL);
}

gboolean
sn_sql_controller_delete_folder(SNSQLController *self, guint64 id)
{
  glong paramCount = 1;

  gchar idString[kLongLongSymbols];
  sn_print_guint64_value(idString, id);

  return sn_sql_controller_execute(self, kStmtKeys[DELETE_FOLDER_INDEX], paramCount, idString, NULL);
}

SNStatement *
sn_sql_controller_select_notes(SNSQLController *self, guint64 folderID)
{
  glong paramsNumber = 1;
  gchar idString[kLongLongSymbols];

  sn_print_guint64_value(idString, folderID);
  
  SNStatement *stmt = sn_data_base_bind(self->_db,
					kStmtKeys[SELECT_NOTES_INDEX],
					paramsNumber,
					idString,
					NULL);
  
  SN_RETURN_VAL_IF_FAIL(stmt, NULL, &kError);

  return stmt;
}

gboolean
sn_sql_controller_update_note_folder_id(SNSQLController *self, guint64 id, guint64 folderID)
{
  glong paramCount = 2;

  gchar idString[kLongLongSymbols];
  sn_print_guint64_value(idString, id);

  gchar folderIDString[kLongLongSymbols];
  sn_print_guint64_value(folderIDString, folderID);

  return sn_sql_controller_execute(self,
				   kStmtKeys[UPDATE_NOTE_FOLDER_INDEX],
				   paramCount,
				   folderIDString,
				   idString,
				   NULL);
}

gboolean
sn_sql_controller_update_note_last_edited(SNSQLController *self, guint64 id, gint64 lastEdited)
{
  glong paramCount = 2;

  gchar idString[kLongLongSymbols];
  sn_print_guint64_value(idString, id);

  gchar lastEditedString[kLongLongSymbols];
  sn_print_gint64_value(lastEditedString, lastEdited);

  return sn_sql_controller_execute(self,
				   kStmtKeys[UPDATE_NOTE_DATE_INDEX],
				   paramCount,
				   lastEditedString,
				   idString,
				   NULL);
}

gboolean
sn_sql_controller_update_note_selected(SNSQLController *self,
				       guint64 id,
				       guint64 folderID,
				       gboolean selected)
{
  gchar folderIDString[kLongLongSymbols];
  sn_print_guint64_value(folderIDString, folderID);

  gboolean select(gchar *idStr, gchar *selectedStr)
  {
    glong paramCount = 3;
    return sn_data_base_execute(self->_db,
				kStmtKeys[UPDATE_NOTE_SELECTED_INDEX],
				paramCount,
				selectedStr,
				idStr,
				folderIDString,
				NULL);
  }

  gboolean deselect(void)
  {
    glong deselectParams = 1;
    return sn_data_base_execute(self->_db,
				kStmtKeys[DESELECT_NOTES_INDEX],
				deselectParams,
				folderIDString,
				NULL);
  }

  return sn_sql_controller_select(self, id, selected, select, deselect);
}

gboolean
sn_sql_controller_insert_note(SNSQLController *self,
			      guint64 folderID,
			      gchar *content,
			      gint64 lastEdited,
			      gboolean selected)
{
  glong paramCount = 4;

  gchar lastEditedString[kLongLongSymbols];
  sn_print_gint64_value(lastEditedString, lastEdited);

  gchar folderIDString[kLongLongSymbols];
  sn_print_guint64_value(folderIDString, folderID);

  gchar selectedString[kSelectedSymbols];
  sn_print_boolean_value(selectedString, selected);

  gboolean transaction(void)
  {
    gboolean insert = sn_data_base_execute(self->_db,
					   kStmtKeys[INSERT_NOTE_INDEX],
					   paramCount,
					   folderIDString,
					   content,
					   lastEditedString,
					   selectedString,
					   NULL);
    SN_RETURN_VAL_IF_FAIL(insert, FALSE, &kError);

    gboolean update = sn_sql_controller_update_folder_count(self, folderIDString, 1);
    SN_RETURN_VAL_IF_FAIL(update, FALSE, &kError);

    return TRUE;
  }

  return sn_sql_controller_perform_transaction(self, transaction);
}

gboolean
sn_sql_controller_delete_note(SNSQLController *self, guint64 id, guint64 folderID)
{
  glong deleteCount = 2;
  glong updateCount = 2;
  
  gchar idStr[kLongLongSymbols];
  sn_print_guint64_value(idStr, id);

  gchar folderIDStr[kLongLongSymbols];
  sn_print_guint64_value(folderIDStr, folderID);

  gboolean transaction(void)
  {
    gboolean delete = sn_data_base_execute(self->_db,
					   kStmtKeys[DELETE_NOTE_INDEX],
					   deleteCount,
					   idStr,
					   folderIDStr);
    SN_RETURN_VAL_IF_FAIL(delete, FALSE, &kError);

    gboolean update = sn_sql_controller_update_folder_count(self, folderIDStr, -1);
    SN_RETURN_VAL_IF_FAIL(update, FALSE, &kError);

    return TRUE;
  }

  return sn_sql_controller_perform_transaction(self, transaction);
}

static gboolean
sn_sql_controller_select(SNSQLController *self,
			 guint64 id,
			 gboolean selected,
			 gboolean (*select)(gchar *idStr, gchar *selectedStr),
			 gboolean (*deselect)(void))
{
  gchar idString[kLongLongSymbols];
  sn_print_guint64_value(idString, id);

  gchar selectedString[kSelectedSymbols];
  sn_print_boolean_value(selectedString, selected);
  
  if (!selected)
    {
      gboolean isSelected = select(idString, selectedString);
      SN_RETURN_VAL_IF_FAIL(isSelected, FALSE, &kError);

      return TRUE;
    }
  else
    {
      gboolean transaction(void)
      {
	gboolean isDeselected = deselect();
	SN_RETURN_VAL_IF_FAIL(isDeselected, FALSE, &kError);
	
	gboolean isSelected = select(idString, selectedString);
	SN_RETURN_VAL_IF_FAIL(isSelected, FALSE, &kError);

	return TRUE;
      }
      
      return sn_sql_controller_perform_transaction(self, transaction);
    }
}

static gboolean
sn_sql_controller_perform_transaction(SNSQLController *self, gboolean (*operation)(void))
{
  gboolean begin = sn_data_base_execute(self->_db, kStmtKeys[BEGIN_TRANSACTION_INDEX], 0, NULL);
  SN_RETURN_VAL_IF_FAIL(begin, FALSE, &kError);

  gboolean success = operation();
      
  gboolean commit = sn_data_base_execute(self->_db, kStmtKeys[COMMIT_TRANSACTION_INDEX], 0, NULL);
  SN_RETURN_VAL_IF_FAIL(commit, FALSE, &kError);

  return success;
}

static gboolean
sn_sql_controller_update_folder_count(SNSQLController *self, gchar *id, glong increment)
{
  glong paramsCount = 2;

  gchar incrementStr[kLongLongSymbols];
  sn_print_long_value(incrementStr, increment);

  return sn_data_base_execute(self->_db,
			      kStmtKeys[UPDATE_FOLDER_COUNT_INDEX],
			      paramsCount,
			      incrementStr,
			      id,
			      NULL);
}

static gboolean
sn_sql_controller_execute(SNSQLController *self, gchar *stmt, glong paramCount, ...)
{
  va_list args;
  gboolean success = sn_data_base_execute(self->_db, stmt, paramCount, args);
  SN_RETURN_VAL_IF_FAIL(success, FALSE, &kError);

  return TRUE;
}
