//
//  note.c
//  simple_notes
//
//  Created by Petr Yanenko on 11/23/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//


#include "note.h"
#include "light_note.h"


struct _SNNote {
  SNObject _parent;

  guint64 _folderID;
  GString *_content;
  GDateTime *_lastEdited;
};


static void
sn_light_note_interface_init(SNLightNoteInterface *iface);


G_DEFINE_TYPE_WITH_CODE(SNNote, sn_note, SN_TYPE_OBJECT,
			G_IMPLEMENT_INTERFACE(SN_TYPE_LIGHT_NOTE,
					      sn_light_note_interface_init))


static GString *
sn_light_note_real_create_description(SNLightNote *object);

static guint64
sn_light_note_real_get_id(SNLightNote *object);

static gboolean
sn_light_note_real_get_selected(SNLightNote *object);

static GString *
sn_light_note_real_get_copy_content(SNLightNote *object);

static guint64
sn_light_note_real_get_folder_id(SNLightNote *object);

static GDateTime *
sn_light_note_real_get_copy_last_edited(SNLightNote *object);


static GString *
sn_note_real_create_description(SNObject *object)
{
  SNNote *note = SN_NOTE(object);
  SNObjectClass *parentClass = SN_OBJECT_CLASS(sn_note_parent_class);
  GString *description = parentClass->create_description(object);
  sn_print_ustring(note->_content, description, "Content\0");
  g_string_append_printf(description, "\nFolder id: %llu\n", note->_folderID);
  if (note->_lastEdited)
    {
      gchar *lastEdited = g_date_time_format(note->_lastEdited, "%c");
      g_string_append_printf(description, "\nLast edited: %s\n", lastEdited);
      g_free(lastEdited);
    } else
    {
      g_string_append(description, "\nLast edited: NULL\n");
    }

    return description;
}

static void
sn_note_dispose(GObject *object)
{
  SNNote *note = SN_NOTE(object);
  if (note->_content)
    {
      g_string_free(note->_content, TRUE);
    }
  if (note->_lastEdited)
    {
      g_date_time_unref(note->_lastEdited);
    }

  G_OBJECT_CLASS(sn_note_parent_class)->dispose(object);
}

static void
sn_light_note_interface_init(SNLightNoteInterface *iface)
{
  iface->create_description = sn_light_note_real_create_description;
  iface->get_id = sn_light_note_real_get_id;
  iface->get_selected = sn_light_note_real_get_selected;
  iface->get_copy_content = sn_light_note_real_get_copy_content;
  iface->get_folder_id = sn_light_note_real_get_folder_id;
  iface->get_copy_last_edited = sn_light_note_real_get_copy_last_edited;
}

static void
sn_note_class_init(SNNoteClass *klass)
{
  GObjectClass *gClassObject = G_OBJECT_CLASS(klass);
  SNObjectClass *parent = SN_OBJECT_CLASS(klass);
  gClassObject->dispose = sn_note_dispose;
  parent->create_description = sn_note_real_create_description;
}

static void
sn_note_init(SNNote *object)
{
  object->_folderID = 0;
  object->_content = g_string_new(NULL);
  object->_lastEdited = NULL;
}

SNNote *
sn_note_new(void)
{
  return g_object_new(SN_TYPE_NOTE, NULL);
}

GString *
sn_note_create_description(SNNote *object)
{
  SNObjectClass *klass;
  g_return_val_if_fail(SN_IS_NOTE(object), NULL);
  klass = SN_OBJECT_CLASS(G_OBJECT(object)->g_type_instance.g_class);
  g_return_val_if_fail(klass->create_description != NULL, NULL);
  return klass->create_description(SN_OBJECT(object));
}

guint64
sn_note_get_id(SNNote *object)
{
  return sn_object_get_id(SN_OBJECT(object));
}

void
sn_note_assign_id(SNNote *object, guint64 id)
{
  sn_object_assign_id(SN_OBJECT(object), id);
}

gboolean sn_note_get_selected(SNNote *object)
{
  return sn_object_get_selected(SN_OBJECT(object));
}

void
sn_note_assign_selected(SNNote *object, gboolean selected)
{
  sn_object_assign_selected(SN_OBJECT(object), selected);
}

GString *
sn_note_get_copy_content(SNNote *object)
{
  return g_string_new(object->_content->str);
}

void
sn_note_copy_content(SNNote *object, gchar *content)
{
  g_string_assign(object->_content, content);
}

guint64
sn_note_get_folder_id(SNNote *object)
{
  return object->_folderID;
}

void
sn_note_assign_folder_id(SNNote *object, guint64 folderID)
{
  object->_folderID = folderID;
}

GDateTime *
sn_note_get_copy_last_edited(SNNote *object)
{
  GDateTime *copy = NULL;

  void setter (GValue *value)
  {
    g_value_set_boxed(value, object->_lastEdited);
  }

  void getter (GValue *value)
  {
    if (g_value_get_boxed(value))
      {
	copy = g_value_dup_boxed(value);
      }
  }

  sn_copy(G_TYPE_DATE_TIME, setter, getter);

  return copy;
}

void
sn_note_copy_last_edited(SNNote *object, GDateTime *time)
{
  if (object->_lastEdited)
    {
      g_date_time_unref(object->_lastEdited);
      object->_lastEdited = NULL;
    }
    if (time)
      {
	void setter (GValue *value)
	{
	  g_value_set_boxed(value, time);
	}

	void getter (GValue *value)
	{
	  object->_lastEdited = g_value_dup_boxed(value);
	}

	sn_copy(G_TYPE_DATE_TIME, setter, getter);
      }
}

static GString *
sn_light_note_real_create_description(SNLightNote *object)
{
  return sn_note_create_description(SN_NOTE(object));
}

static guint64
sn_light_note_real_get_id(SNLightNote *object)
{
  return sn_note_get_id(SN_NOTE(object));
}

static gboolean
sn_light_note_real_get_selected(SNLightNote *object)
{
  return sn_note_get_selected(SN_NOTE(object));
}

static GString *
sn_light_note_real_get_copy_content(SNLightNote *object)
{
  return sn_note_get_copy_content(SN_NOTE(object));
}

static guint64
sn_light_note_real_get_folder_id(SNLightNote *object)
{
  return sn_note_get_folder_id(SN_NOTE(object));
}

static GDateTime *
sn_light_note_real_get_copy_last_edited(SNLightNote *object)
{
  return sn_note_get_copy_last_edited(SN_NOTE(object));
}
