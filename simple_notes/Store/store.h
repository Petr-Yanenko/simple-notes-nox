//
//  store.h
//


#ifndef sn_store____FILEEXTENTION___
#define sn_store____FILEEXTENTION___

#include "simple_notes.h"


#define SN_TYPE_STORE sn_store_get_type()
G_DECLARE_FINAL_TYPE(SNStore, sn_store, SN, STORE, GObject)

SNStore *sn_store_get_instance(void);

SNFolderIterator *sn_store_get_folders(SNStore *self);
gboolean sn_store_save_folder(
                              SNStore *self,
                              guint64 id,
                              gboolean selected,
                              glong count,
                              guint8 *title,
                              SNError *error
                              );
gboolean sn_store_insert_folder(SNStore *self, guint8 *title, SNError *error);
gboolean sn_store_delete_folder(SNStore *self, guint64 id, SNError *error);

#endif
