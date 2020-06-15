//
// note_presenter.c
//


#include "note_presenter.h"
#include "note.h"


struct _SNNotePresenter {
  SNEntityPresenter _parent;

  SNStore *_unsafe_store;
  SNIEntityModel *_unsafe_model;

  gulong _folderSelected;
  gulong _folderChanged;
};


G_DEFINE_TYPE(SNNotePresenter, sn_note_presenter, SN_TYPE_ENTITY_PRESENTER)


static SNError kError = SNErrorNotePresenter;


static void
sn_note_presenter_new_data_handler(GObject *sender,
				   GParamSpec *pspec,
				   gpointer user_data);

static SNEntityIterator *
sn_note_presenter_real_create_iterator(SNEntityPresenter *self);

static SNObject *
sn_note_presenter_real_create_item(SNEntityPresenter *self,
				   SNEntityIterator *itr);


static void
sn_note_presenter_dispose(GObject *self)
{
  SNNotePresenter *pres = SN_NOTE_PRESENTER(self);
  g_signal_handler_disconnect(pres->_unsafe_store, pres->_folderChanged);
  g_signal_handler_disconnect(pres->_unsafe_store, pres->_folderSelected);

  G_OBJECT_CLASS(sn_note_presenter_parent_class)->dispose(self);
}

static void
sn_note_presenter_class_init(SNNotePresenterClass *class)
{
  GObjectClass *gClass = G_OBJECT_CLASS(class);
  gClass->dispose = sn_note_presenter_dispose;

  SNEntityPresenterClass *entityClass = SN_ENTITY_PRESENTER_CLASS(class);
  entityClass->create_item = sn_note_presenter_real_create_item;
  entityClass->create_iterator = sn_note_presenter_real_create_iterator;
}

static void
sn_note_presenter_init(SNNotePresenter *self)
{
  SNEntityPresenter *entities = SN_ENTITY_PRESENTER(self);
  self->_unsafe_store = sn_entity_presenter_get_store(entities);

  self->_folderChanged = sn_notify_connect(G_OBJECT(self->_unsafe_store),
					   kFolderChanged,
					   sn_note_presenter_new_data_handler,
					   self);
  self->_folderSelected = sn_notify_connect(G_OBJECT(self->_unsafe_store),
					    kFolderSelected,
					    sn_note_presenter_new_data_handler,
					    self);
}

SNNotePresenter *
sn_note_presenter_new(SNIEntityModel *model)
{
  SNNotePresenter *notes = g_object_new(SN_TYPE_NOTE_PRESENTER, NULL);
  SNEntityPresenter *entities = SN_ENTITY_PRESENTER(notes);

  sn_entity_presenter_assign_model(entities, model);
  notes->_unsafe_model = model;

  return notes;
}

void
sn_note_presenter_fetch(SNNotePresenter *self)
{
  sn_entity_presenter_fetch(SN_ENTITY_PRESENTER(self));
}

void
sn_note_presenter_move_note(SNNotePresenter *self,
			    guint64 id,
			    guint64 newFolderID)
{
  gboolean changed = sn_store_update_note(self->_unsafe_store, id, newFolderID);
  SN_ENTITY_PRESENTER_SEND_CHANGED(self, changed, id);
}

void
sn_note_presenter_add_note(SNNotePresenter *self)
{
  gboolean changed = sn_store_insert_note(self->_unsafe_store);
  SN_ENTITY_PRESENTER_SEND_CHANGED(self, changed, kItemInserted);
}

void
sn_note_presenter_delete_note(SNNotePresenter *self, guint64 id)
{
  gboolean changed = sn_store_delete_note(self->_unsafe_store, id);
  SN_ENTITY_PRESENTER_SEND_CHANGED(self, changed, id);
}

void
sn_note_presenter_select_note(SNNotePresenter *self, guint64 id)
{
  gboolean changed = sn_store_select_note(self->_unsafe_store, id);
  SN_ENTITY_PRESENTER_SEND_CHANGED(self, changed, id);
}

static void
sn_note_presenter_new_data_handler(GObject *sender,
				   GParamSpec *pspec,
				   gpointer user_data)
{
  SNNotePresenter *self = SN_NOTE_PRESENTER(user_data);
  sn_note_presenter_fetch(self);
}

static SNEntityIterator *
sn_note_presenter_real_create_iterator(SNEntityPresenter *self)
{
  SNNotePresenter *note = SN_NOTE_PRESENTER(self);

  SNNoteIterator *itr = NULL;
  itr = sn_store_create_note_iterator(note->_unsafe_store);

  return SN_ENTITY_ITERATOR(itr);
}

static SNObject *
sn_note_presenter_real_create_item(SNEntityPresenter *self,
				   SNEntityIterator *itr)
{
  SNNoteIterator *noteItr = SN_NOTE_ITERATOR(itr);
  SNNote *note = sn_note_new();

  guint64 folderID = sn_note_iterator_item_folder_id(noteItr);
  gchar *content = sn_note_iterator_item_content(noteItr);
  GDateTime *lastEdited = sn_note_iterator_create_item_last_edited(noteItr);
  gboolean selected = sn_note_iterator_item_selected(noteItr);

  sn_note_assign_folder_id(note, folderID);
  sn_note_copy_content(note, content);
  sn_note_copy_last_edited(note, lastEdited);
  sn_note_assign_selected(note, selected);

  g_date_time_unref(lastEdited);

  return SN_OBJECT(note);
}
