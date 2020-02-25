//
// sql_controller.h
//


#ifndef sn_sql_controller____FILEEXTENSION___
#define sn_sql_controller____FILEEXTENSION___

#include "simple_notes.h"
#include "statement.h"


#define SN_TYPE_SQL_CONTROLLER sn_sql_controller_get_type()
G_DECLARE_FINAL_TYPE(SNSQLController, sn_sql_controller, SN, SQL_CONTROLLER, GObject)


SNSQLController *
sn_sql_controller_new(void);

SNStatement *
sn_sql_controller_select_folders(SNSQLController *self);

gboolean
sn_sql_controller_update_folder_title(SNSQLController *self, guint64 id, gchar *title);

gboolean
sn_sql_controller_update_folder_selected(SNSQLController *self, guint64 id, gboolean selected);

gboolean
sn_sql_controller_insert_folder(SNSQLController *self,
				gchar *title,
				gboolean selected);

gboolean
sn_sql_controller_delete_folder(SNSQLController *self, guint64 id);


SNStatement *
sn_sql_controller_select_notes(SNSQLController *self, guint64 folderID);

gboolean
sn_sql_controller_update_note_folder_id(SNSQLController *self, guint64 id, guint64 folderID);

gboolean
sn_sql_controller_update_note_last_edited(SNSQLController *self, guint64 id, gint64 lastEdited);

gboolean
sn_sql_controller_update_note_selected(SNSQLController *self,
				       guint64 id,
				       guint64 folderID,
				       gboolean selected);

gboolean
sn_sql_controller_insert_note(SNSQLController *self,
			      guint64 folderID,
			      gchar *content,
			      gint64 lastEdited,
			      gboolean selected);

gboolean
sn_sql_controller_delete_note(SNSQLController *self, guint64 id, guint64 folderID);


#endif
