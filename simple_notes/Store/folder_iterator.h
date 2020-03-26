//
// folder_iterator.h
//

#ifndef sn_folder_iterator____FILEEXTENSION___
#define sn_folder_iterator____FILEEXTENSION___

#include "entity_iterator.h"


#define SN_TYPE_FOLDER_ITERATOR sn_folder_iterator_get_type()
G_DECLARE_FINAL_TYPE(SNFolderIterator, sn_folder_iterator, SN, FOLDER_ITERATOR, SNEntityIterator)


SNFolderIterator *
sn_folder_iterator_new(SNStatement *stmt);


guint64
sn_folder_iterator_item_id(SNFolderIterator *self);

gchar *
sn_folder_iterator_item_title(SNFolderIterator *self);

glong
sn_folder_iterator_item_count(SNFolderIterator *self);

gboolean
sn_folder_iterator_item_selected(SNFolderIterator *self);


#endif
