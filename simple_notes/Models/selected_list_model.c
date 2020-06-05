//
//  selected_list_model.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/31/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "selected_list_model.h"
#include "model_utility.h"
#include "stdint.h"


enum {
      PROP_SELECTED_OBJECT_ID = 1,
      N_PROPERTIES
};


static GParamSpec *objProperties[N_PROPERTIES] = { NULL, };


G_DEFINE_TYPE(SNSelectedListModel, sn_selected_list_model, SN_TYPE_LIST_MODEL)


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


static SNObject *
sn_selected_list_model_find_object_with_condition(SNSelectedListModel *object,
						  gboolean
						  (*condition)(SNObject *item));

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
      if (item)
	{
	  SNObject *previousSelected;
	  previousSelected = sn_selected_list_model_find_selected_object(self);
	  if (previousSelected)
	    {
	      sn_object_assign_selected(previousSelected, FALSE);
	    }
	  sn_object_assign_selected(item, TRUE);
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
