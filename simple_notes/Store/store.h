//
// store.h
//


#ifndef sn_store____FILEEXTENTION___
#define sn_store____FILEEXTENTION___

#include "simple_notes.h"
#include "folder_iterator.h"
#include "note_iterator.h"


//signals
extern gchar *const kFolderChanged;
extern gchar *const kFolderSelected;

extern gchar *const kNoteChanged;
extern gchar *const kNoteSelected;

//constants
extern guint64 const kItemInserted; // Equal 0
extern guint64 const kItemDeselected; // Equal 0
extern gint64 const kNotEdited; // Equal 0


#define SN_TYPE_STORE sn_store_get_type()
G_DECLARE_FINAL_TYPE(SNStore, sn_store, SN, STORE, GObject)


SNStore *
sn_store_get_instance(void);


SNFolderIterator *
sn_store_create_folder_iterator(SNStore *self);

gboolean
sn_store_update_folder(SNStore *self, guint64 id, gchar *title);

gboolean
sn_store_insert_folder(SNStore *self, gchar *title);

gboolean
sn_store_delete_folder(SNStore *self, guint64 id);

gboolean
sn_store_select_folder(SNStore *self, guint64 id);


guint64
sn_store_get_folder_changed(SNStore *self);

guint64
sn_store_get_folder_selected(SNStore *self);


SNNoteIterator *
sn_store_create_note_iterator(SNStore *self);

gboolean
sn_store_update_note(SNStore *self, guint64 id, guint64 newFolderID);

gboolean
sn_store_insert_note(SNStore *self);

gboolean
sn_store_delete_note(SNStore *self, guint64 id);

gboolean
sn_store_select_note(SNStore *self, guint64 id);


guint64
sn_store_get_note_changed(SNStore *self);

guint64
sn_store_get_note_selected(SNStore *self);


GFile *
sn_store_create_note_for_editing(SNStore *self);

gboolean
sn_store_end_editing(SNStore *self);

gboolean
sn_store_save_note(SNStore *self, SNError *error);


#endif
