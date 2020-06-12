//
// folder_presenter.c
//


#include "folder_presenter.h"
#include "folder.h"


struct _SNFolderPresenter {
  SNEntityPresenter _parent;

  SNStore *_unsafe_store;
  SNIEntityModel *_unsafe_model;
  gulong _noteChanged;
};


G_DEFINE_TYPE(SNFolderPresenter, sn_folder_presenter, SN_TYPE_ENTITY_PRESENTER)


static SNError kError = SNErrorFolderPresenter;


static void
sn_folder_presenter_send_changed(SNFolderPresenter *self,
				 gboolean changed,
				 guint64 id);

static void
sn_folder_presenter_new_data_handler(GObject *sender,
				     GParamSpec *pspec,
				     gpointer user_data);

static SNEntityIterator *
sn_folder_presenter_real_create_iterator(SNEntityPresenter *self);

static SNObject *
sn_folder_presenter_real_create_item(SNEntityPresenter *self,
				     SNEntityIterator *itr);


static void
sn_folder_presenter_dispose(GObject *self)
{
  SNFolderPresenter *pres = SN_FOLDER_PRESENTER(self);
  g_signal_handler_disconnect(pres->_unsafe_store, pres->_noteChanged);

  G_OBJECT_CLASS(sn_folder_presenter_parent_class)->dispose(self);
}

static void
sn_folder_presenter_class_init(SNFolderPresenterClass *class)
{
  GObjectClass *gClass = G_OBJECT_CLASS(class);
  gClass->dispose = sn_folder_presenter_dispose;

  SNEntityPresenterClass *entityClass = SN_ENTITY_PRESENTER_CLASS(class);
  entityClass->create_iterator = sn_folder_presenter_real_create_iterator;
  entityClass->create_item = sn_folder_presenter_real_create_item;
}

static void
sn_folder_presenter_init(SNFolderPresenter *self)
{
  SNEntityPresenter *entities = SN_ENTITY_PRESENTER(self);
  self->_unsafe_store = sn_entity_presenter_get_store(entities);

  self->_noteChanged = sn_notify_connect(G_OBJECT(self->_unsafe_store),
					 kNoteChanged,
					 sn_folder_presenter_new_data_handler,
					 self);
}

SNFolderPresenter *
sn_folder_presenter_new(SNIEntityModel *model)
{
  SNFolderPresenter *folders = g_object_new(SN_TYPE_FOLDER_PRESENTER, NULL);
  SNEntityPresenter *entities = SN_ENTITY_PRESENTER(folders);

  sn_entity_presenter_assign_model(entities, model);
  folders->_unsafe_model = model;

  return folders;
}

void
sn_folder_presenter_fetch(SNFolderPresenter *self)
{
  sn_entity_presenter_fetch(SN_ENTITY_PRESENTER(self));
}

void
sn_folder_presenter_change_title(SNFolderPresenter *self,
				 guint64 id,
				 gchar *title)
{
  gboolean changed = sn_store_update_folder(self->_unsafe_store, id, title);
  SN_ENTITY_PRESENTER_SEND_CHANGED(self, changed, id);
}

void
sn_folder_presenter_add_folder(SNFolderPresenter *self, gchar *title)
{
  gboolean changed = sn_store_insert_folder(self->_unsafe_store, title);
  SN_ENTITY_PRESENTER_SEND_CHANGED(self, changed, kItemInserted);
}

void
sn_folder_presenter_delete_folder(SNFolderPresenter *self, guint64 id)
{
  gboolean changed = sn_store_delete_folder(self->_unsafe_store, id);
  SN_ENTITY_PRESENTER_SEND_CHANGED(self, changed, id);
}

void
sn_folder_presenter_select_folder(SNFolderPresenter *self, guint64 id)
{
  gboolean changed = sn_store_select_folder(self->_unsafe_store, id);
  SN_ENTITY_PRESENTER_SEND_CHANGED(self, changed, id);
}

static void
sn_folder_presenter_new_data_handler(GObject *sender,
				     GParamSpec *pspec,
				     gpointer user_data)
{
  SNFolderPresenter *self = SN_FOLDER_PRESENTER(user_data);
  sn_folder_presenter_fetch(self);
}

static SNEntityIterator *
sn_folder_presenter_real_create_iterator(SNEntityPresenter *self)
{
  SNFolderPresenter *folder = SN_FOLDER_PRESENTER(self);

  SNFolderIterator *itr = NULL;
  itr = sn_store_create_folder_iterator(folder->_unsafe_store);

  return SN_ENTITY_ITERATOR(itr);
}

static SNObject *
sn_folder_presenter_real_create_item(SNEntityPresenter *self,
				     SNEntityIterator *itr)
{
  SNFolderIterator *folderItr = SN_FOLDER_ITERATOR(itr);
  SNFolder *folder = sn_folder_new();

  gchar *title = sn_folder_iterator_item_title(folderItr);
  glong count = sn_folder_iterator_item_count(folderItr);
  gboolean selected = sn_folder_iterator_item_selected(folderItr);

  sn_folder_copy_title(folder, title);
  sn_folder_assign_count(folder, count);
  sn_folder_assign_selected(folder, selected);

  return SN_OBJECT(folder);
}
