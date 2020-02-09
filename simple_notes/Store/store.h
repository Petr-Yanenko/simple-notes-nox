//
//  store.h
//


#ifndef sn_store____FILEEXTENTION___
#define sn_store____FILEEXTENTION___

#include "simple_notes.h"


extern gchar *const kFolderChanged;
extern guint64 const kFolderInserted; // Equal 0


#define SN_TYPE_STORE sn_store_get_type()
G_DECLARE_FINAL_TYPE(SNStore, sn_store, SN, STORE, GObject)

  
SNStore *
sn_store_get_instance(void);


SNFolderIterator *
sn_store_create_folder_iterator(SNStore *self);

gboolean
sn_store_update_folder(SNStore *self,
                       guint64 id,
                       gboolean selected,
                       glong count,
                       gchar *title);

gboolean
sn_store_insert_folder(SNStore *self, gchar *title);

gboolean
sn_store_delete_folder(SNStore *self, guint64 id);


guint64
sn_store_get_folder_changed(SNStore *self);


#endif
