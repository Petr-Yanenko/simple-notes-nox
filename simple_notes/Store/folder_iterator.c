//
// folder_iterator.c
//

#include "folder_iterator.h"
#include "data_iterator_private.h"


struct _SNFolderIterator {
  SNEntityIterator _parent;
};


G_DEFINE_TYPE(SNFolderIterator, sn_folder_iterator, SN_TYPE_ENTITY_ITERATOR)


static SNError kError = SNErrorFolderIterator;


static glong
sn_folder_iterator_real_selected_column(SNEntityIterator *self);


static void
sn_folder_iterator_class_init(SNFolderIteratorClass *class)
{
  SNEntityIteratorClass *parent = SN_ENTITY_ITERATOR_CLASS(class);
  parent->selected_column = sn_folder_iterator_real_selected_column;
}

static void
sn_folder_iterator_init(SNFolderIterator *self)
{
}

SNFolderIterator *
sn_folder_iterator_new(SNStatement *stmt)
{
  return SN_FOLDER_ITERATOR(sn_entity_iterator_new(stmt,
						   SN_TYPE_FOLDER_ITERATOR));
}

guint64
sn_folder_iterator_item_id(SNFolderIterator *self)
{
  return sn_entity_iterator_item_id(SN_ENTITY_ITERATOR(self));
}

gchar *
sn_folder_iterator_item_title(SNFolderIterator *self)
{
  glong titleColumn = 1;

  SNStatement *stmt = NULL;
  GET_STATEMENT(self, &stmt, &kError);

  return (gchar *)sn_statement_column_text(stmt, titleColumn);
}

glong
sn_folder_iterator_item_count(SNFolderIterator *self)
{
  glong countColumn = 2;

  SNStatement *stmt = NULL;
  GET_STATEMENT(self, &stmt, &kError);

  return sn_statement_column_int(stmt, countColumn);
}

gboolean
sn_folder_iterator_item_selected(SNFolderIterator *self)
{
  ;

  return sn_entity_iterator_item_selected(SN_ENTITY_ITERATOR(self));
}

static glong
sn_folder_iterator_real_selected_column(SNEntityIterator *self)
{
  glong selectedColumn = 3;
  return selectedColumn;
}
