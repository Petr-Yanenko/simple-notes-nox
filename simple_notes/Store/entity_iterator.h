//
// entity_iterator.h
//

#ifndef sn_entity_iterator____FILEEXTENSION___
#define sn_entity_iterator____FILEEXTENSION___

#include "data_iterator.h"


#define SN_TYPE_ENTITY_ITERATOR sn_entity_iterator_get_type()
G_DECLARE_DERIVABLE_TYPE(SNEntityIterator,
			 sn_entity_iterator,
			 SN,
			 ENTITY_ITERATOR,
			 SNDataIterator)


struct _SNEntityIteratorClass {
  SNDataIteratorClass _parent;

  //private
  glong (*selected_column)(SNEntityIterator *self);
};


SNEntityIterator *
sn_entity_iterator_new(SNStatement *stmt, GType type);

guint64
sn_entity_iterator_item_id(SNEntityIterator *self);

gboolean
sn_entity_iterator_item_selected(SNEntityIterator *self);


#endif
