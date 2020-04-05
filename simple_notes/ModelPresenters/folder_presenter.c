//
// folder_presenter.c
//


#include "folder_presenter.h"


struct _SNFolderPresenter {
  SNEntityPresenter _parent;

  SNStore *_unsafe_store;
};


G_DEFINE_TYPE(SNFolderPresenter, sn_folder_presenter, SNEntityPresenter)


static SNError kError = SNErrorFolderPresenter;


static void
sn_folder_presenter_class_init(SNFolderPresenterClass *class)
{
}

static void
sn_folder_presenter_init(SNFolderPresenter *self)
{
  self->_unsafe_store = sn_store_get_instance();
}

SNFolderPresenter *
sn_folder_presenter_new(SNIModel *model)
{
  SNFolderPresenter *folders = g_object_new(SN_TYPE_FOLDER_PRESENTER, NULL);
  SNEntityPresenter *entities = SN_ENTITY_PRESENTER(folders);

  sn_entity_presenter_assign_model(entities, model);

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

  GList *items = sn_entity_presenter_create_items(super, entities, create_item);
}

