//
//  data_iterator.h
//

#ifndef sn_data_iterator____FILEEXTENTION___
#define sn_data_iterator____FILEEXTENTION___

#include "simple_notes.h"
#include "statement.h"


typedef enum {
              SNIteratorResultError = 0,
              SNIteratorResultRow = 1,
              SNIteratorResultDone = 2
} SNIteratorResult;


#define SN_TYPE_DATA_ITERATOR sn_data_iterator_get_type()
G_DECLARE_DERIVABLE_TYPE(SNDataIterator, sn_data_iterator, SN, DATA_ITERATOR, GObject);


struct _SNDataIteratorClass {

  GObjectClass parent;

  SNIteratorResult (*first)(SNDataIterator *self);
  SNIteratorResult (*next)(SNDataIterator *self);
};


SNIteratorResult
sn_data_iterator_first(SNDataIterator *self);

SNIteratorResult
sn_data_iterator_next(SNDataIterator *self);


#endif
