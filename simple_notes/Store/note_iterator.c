//
// note_iterator.c
//

#include "note_iterator.h"
#include "data_iterator_private.h"


struct _SNNoteIterator {
  SNEntityIterator _parent;
};


G_DEFINE_TYPE(SNNoteIterator, sn_note_iterator, SN_TYPE_ENTITY_ITERATOR)


static SNError kError = SNErrorNoteIterator;


static void
sn_note_iterator_class_init(SNNoteIteratorClass *class)
{
}

static void
sn_note_iterator_init(SNNoteIterator *self)
{
}

SNNoteIterator *
sn_note_iterator_new(SNStatement *stmt)
{
  return SN_NOTE_ITERATOR(sn_entity_iterator_new(stmt, SN_TYPE_NOTE_ITERATOR));
}

guint64
sn_note_iterator_item_id(SNNoteIterator *self)
{
  return sn_entity_iterator_item_id(SN_ENTITY_ITERATOR(self));
}

guint64
sn_note_iterator_item_folder_id(SNNoteIterator *self)
{
  glong folderColumn = 1;

  SNStatement *stmt = NULL;
  GET_STATEMENT(self, &stmt, &kError);

  return (guint64)sn_statement_column_int64(stmt, folderColumn);
}

gchar *
sn_note_iterator_item_content(SNNoteIterator *self)
{
  glong contentColumn = 2;

  SNStatement *stmt = NULL;
  GET_STATEMENT(self, &stmt, &kError);

  return (gchar *)sn_statement_column_text(stmt, contentColumn);
}

GDateTime *
sn_note_iterator_create_item_last_edited(SNNoteIterator *self)
{
  glong lastEditedColumn = 3;

  SNStatement *stmt = NULL;
  GET_STATEMENT(self, &stmt, &kError);

  gint64 lastEdited = sn_statement_column_int64(stmt, lastEditedColumn);
  GDateTime *time = g_date_time_new_from_unix_local(lastEdited);

  return time;
}

gboolean
sn_note_iterator_item_selected(SNNoteIterator *self)
{
  glong selectedColumn = 4;

  return sn_entity_iterator_item_selected(SN_ENTITY_ITERATOR(self),
					  selectedColumn);
}
