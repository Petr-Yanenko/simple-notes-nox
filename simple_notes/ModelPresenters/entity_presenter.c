//
// entity_presenter.c
//


#include "entity_presenter.h"


typedef struct {
  SNIEntityModel *_unsafe_model;
  SNStore *_store;
} SNEntityPresenterPrivate;


G_DEFINE_TYPE_WITH_PRIVATE(SNEntityPresenter,
			   sn_entity_presenter,
			   G_TYPE_OBJECT)


static SNError kError = SNErrorEntityPresenter;


static SNEntityIterator *
sn_entity_presenter_create_iterator(SNEntityPresenter *self);

static SNObject *
sn_entity_presenter_create_item(SNEntityPresenter *self, SNEntityIterator *itr);


static void
sn_entity_presenter_dispose(GObject *self)
{
  SNEntityPresenter *presenter = SN_ENTITY_PRESENTER(self);
  SNEntityPresenterPrivate *prv;
  prv = sn_entity_presenter_get_instance_private(presenter);
  g_clear_object(&prv->_store);
  prv->_unsafe_model = NULL;

  G_OBJECT_CLASS(sn_entity_presenter_parent_class)->dispose(self);
}

static void
sn_entity_presenter_class_init(SNEntityPresenterClass *class)
{
  GObjectClass *gClass = G_OBJECT_CLASS(class);
  gClass->dispose = sn_entity_presenter_dispose;
}

static void
sn_entity_presenter_init(SNEntityPresenter *self)
{
  SNEntityPresenterPrivate *prv;
  prv = sn_entity_presenter_get_instance_private(self);

  prv->_store = g_object_ref(sn_store_get_instance());
}

SNIEntityModel *
sn_entity_presenter_get_model(SNEntityPresenter *self)
{
  SNEntityPresenterPrivate *prv;
  prv = sn_entity_presenter_get_instance_private(self);

  return prv->_unsafe_model;
}

void
sn_entity_presenter_assign_model(SNEntityPresenter *self, SNIEntityModel *model)
{
  SNEntityPresenterPrivate *prv;
  prv = sn_entity_presenter_get_instance_private(self);
  prv->_unsafe_model = model;
}

SNStore *
sn_entity_presenter_get_store(SNEntityPresenter *self)
{
  SNEntityPresenterPrivate *prv;
  prv = sn_entity_presenter_get_instance_private(self);

  return prv->_store;
}

void
sn_entity_presenter_fetch(SNEntityPresenter *self)
{
  SNEntityPresenterPrivate *prv;
  prv = sn_entity_presenter_get_instance_private(self);
  SNEntityIterator *itr = sn_entity_presenter_create_iterator(self);
  SNDataIterator *data = SN_DATA_ITERATOR(itr);
  GList *items = NULL;

  SNIteratorResult result = sn_data_iterator_next(data);
  while (result == SNIteratorResultRow)
    {
      SNObject *item = sn_entity_presenter_create_item(self, itr);
      guint64 id = sn_entity_iterator_item_id(itr);
      sn_object_assign_id(item, id);
      items = g_list_append(items, item);

      result = sn_data_iterator_next(data);
    }

  gboolean success = result == SNIteratorResultDone;
  SNError error = SNErrorEntityFetch;
  if (!success)
    {
      if (items) g_list_free_full(g_steal_pointer(&items), g_object_unref);
      sn_imodel_error(SN_IMODEL(prv->_unsafe_model), error, NULL);
    }
  else
    {
      sn_imodel_new_data(SN_IMODEL(prv->_unsafe_model), (gpointer)items);
    }

  g_object_unref(itr);
  SN_RETURN_IF_FAIL(success, &error);
}

void
sn_entity_presenter_send_changed(SNEntityPresenter *self,
				 gboolean changed,
				 guint64 id,
				 SNError error)
{
  SNEntityPresenterPrivate *prv = NULL;
  prv = sn_entity_presenter_get_instance_private(self);
  if (changed)
    {
      sn_ientity_model_changed(prv->_unsafe_model, id);
    }
  else
    {
      sn_imodel_error(SN_IMODEL(prv->_unsafe_model), error, NULL);
    }

  SN_RETURN_IF_FAIL(changed, &error);
}

static SNEntityIterator *
sn_entity_presenter_create_iterator(SNEntityPresenter *self)
{
  SNEntityPresenterClass *class = NULL;
  SN_GET_CLASS_OR_RETURN_VAL(self,
			     &class,
			     create_iterator,
			     SNEntityPresenter,
			     SN,
			     ENTITY_PRESENTER,
			     NULL);

  return class->create_iterator(self);
}

static SNObject *
sn_entity_presenter_create_item(SNEntityPresenter *self, SNEntityIterator *itr)
{
  SNEntityPresenterClass *class = NULL;
  SN_GET_CLASS_OR_RETURN_VAL(self,
			     &class,
			     create_item,
			     SNEntityPresenter,
			     SN,
			     ENTITY_PRESENTER,
			     NULL);

  return class->create_item(self, itr);
}
