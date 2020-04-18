//
// folder_presenter.c
//


#include "folder_presenter.h"
#include "ientity_model.h"
#include <stdio.h>
#include "folder.h"


struct _SNFolderPresenter {
  SNEntityPresenter _parent;

  SNStore *_unsafe_store;
  SNIEntityModel *_unsafe_model;
  gulong _noteChanged;
};


G_DEFINE_TYPE(SNFolderPresenter, sn_folder_presenter, SN_TYPE_ENTITY_PRESENTER)


static void
sn_folder_presenter_send_changed(SNFolderPresenter *self,
				 gboolean changed,
				 guint64 id);

static void
sn_folder_presenter_new_data_handler(GObject *sender,
				     GParamSpec *pspec,
				     gpointer user_data);


static SNError kError = SNErrorFolderPresenter;


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
}

static void
sn_folder_presenter_init(SNFolderPresenter *self)
{
  SNEntityPresenter *entities = SN_ENTITY_PRESENTER(self);
  self->_unsafe_store = sn_entity_presenter_get_store(entities);

  gchar *signalName = "notify::";
  gchar *signalDetail = kNoteChanged;
  glong nameLen = strlen(signalName);
  glong detailLen = strlen(signalDetail);
  glong const fullLen = nameLen + detailLen +1;
  gchar buff[fullLen];
  sprintf(buff, "%s%s", signalName, signalDetail);

  void (*callback)(GObject *, GParamSpec *, gpointer);
  callback = sn_folder_presenter_new_data_handler;
  gulong noteChanged = g_signal_connect(self->_unsafe_store,
					buff,
					G_CALLBACK(callback),
					self);

  self->_noteChanged = noteChanged;
}

SNFolderPresenter *
sn_folder_presenter_new(SNIEntityModel *model)
{
  SNFolderPresenter *folders = g_object_new(SN_TYPE_FOLDER_PRESENTER, NULL);
  SNEntityPresenter *entities = SN_ENTITY_PRESENTER(folders);

  sn_entity_presenter_assign_model(entities, SN_IMODEL(model));
  folders->_unsafe_model = model;

  return folders;
}

void
sn_folder_presenter_fetch(SNFolderPresenter *self)
{
  SNFolderIterator *itr = sn_store_create_folder_iterator(self->_unsafe_store);
  SNEntityIterator *entities = SN_ENTITY_ITERATOR(itr);
  SNEntityPresenter *super = SN_ENTITY_PRESENTER(self);

  SNObject *create_item(void)
  {
    SNFolder *folder = sn_folder_new();

    gchar *title = sn_folder_iterator_item_title(itr);
    glong count = sn_folder_iterator_item_count(itr);
    gboolean selected = sn_folder_iterator_item_selected(itr);

    sn_folder_copy_title(folder, title);
    sn_folder_assign_count(folder, count);
    sn_folder_assign_selected(folder, selected);

    return SN_OBJECT(folder);
  }

  sn_entity_presenter_fetch(super, entities, create_item);

  g_object_unref(itr);
}

void
sn_folder_presenter_change_title(SNFolderPresenter *self,
				 guint64 id,
				 gchar *title)
{
  gboolean changed = sn_store_update_folder(self->_unsafe_store, id, title);
  sn_folder_presenter_send_changed(self, changed, id);
}

void
sn_folder_presenter_add_folder(SNFolderPresenter *self, gchar *title)
{
  gboolean changed = sn_store_insert_folder(self->_unsafe_store, title);
  sn_folder_presenter_send_changed(self, changed, kItemInserted);
}

void
sn_folder_presenter_delete_folder(SNFolderPresenter *self, guint64 id)
{
  gboolean changed = sn_store_delete_folder(self->_unsafe_store, id);
  sn_folder_presenter_send_changed(self, changed, id);
}

void
sn_folder_presenter_select_folder(SNFolderPresenter *self, guint64 id)
{
  gboolean changed = sn_store_select_folder(self->_unsafe_store, id);
  sn_folder_presenter_send_changed(self, changed, id);
}

static void
sn_folder_presenter_send_changed(SNFolderPresenter *self,
				 gboolean changed,
				 guint64 id)
{
  SNError error = kError;
  if (changed)
    {
      sn_ientity_model_changed(self->_unsafe_model, id);
    }
  else
    {
      sn_imodel_error(SN_IMODEL(self->_unsafe_model), error, &id);
    }

  SN_RETURN_IF_FAIL(changed, &error);
}

static void
sn_folder_presenter_new_data_handler(GObject *sender,
				     GParamSpec *pspec,
				     gpointer user_data)
{
  SNFolderPresenter *self = SN_FOLDER_PRESENTER(user_data);
  sn_folder_presenter_fetch(self);
}
