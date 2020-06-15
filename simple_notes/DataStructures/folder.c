//
//  folder.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/15/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//


#include "folder.h"
#include "light_folder.h"


struct _SNFolder {
  SNObject _parent;

  GString *_title;
  glong _count;
};


static void
sn_light_folder_interface_init(SNLightFolderInterface *iface);


G_DEFINE_TYPE_WITH_CODE(SNFolder, sn_folder, SN_TYPE_OBJECT,
			G_IMPLEMENT_INTERFACE(SN_TYPE_LIGHT_FOLDER,
					      sn_light_folder_interface_init))


static GString *
sn_light_folder_real_create_description(SNLightFolder *object);

static guint64
sn_light_folder_real_get_id(SNLightFolder *object);

static gboolean
sn_light_folder_real_get_selected(SNLightFolder *object);

static GString *
sn_light_folder_real_get_copy_title(SNLightFolder *object);

static glong
sn_light_folder_real_get_count(SNLightFolder *object);


static GString *
sn_folder_real_create_description(SNObject *object)
{
  SNFolder *folder = SN_FOLDER(object);
  SNObjectClass *parentClass = SN_OBJECT_CLASS(sn_folder_parent_class);
  GString *description = parentClass->create_description(object);
  sn_print_ustring(folder->_title, description, "Title\0");
  g_string_append_printf(description, "\nCount: %ld\n", folder->_count);

  return description;
}

static void
sn_folder_dispose(GObject *object)
{
  SNFolder *folder = SN_FOLDER(object);
  if (folder->_title)
    {
      g_string_free(folder->_title, TRUE);
    }

  G_OBJECT_CLASS(sn_folder_parent_class)->dispose(object);
}

static void
sn_light_folder_interface_init(SNLightFolderInterface *iface)
{
  iface->create_description = sn_light_folder_real_create_description;
  iface->get_id = sn_light_folder_real_get_id;
  iface->get_selected = sn_light_folder_real_get_selected;
  iface->get_copy_title = sn_light_folder_real_get_copy_title;
  iface->get_count = sn_light_folder_real_get_count;
}

static void
sn_folder_class_init(SNFolderClass *classObject)
{
  GObjectClass *gClassObject = G_OBJECT_CLASS(classObject);
  SNObjectClass *parent = SN_OBJECT_CLASS(classObject);
  gClassObject->dispose = sn_folder_dispose;
  parent->create_description = sn_folder_real_create_description;
}

static void
sn_folder_init(SNFolder *object)
{
  object->_title = g_string_new(NULL);
  object->_count = 0;
}

SNFolder *sn_folder_new(void)
{
  return g_object_new(SN_TYPE_FOLDER, NULL);
}

guint64
sn_folder_get_id(SNFolder *object)
{
  return sn_object_get_id(SN_OBJECT(object));
}

void
sn_folder_assign_id(SNFolder *object, guint64 id)
{
  sn_object_assign_id(SN_OBJECT(object), id);
}

gboolean
sn_folder_get_selected(SNFolder *object)
{
  return sn_object_get_selected(SN_OBJECT(object));
}

void
sn_folder_assign_selected(SNFolder *object, gboolean selected)
{
  sn_object_assign_selected(SN_OBJECT(object), selected);
}

GString *
sn_folder_get_copy_title(SNFolder *object)
{
  return g_string_new(object->_title->str);
}

void
sn_folder_copy_title(SNFolder *object, gchar *title)
{
  g_string_assign(object->_title, title);
}

glong
sn_folder_get_count(SNFolder *object)
{
  return object->_count;
}

void
sn_folder_assign_count(SNFolder *object, glong count)
{
  object->_count = count;
}

GString *
sn_folder_create_description(SNFolder *object)
{
  SNObjectClass *klass;
  g_return_val_if_fail(SN_IS_FOLDER(object), NULL);
  klass = SN_OBJECT_CLASS(G_OBJECT(object)->g_type_instance.g_class);
  g_return_val_if_fail(klass->create_description != NULL, NULL);
  return klass->create_description(SN_OBJECT(object));
}

static GString *
sn_light_folder_real_create_description(SNLightFolder *object)
{
  return sn_folder_create_description(SN_FOLDER(object));
}

static guint64
sn_light_folder_real_get_id(SNLightFolder *object)
{
  return sn_folder_get_id(SN_FOLDER(object));
}

static gboolean
sn_light_folder_real_get_selected(SNLightFolder *object)
{
  return sn_folder_get_selected(SN_FOLDER(object));
}

static GString *
sn_light_folder_real_get_copy_title(SNLightFolder *object)
{
  return sn_folder_get_copy_title(SN_FOLDER(object));
}

static glong
sn_light_folder_real_get_count(SNLightFolder *object)
{
  return sn_folder_get_count(SN_FOLDER(object));
}
