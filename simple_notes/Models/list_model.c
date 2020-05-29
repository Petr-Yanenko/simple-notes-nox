//
//  list_model.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/25/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "list_model.h"


typedef struct {
  GList *_list;
} SNListModelPrivate;


G_DEFINE_TYPE_WITH_PRIVATE(SNListModel, sn_list_model, SN_TYPE_BASE_MODEL)


static GList *
sn_list_model_real_get_item(SNListModel *object, guint position);

static void
sn_list_model_real_append(SNListModel *object, gpointer item);

static void
sn_list_model_real_delete(SNListModel *object, gpointer item);

static void
sn_list_model_real_load(SNBaseModel *object);

static void
sn_list_model_real_reset(SNBaseModel *object);

static void
sn_list_model_real_changed(SNBaseModel *object);

static GList *
sn_list_model_real_create_items(SNListModel *object);

static void
sn_list_model_delete_list(SNListModelPrivate *modelPrivate);

static GList *
sn_list_model_create_items(SNListModel *object);


static void
sn_list_model_dispose(GObject *object)
{
  SNListModel *self = SN_LIST_MODEL(object);
  SNListModelPrivate *modelPrivate = sn_list_model_get_instance_private(self);
  sn_list_model_delete_list(modelPrivate);

  G_OBJECT_CLASS(sn_list_model_parent_class)->dispose(object);
}

static void
sn_list_model_class_init(SNListModelClass *klass)
{
  g_type_add_instance_private(SN_TYPE_LIST_MODEL, sizeof(SNListModelPrivate));

  GObjectClass *objectClass = G_OBJECT_CLASS(klass);
  objectClass->dispose = sn_list_model_dispose;

  SNBaseModelClass *baseModelClass = SN_BASE_MODEL_CLASS(klass);
  baseModelClass->load = sn_list_model_real_load;
  baseModelClass->reset = sn_list_model_real_reset;
  baseModelClass->changed = sn_list_model_real_changed;

  klass->get_item = sn_list_model_real_get_item;
  klass->append = sn_list_model_real_append;
  klass->delete =sn_list_model_real_delete;
  klass->create_items = sn_list_model_real_create_items;
}

static void
sn_list_model_init(SNListModel *object)
{
  SNListModelPrivate *modelPrivate = sn_list_model_get_instance_private(object);
  modelPrivate->_list = NULL;
}

void
sn_list_model_copy_list(SNListModel *object, GList *list)
{
  SNListModelPrivate *modelPrivate = sn_list_model_get_instance_private(object);
  sn_list_model_delete_list(modelPrivate);
  modelPrivate->_list = g_list_copy_deep(list, (GCopyFunc)g_object_ref, NULL);
}

GList *
sn_list_model_get_item(SNListModel *object, guint position)
{
  SNListModelClass *klass;
  SN_GET_CLASS_OR_RETURN_VAL(object,
			     &klass,
			     get_item,
			     SNListModel,
			     SN,
			     LIST_MODEL,
			     NULL);

  return klass->get_item(object, position);
}

GPtrArray *
sn_list_model_copy_data(SNListModel *object, guint fromPosition)
{
  GPtrArray *data = g_ptr_array_new();
  g_ptr_array_set_free_func(data, g_object_unref);
  GList *list = sn_list_model_get_item(object, fromPosition);
  for (GList *l = list; l != NULL; l = l->next) {
    g_ptr_array_add(data, g_object_ref(l->data));
  }

  return data;
}

void
sn_list_model_append(SNListModel *object, gpointer item)
{
  SNListModelClass *klass;
  SN_GET_CLASS(object, &klass, append, SNListModel, SN, LIST_MODEL);
  klass->append(object, item);
}

void
sn_list_model_delete(SNListModel *object, gpointer item)
{
  SNListModelClass *klass;
  SN_GET_CLASS(object, &klass, delete, SNListModel, SN, LIST_MODEL);
  klass->delete(object, item);
}

glong
sn_list_model_get_count(SNListModel *object)
{
  SNListModelPrivate *modelPrivate = sn_list_model_get_instance_private(object);
  return g_list_length(modelPrivate->_list);
}

static GList *
sn_list_model_create_items(SNListModel *object)
{
  SNListModelClass *klass;
  SN_GET_CLASS_OR_RETURN_VAL(object,
			     &klass,
			     create_items,
			     SNListModel,
			     SN,
			     LIST_MODEL,
			     NULL);

  return klass->create_items(object);
}

static GList *
sn_list_model_real_get_item(SNListModel *object, guint position)
{
  SNListModelPrivate *modelPrivate = sn_list_model_get_instance_private(object);
  return g_list_nth(modelPrivate->_list, position);
}

static void
sn_list_model_real_append(SNListModel *object, gpointer item)
{
  SNListModelPrivate *modelPrivate = sn_list_model_get_instance_private(object);
  modelPrivate->_list = g_list_append(modelPrivate->_list, g_object_ref(item));
}

static void
sn_list_model_real_delete(SNListModel *object, gpointer item)
{
  SNListModelPrivate *modelPrivate = sn_list_model_get_instance_private(object);
  modelPrivate->_list = g_list_remove(modelPrivate->_list, item);
  g_object_unref(item);
}

static void
sn_list_model_delete_list(SNListModelPrivate *modelPrivate)
{
  if (modelPrivate->_list) {
    g_list_free_full(modelPrivate->_list, g_object_unref);
    modelPrivate->_list = NULL;
  }
}

static void
sn_list_model_real_load(SNBaseModel *object)
{
  SNListModel *self = SN_LIST_MODEL(object);
  g_return_if_fail(self);

  GList *items = sn_list_model_create_items(self);
  sn_list_model_copy_list(SN_LIST_MODEL(object), items);
  g_list_free_full(items, g_object_unref);
}

static void
sn_list_model_real_reset(SNBaseModel *object)
{
  SNListModel *listModel = SN_LIST_MODEL(object);
  g_return_if_fail(listModel);

  sn_list_model_copy_list(listModel, NULL);
}

static void
sn_list_model_real_changed(SNBaseModel *object)
{
  SN_BASE_MODEL_CLASS(sn_list_model_parent_class)->changed(object);

  SNListModel *self = SN_LIST_MODEL(object);
  g_return_if_fail(self);

  sn_base_model_reset(object);
  sn_base_model_load_data(object);
}

static GList *
sn_list_model_real_create_items(SNListModel *object)
{
  return NULL;
}
