//
//  data_iterator.c
//

#include "data_iterator_private.h"


typedef struct {

  SNStatement *_stmt;

} SNDataIteratorPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SNDataIterator, sn_data_iterator, G_TYPE_OBJECT)


static SNError const kError = SNErrorDataIterator;


static SNIteratorResult
sn_data_iterator_real_first(SNDataIterator *self);

static SNIteratorResult
sn_data_iterator_real_next(SNDataIterator *self);

static gboolean
sn_data_iterator_real_reset(SNDataIterator *self);


static void
sn_data_iterator_dispose(GObject *self)
{
  SNDataIterator *itr = SN_DATA_ITERATOR(self);
  SNDataIteratorPrivate *private = sn_data_iterator_get_instance_private(itr);
  g_clear_object(&private->_stmt);

  G_OBJECT_CLASS(sn_data_iterator_parent_class)->dispose(self);
}

static void
sn_data_iterator_class_init(SNDataIteratorClass *class)
{
  GObjectClass *gClass = G_OBJECT_CLASS(class);

  gClass->dispose = sn_data_iterator_dispose;

  class->first = sn_data_iterator_real_first;
  class->next = sn_data_iterator_real_next;
  class->reset = sn_data_iterator_real_reset;

  g_type_add_instance_private(SN_TYPE_DATA_ITERATOR,
			      sizeof(SNDataIteratorPrivate));
}

static void
sn_data_iterator_init(SNDataIterator *self)
{
  SNDataIteratorPrivate *private = sn_data_iterator_get_instance_private(self);

  private->_stmt = NULL;
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

gboolean
sn_data_iterator_reset(SNDataIterator *self)
{
  SNDataIteratorClass *class = NULL;
  SN_GET_CLASS_OR_RETURN_VAL(self,
			     &class,
			     reset,
			     SNDataIterator,
			     SN,
			     DATA_ITERATOR,
			     FALSE);
  return class->reset(self);
}

SNStatement *
sn_data_iterator_private_get_statement(SNDataIterator *self)
{
  SNDataIteratorPrivate *private = sn_data_iterator_get_instance_private(self);

  return private->_stmt;
}

void
sn_data_iterator_private_set_statement(SNDataIterator *self, SNStatement *stmt)
{
  SNDataIteratorPrivate *private = sn_data_iterator_get_instance_private(self);

  SN_RETURN_IF_FAIL(stmt, NULL);
  if (private->_stmt != stmt)
    {
      if (private->_stmt) g_object_unref(private->_stmt);
      private->_stmt = g_object_ref(stmt);
    }
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

static gint
sn_data_iterator_perform(SNDataIterator *self, void (operation)(SNStatement *))
{
  SNDataIteratorPrivate *private = sn_data_iterator_get_instance_private(self);
  SN_RETURN_VAL_IF_FAIL(sn_statement_is_valid(private->_stmt),
			-1,
			&kError);
  operation(private->_stmt);

  return sn_statement_get_result_code(private->_stmt);
}

static SNIteratorResult
sn_data_iterator_real_first(SNDataIterator *self)
{
  void operation(SNStatement *stmt)
  {
    if (sn_data_iterator_reset(self))
      {
	sn_statement_step(stmt);
      }
  }

  gint code = sn_data_iterator_perform(self, operation);

  return sn_data_iterator_get_result(code);
}

static SNIteratorResult
sn_data_iterator_real_next(SNDataIterator *self)
{
  void operation(SNStatement *stmt)
  {
    sn_statement_step(stmt);
  }

  gint code = sn_data_iterator_perform(self, operation);

  return sn_data_iterator_get_result(code);
}

static gboolean
sn_data_iterator_real_reset(SNDataIterator *self)
{
  void operation(SNStatement *stmt)
  {
    sn_statement_reset(stmt);
  }

  return sn_data_iterator_perform(self, operation) == SQLITE_OK;
}
