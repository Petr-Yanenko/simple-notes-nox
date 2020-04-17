//
//  object.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/15/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//


#include "object.h"


typedef struct {
  guint64 _id;
  gboolean _selected;
} SNObjectPrivate;


G_DEFINE_TYPE_WITH_PRIVATE(SNObject,
			   sn_object,
			   G_TYPE_OBJECT);


static GString *
sn_object_real_create_description(SNObject *object)
{
  SNObjectPrivate *objectPrivate = sn_object_get_instance_private(object);
  GString *buff = g_string_sized_new(50);
  g_string_printf(buff,
		  "\n\n<%p> ID: %llu\nSelected: %i\n",
		  object,
		  objectPrivate->_id,
		  objectPrivate->_selected);

  return buff;
}

static void
sn_object_class_init(SNObjectClass *classObject)
{
  classObject->create_description = sn_object_real_create_description;
}

static void
sn_object_init(SimpleNotesObject *object)
{
  SNObjectPrivate *objectPrivate = sn_object_get_instance_private(object);
  objectPrivate->_id = 0;
  objectPrivate->_selected = FALSE;
}

SNObject *
sn_object_new(void)
{
  return g_object_new(SN_TYPE_OBJECT, NULL);
}

guint64
sn_object_get_id(SNObject *object)
{
  SNObjectPrivate *objectPrivate = sn_object_get_instance_private(object);
  return objectPrivate->_id;
}

void
sn_object_assign_id(SNObject *object, guint64 id)
{
  SNObjectPrivate *objectPrivate = sn_object_get_instance_private(object);
  objectPrivate->_id = id;
}

gboolean
sn_object_get_selected(SNObject *object)
{
  SNObjectPrivate *objectPrivate = sn_object_get_instance_private(object);
  return objectPrivate->_selected;
}

void
sn_object_assign_selected(SNObject *object, gboolean selected)
{
  SNObjectPrivate *objectPrivate = sn_object_get_instance_private(object);
  objectPrivate->_selected = selected;
}

GString *
sn_object_create_description(SNObject *object)
{
  SNObjectClass *klass;
  SN_CHECK_VIRTUAL_CLASS_FUNC_WITH_RETURN_VAL(object,
					      &klass,
					      create_description,
					      SNObject,
					      SN,
					      OBJECT,
					      NULL);
  return klass->create_description(object);
}
