//
//  selected_list_model.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/31/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "selected_list_model.h"
#include "model_utility.h"
#include "ientity_model.h"
#include <stdint.h>


enum {
      PROP_SELECTED_OBJECT_ID = 1,
      N_PROPERTIES
};


static GParamSpec *objProperties[N_PROPERTIES] = { NULL, };


static void
sn_imodel_interface_init(SNIModelInterface *iface);

static void
sn_ientity_model_interface_init(SNIEntityModelInterface *iface);


G_DEFINE_TYPE_WITH_CODE(SNSelectedListModel,
			sn_selected_list_model,
			SN_TYPE_LIST_MODEL,
			G_IMPLEMENT_INTERFACE(SN_TYPE_IMODEL,
					      sn_imodel_interface_init)
			G_IMPLEMENT_INTERFACE(SN_TYPE_IENTITY_MODEL,
					      sn_ientity_model_interface_init))


static SNObject *
sn_selected_list_model_find_object_with_condition(SNSelectedListModel *object,
						  gboolean
						  (*condition)(SNObject *item));


static SNObject *
sn_selected_list_model_real_get_object(SNSelectedListModel *object,
				       guint position);

static SNObject **
sn_selected_list_model_real_copy_items(SNSelectedListModel *object,
				       guint *out_count);

static void
sn_selected_list_model_real_append_object(SNSelectedListModel *object,
					  SNObject *item);

static void
sn_selected_list_model_real_delete_object(SNSelectedListModel *object,
					  SNObject *item);

static void
sn_selected_list_model_real_changed(SNBaseModel *object);


static void
sn_ientity_model_real_changed(SNIEntityModel *self, guint64 id);

static void
sn_ientity_model_real_selected(SNIEntityModel *self, guint64 id);

static void
sn_imodel_real_new_data(SNIModel *self, gpointer data);

static void
sn_imodel_real_error(SNIModel *self, SNError error, gpointer data);


static void
sn_base_model_set_property(GObject *object,
			   guint property_id,
			   const GValue *value,
			   GParamSpec *pspec)
{
  SNSelectedListModel *self = SN_SELECTED_LIST_MODEL(object);
  guint64 objectID;

  switch (property_id)
    {
    case PROP_SELECTED_OBJECT_ID:
      objectID = g_value_get_uint64(value);
      SNObject *item = sn_selected_list_model_find_object(self, objectID);
      SNObject *previousSelected;
      previousSelected = sn_selected_list_model_find_selected_object(self);
      if (previousSelected)
	{
	  sn_object_assign_selected(previousSelected, FALSE);
	}

      if (item)
	{
	  sn_object_assign_selected(item, TRUE);
	}
      else
	{
	  sn_base_model_changed(SN_BASE_MODEL(object));
	}

      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
    }
}

static void
sn_base_model_get_property(GObject *object,
			   guint property_id,
			   GValue *value,
			   GParamSpec *pspec)
{
  SNSelectedListModel *self = SN_SELECTED_LIST_MODEL(object);

  guint64 objectID = 0;
  SNObject *selectedObject = NULL;

  switch (property_id)
    {
    case PROP_SELECTED_OBJECT_ID:
      selectedObject = sn_selected_list_model_find_selected_object(self);
      if (selectedObject)
	{
	  objectID = sn_object_get_id(selectedObject);
	}
      g_value_set_uint64(value, objectID);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
    }
}

static void
sn_imodel_interface_init(SNIModelInterface *iface)
{
  iface->new_data = sn_imodel_real_new_data;
  iface->error = sn_imodel_real_error;
}

static void
sn_ientity_model_interface_init(SNIEntityModelInterface *iface)
{
  iface->changed = sn_ientity_model_real_changed;
  iface->selected = sn_ientity_model_real_selected;
}

static void
sn_selected_list_model_class_init(SNSelectedListModelClass *klass)
{
  GObjectClass *objectClass = G_OBJECT_CLASS(klass);
  SNBaseModelClass *baseModelClass = SN_BASE_MODEL_CLASS(klass);

  klass->get_object = sn_selected_list_model_real_get_object;
  klass->copy_items = sn_selected_list_model_real_copy_items;
  klass->append_object = sn_selected_list_model_real_append_object;
  klass->delete_object = sn_selected_list_model_real_delete_object;

  baseModelClass->changed = sn_selected_list_model_real_changed;

  objectClass->set_property = sn_base_model_set_property;
  objectClass->get_property = sn_base_model_get_property;

  objProperties[PROP_SELECTED_OBJECT_ID] =
    g_param_spec_uint64(kSelectedObjectID,
			"Selected Object ID",
			"Selected Object ID.",
			0,
			UINT64_MAX,
			0  /* default value */,
			G_PARAM_READWRITE);

  g_object_class_install_properties(objectClass,
				    N_PROPERTIES,
				    objProperties);
}

static void
sn_selected_list_model_init(SNSelectedListModel *object)
{
  sn_base_model_load_data(SN_BASE_MODEL(object));
}

SNObject *
sn_selected_list_model_get_object(SNSelectedListModel *object, guint position)
{
  SNSelectedListModelClass *klass;
  SN_GET_CLASS_OR_RETURN_VAL(object,
			     &klass,
			     get_object,
			     SNSelectedListModel,
			     SN,
			     SELECTED_LIST_MODEL,
			     NULL);

  return klass->get_object(object, position);
}

SNObject **
sn_selected_list_model_copy_items(SNSelectedListModel *object,
				  guint *out_count)
{
  SNSelectedListModelClass *klass;
  SN_GET_CLASS_OR_RETURN_VAL(object,
			     &klass,
			     copy_items,
			     SNSelectedListModel,
			     SN,
			     SELECTED_LIST_MODEL,
			     NULL);

  return klass->copy_items(object, out_count);
}

void
sn_selected_list_model_append_object(SNSelectedListModel *object,
				     SNObject *item)
{
  SNSelectedListModelClass *klass;
  SN_GET_CLASS(object,
	       &klass,
	       append_object,
	       SNSelectedListModel,
	       SN,
	       SELECTED_LIST_MODEL);

  klass->append_object(object, item);
}

void
sn_selected_list_model_delete_object(SNSelectedListModel *object,
				     SNObject *item)
{
  SNSelectedListModelClass *klass;
  SN_GET_CLASS(object,
	       &klass,
	       delete_object,
	       SNSelectedListModel,
	       SN,
	       SELECTED_LIST_MODEL);

  klass->delete_object(object, item);
}

guint64
sn_selected_list_model_get_selected_object_id(SNSelectedListModel *object)
{
  guint64 value;
  g_object_get(G_OBJECT(object), kSelectedObjectID, &value, NULL);

  return value;
}

void
sn_selected_list_model_assign_selected_object_id(SNSelectedListModel *object,
						 guint64 objectID)
{
  g_object_set(G_OBJECT(object), kSelectedObjectID, objectID, NULL);
}

SNObject *
sn_selected_list_model_find_object(SNSelectedListModel *object,
				   guint64 objectID)
{
  gboolean condition (SNObject *item)
  {
    return sn_object_get_id(item) == objectID;
  }

  return sn_selected_list_model_find_object_with_condition(object,condition);
}

SNObject *
sn_selected_list_model_find_selected_object(SNSelectedListModel *object)
{
  gboolean condition (SNObject *item)
  {
    return sn_object_get_selected(item);
  }

  return sn_selected_list_model_find_object_with_condition(object, condition);
}

static SNObject *
sn_selected_list_model_find_object_with_condition(SNSelectedListModel *object,
						  gboolean
						  (*condition)(SNObject *item))
{
  GList *item = sn_list_model_get_item(SN_LIST_MODEL(object), 0);
  SNObject *foundObject = NULL;
  while (item)
    {
      SNObject *data = SN_OBJECT(item->data);
      if (condition(data))
	{
	  foundObject = data;
	  break;
	}
      item = item->next;
    }

  return foundObject;
}

static SNObject *
sn_selected_list_model_real_get_object(SNSelectedListModel *object,
				       guint position)
{
  GList *item = sn_list_model_get_item(SN_LIST_MODEL(object), position);
  if (item)
    {
      return SN_OBJECT(item->data);
    }

  return NULL;
}

static SNObject **
sn_selected_list_model_real_copy_items(SNSelectedListModel *object,
				       guint *out_count)
{
  void **items;
  SN_COPY_DATA(object, &items, out_count);
  return (SNObject **)items;
}

static void
sn_selected_list_model_real_append_object(SNSelectedListModel *object,
					  SNObject *item)
{
  sn_list_model_append(SN_LIST_MODEL(object), item);
}

static void
sn_selected_list_model_real_delete_object(SNSelectedListModel *object,
					  SNObject *item)
{
  sn_list_model_delete(SN_LIST_MODEL(object), item);
}

static void
sn_selected_list_model_real_changed(SNBaseModel *object)
{
  SNBaseModelClass *parent;
  parent = SN_BASE_MODEL_CLASS(sn_selected_list_model_parent_class);
  parent->changed(object);

  SNSelectedListModel *self = SN_SELECTED_LIST_MODEL(object);
  g_return_if_fail(self);
}

static void
sn_ientity_model_real_changed(SNIEntityModel *self, guint64 id)
{
  sn_base_model_changed(SN_BASE_MODEL(self));
}

static void
sn_ientity_model_real_selected(SNIEntityModel *self, guint64 id)
{
  SNSelectedListModel *list = SN_SELECTED_LIST_MODEL(self);
  sn_selected_list_model_assign_selected_object_id(list, id);
}

static void
sn_imodel_real_new_data(SNIModel *self, gpointer data)
{
  sn_list_model_assign_list(SN_LIST_MODEL(self), (GList *)data);
}

static void
sn_imodel_real_error(SNIModel *self, SNError error, gpointer data)
{
  sn_base_model_assign_error_code(SN_BASE_MODEL(self), error);
}
