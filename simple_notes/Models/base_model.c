//
//  base_model.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/24/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "base_model.h"


gchar *const kLoading = "loading";
gchar *const kNewData = "new-data";
gchar *const kErrorCode = "error-code";


enum {
      PROP_LOADING = 1,
      PROP_NEW_DATA,
      PROP_ERROR_CODE,
      N_PROPERTIES
};


typedef struct {
  gboolean _loading;
  gboolean _newData;
  SNError _errorCode;
} SNBaseModelPrivate;


static GParamSpec *objProperties[N_PROPERTIES] = { NULL, };


G_DEFINE_TYPE_WITH_PRIVATE(SNBaseModel, sn_base_model, G_TYPE_OBJECT)


static void
sn_base_model_load(SNBaseModel *object);

static void
sn_base_model_real_load_data(SNBaseModel *object);

static void
sn_base_model_real_load(SNBaseModel *object);

static void
sn_base_model_real_reset(SNBaseModel *object);

static void
sn_base_model_real_cancel(SNBaseModel *object);

static void
sn_base_model_real_changed(SNBaseModel *object);


static void
sn_base_model_set_property(GObject *object,
			   guint property_id,
			   const GValue *value,
			   GParamSpec *pspec)
{
  SNBaseModel *self = SN_BASE_MODEL(object);
  SNBaseModelPrivate *modelPrivate = sn_base_model_get_instance_private(self);

  switch (property_id)
    {
    case PROP_LOADING:
      modelPrivate->_loading = g_value_get_boolean(value);
      break;

    case PROP_NEW_DATA:
      modelPrivate->_newData = g_value_get_boolean(value);
      break;

    case PROP_ERROR_CODE:
      modelPrivate->_errorCode = g_value_get_long(value);
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
			   GValue     *value,
			   GParamSpec *pspec)
{
  SNBaseModel *self = SN_BASE_MODEL(object);
  SNBaseModelPrivate *modelPrivate = sn_base_model_get_instance_private(self);

  switch (property_id)
    {
    case PROP_LOADING:
      g_value_set_boolean(value, modelPrivate->_loading);
      break;

    case PROP_NEW_DATA:
      g_value_set_boolean(value, modelPrivate->_newData);
      break;

    case PROP_ERROR_CODE:
      g_value_set_long(value, modelPrivate->_errorCode);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
    }
}

static void
sn_base_model_dispose(GObject *object)
{
  G_OBJECT_CLASS(sn_base_model_parent_class)->dispose(object);
}

static void
sn_base_model_class_init(SNBaseModelClass *klass)
{

  g_type_add_instance_private(SN_TYPE_BASE_MODEL, sizeof(SNBaseModelPrivate));

  GObjectClass *objectClass = G_OBJECT_CLASS(klass);
  objectClass->dispose = sn_base_model_dispose;

  klass->load_data = sn_base_model_real_load_data;
  klass->load = sn_base_model_real_load;
  klass->changed = sn_base_model_real_changed;
  klass->reset = sn_base_model_real_reset;
  klass->cancel = sn_base_model_real_cancel;

  objectClass->set_property = sn_base_model_set_property;
  objectClass->get_property = sn_base_model_get_property;

  objProperties[PROP_LOADING] = g_param_spec_boolean(kLoading,
						     "Loading",
						     "Loading flag.",
						     FALSE  /* default value */,
						     G_PARAM_READWRITE);

  objProperties[PROP_NEW_DATA] = g_param_spec_boolean(kNewData,
						      "New Data",
						      "New data flag.",
						      FALSE,
						      G_PARAM_READWRITE);

  gchar *description = "Error reason of loading data.";
  objProperties[PROP_ERROR_CODE] = g_param_spec_long(kErrorCode,
						     "Error Reason",
						     description,
						     G_MINLONG,
						     G_MAXLONG,
						     -1,
						     G_PARAM_READWRITE);

  g_object_class_install_properties(objectClass,
				    N_PROPERTIES,
				    objProperties);
}

static void
sn_base_model_init(SNBaseModel *object)
{
  SNBaseModelPrivate *modelPrivate = sn_base_model_get_instance_private(object);
  modelPrivate->_loading = FALSE;
  modelPrivate->_newData = FALSE;
  modelPrivate->_errorCode = -1;
}

void
sn_base_model_load_data(SNBaseModel *object)
{
  SNBaseModelClass *klass;
  SN_GET_CLASS(object, &klass, load_data, SNBaseModel, SN, BASE_MODEL);
  klass->load_data(object);
}

void
sn_base_model_reset(SNBaseModel *object)
{
  SNBaseModelClass *klass;
  SN_GET_CLASS(object, &klass, reset, SNBaseModel, SN, BASE_MODEL);
  klass->reset(object);
}

void
sn_base_model_cancel(SNBaseModel *object)
{
  SNBaseModelClass *klass;
  SN_GET_CLASS(object, &klass, cancel, SNBaseModel, SN, BASE_MODEL);
  klass->cancel(object);
}

gboolean
sn_base_model_get_loading(SNBaseModel *object)
{
  gboolean value;
  g_object_get(G_OBJECT(object), kLoading, &value, NULL);
  return value;
}

void
sn_base_model_assign_loading(SNBaseModel *object, gboolean loading)
{
  g_object_set(G_OBJECT(object), kLoading, loading, NULL);
}

gboolean
sn_base_model_get_new_data(SNBaseModel *object)
{
  gboolean value;
  g_object_get(G_OBJECT(object), kNewData, &value, NULL);
  return value;
}

void
sn_base_model_assign_new_data(SNBaseModel *object, gboolean newData)
{
  g_object_set(G_OBJECT(object), kNewData, newData, NULL);
}

SNError
sn_base_model_get_error_code(SNBaseModel *object)
{
  SNError value;
  g_object_get(G_OBJECT(object), kErrorCode, &value, NULL);
  return value;
}

void
sn_base_model_assign_error_code(SNBaseModel *object, SNError errorCode)
{
  g_object_set(G_OBJECT(object), kErrorCode, errorCode, NULL);
}

void
sn_base_model_changed(SNBaseModel *object)
{
  SNBaseModelClass *klass;

  SN_GET_CLASS(object, &klass, changed, SNBaseModel, SN, BASE_MODEL);

  klass->changed(object);
}

static void
sn_base_model_load(SNBaseModel *object)
{
  SNBaseModelClass *klass;

  SN_GET_CLASS(object, &klass, load, SNBaseModel, SN, BASE_MODEL);

  klass->load(object);
}

static void
sn_base_model_real_load_data(SNBaseModel *object)
{
  sn_base_model_assign_loading(object, TRUE);
  sn_base_model_load(object);
  sn_base_model_assign_loading(object, FALSE);
}

static void
sn_base_model_real_load(SNBaseModel *object)
{
}

static void
sn_base_model_real_changed(SNBaseModel *object)
{
}

static void
sn_base_model_real_reset(SNBaseModel *object)
{
}

static void
sn_base_model_real_cancel(SNBaseModel *object)
{
}
