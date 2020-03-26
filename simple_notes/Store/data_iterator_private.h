//
// data_iterator_private.h
//

#ifndef sn_data_iterator_private____FILEEXTENSION___
#define sn_data_iterator_private____FILEEXTENSION___

#include "data_iterator.h"


#define GET_STATEMENT(self, pStmt, pError)		\
  {\
    SN_RETURN_VAL_IF_FAIL(pStmt, 0, pError);				\
    SNStatement *stmt = sn_data_iterator_private_get_statement(SN_DATA_ITERATOR(self));\
    SN_RETURN_VAL_IF_FAIL(sn_statement_is_valid(stmt), 0, pError);\
    *pStmt = stmt;\
  }


SNStatement *
sn_data_iterator_private_get_statement(SNDataIterator *self);

void
sn_data_iterator_private_set_statement(SNDataIterator *self, SNStatement *stmt);


#endif
