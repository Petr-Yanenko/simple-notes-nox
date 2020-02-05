//
// data_iterator_private.h
//

#ifndef sn_data_iterator_private____FILEEXTENSION___
#define sn_data_iterator_private____FILEEXTENSION___

#include "data_iterator.h"


SNStatement *
sn_data_iterator_private_get_statement(SNDataIterator *self);

void
sn_data_iterator_private_set_statement(SNDataIterator *self, SNStatement *stmt);


#endif
