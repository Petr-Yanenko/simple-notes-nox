//
// entity_iterator.c
//

#include "entity_iterator.h"
#include "data_iterator_private.h"


G_DEFINE_TYPE(SNEntityIterator, sn_entity_iterator, SN_TYPE_DATA_ITERATOR)


static SNError const kError = SNErrorEntityIterator;


static glong
sn_entity_iterator_selected_column(SNEntityIterator *self);


void
sn_entity_iterator_class_init(SNEntityIteratorClass *class)
{
  class->selected_column = NULL;
}

void
sn_entity_iterator_init(SNEntityIterator *self)
{
}

SNEntityIterator *
sn_entity_iterator_new(SNStatement *stmt, GType type)
{
  SNEntityIterator *instance = g_object_new(type, NULL);
  sn_data_iterator_private_set_statement(SN_DATA_ITERATOR(instance), stmt);

  return instance;
}

guint64
sn_entity_iterator_item_id(SNEntityIterator *self)
{
  glong idColumn = 0;

  SNStatement *stmt = NULL;
  GET_STATEMENT(self, &stmt, &kError);

  return (guint64)sn_statement_column_int64(stmt, idColumn);
}

gboolean
sn_entity_iterator_item_selected(SNEntityIterator *self)
{
  SNStatement *stmt = NULL;
  GET_STATEMENT(self, &stmt, &kError);

  glong column = sn_entity_iterator_selected_column(self);
  SN_RETURN_VAL_IF_FAIL(column, FALSE, &kError);

  return sn_statement_column_int(stmt, column);
}

static glong
sn_entity_iterator_selected_column(SNEntityIterator *self)
{
  SNEntityIteratorClass *class;
  SN_GET_CLASS_OR_RETURN_VAL(self,
			     &class,
			     selected_column,
			     SNEntityIterator,
			     SN,
			     ENTITY_ITERATOR,
			     0);

  return class->selected_column(self);
}
