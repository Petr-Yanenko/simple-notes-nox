//
//  data_iterator.c
//

#include "data_iterator.h"


typedef struct {

  gint _result;
  sqlite3_stmt *_stmt;

} SNDataIteratorPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SNDataIterator, sn_data_iterator,G_TYPE_OBJECT);

static SNIteratorResult
sn_data_iterator_real_first(SNDataIterator *self);

static SNIteratorResult
sn_data_iterator_real_next(SNDataIterator *self);

static gint
sn_data_iterator_real_error(SNDataIterator *self);


static void
sn_data_iterator_class_init(SNDataIteratorClass *class)
{
  class->first = sn_data_iterator_real_first;
  class->next = sn_data_iterator_real_next;
  class->error = sn_data_iterator_real_error;

  g_type_add_instance_private(SN_TYPE_DATA_ITERATOR, sizeof(SNDataIteratorPrivate));
}

static void
sn_data_iterator_init(SNDataIterator *self)
{
  SNDataIteratorPrivate *private = sn_data_iterator_get_instance_private(self);

  private->_stmt = NULL;
  private->_result = SQLITE_OK;
}

SNDataIterator *
sn_data_iterator_new(sqlite3_stmt *stmt)
{
  g_return_val_if_fail(stmt != NULL, NULL);

  SNDataIterator *instance = g_object_new(SN_TYPE_DATA_ITERATOR, NULL);
  SNDataIteratorPrivate *private = sn_data_itarator_get_instance_private(instance);

  private->_stmt = stmt;

  return instance;
}

SNIteratorResult
sn_data_iterator_first(SNDataIterator *self)
{
  SNDataIteratorClass *class = NULL;
  SN_GET_CLASS_OR_RETURN_VAL(self,
                             &class,
                             first,
                             SNDataIterator,
                             SN,
                             DATA_ITERATOR,
                             SNIteratorResultError);
  return class->first(self);
}

SNIteratorResult
sn_data_iterator_next(SNDataIterator *self)
{
  SNDataIteratorClass *class = NULL;
  SN_GET_CLASS_OR_RETURN_VAL(self,
                             &class,
                             next,
                             SNDataIterator,
                             SN,
                             DATA_ITERATOR,
                             SNIteratorResultError);
  return class->next(self);
}

gint
sn_data_iterator_error(SNDataIterator *self)
{
  SNDataIteratorClass *class = NULL;
  SN_GET_CLASS_OR_RETURN_VAL(self,
                             &class,
                             error,
                             SNDataIterator,
                             SN,
                             DATA_ITERATOR,
                             -1);
  return class->error(self);
}

static SNIteratorResult
sn_data_iterator_get_result(gint code)
{
  switch(code)
    {
    case SQLITE_DONE:
      return SNIteratorResultDone;

    case SQLITE_ROW:
      return SNIteratorResultRow;

    default:
      return SNIteratorResultError;
    }
}

static SNIteratorResult
sn_data_iterator_real_first(SNDataIterator *self)
{
  SNDataIteratorPrivate *private = sn_data_iterator_get_instance_private(self);

  gint resetCode = sqlite3_reset(private->_stmt);
  gint stepCode = resetCode;
  if (resetCode == SQLITE_OK)
    {
      stepCode = sqlite3_step(private->_stmt);
    }
  private->_result = stepCode;

  return sn_data_iterator_get_result(stepCode);
}

static SNIteratorResult
sn_data_iterator_real_next(SNDataIterator *self)
{
  SNDataIteratorPrivate *private = sn_data_iterator_get_instance_private(self);

  gint stepCode = sqlite3_step(private->_stmt);
  private->_result = stepCode;

  return sn_data_iterator_get_result(stepCode);
}

static gint
sn_data_iterator_real_error(SNDataIterator *self)
{
  SNDataIteratorPrivate *private = sn_data_iterator_get_instance_private(self);

  return private->_result;
}
