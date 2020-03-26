//
// note_iterator.h
//

#ifndef sn_note_iterator____FILEEXTENSION___
#define sn_note_iterator____FILEEXTENSION___

#include "entity_iterator.h"


#define SN_TYPE_NOTE_ITERATOR  sn_note_iterator_get_type()
G_DECLARE_FINAL_TYPE(SNNoteIterator, sn_note_iterator, SN, NOTE_ITERATOR, SNEntityIterator)


SNNoteIterator *
sn_note_iterator_new(SNStatement *stmt);


guint64
sn_note_iterator_item_id(SNNoteIterator *self);

guint64
sn_note_iterator_item_folder_id(SNNoteIterator *self);

gchar *
sn_note_iterator_item_content(SNNoteIterator *self);

GDateTime *
sn_note_iterator_create_item_last_edited(SNNoteIterator *self);

gboolean
sn_note_iterator_item_selected(SNNoteIterator *self);
  

#endif
