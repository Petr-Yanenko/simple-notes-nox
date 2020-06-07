//
//  global_functions.c
//  simple_notes
//
//  Created by Petr Yanenko on 12/16/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//


#include "global_functions.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>


#ifdef TESTING
gchar *const kFile = "test_simple_notes.data";
gchar *const kNoteFolder = "test_notes";
#else
gchar *const kFile = "simple_notes.data";
gchar *const kNoteFolder = "notes";
#endif

GQuark const kSimpleNotesDomain = 11111;
long const kLongLongSymbols = 21;
long const kSelectedSymbols = 2;

gchar *const kFolderPathFormat ="folder_%llu/";
gchar *const kNotePathFormat = "folder_%llu/note_%ld.txt";


gboolean
simple_notes_trash_file (gchar *fileName)
{
  GFile *file = g_file_new_for_path (fileName);
  gboolean success = g_file_trash (file, NULL, NULL);
  g_object_unref (file);
  return success;
}

void
sn_print_ustring(GString *ustring, GString *buff, gchar *title)
{
  if (ustring)
    {
      g_string_append_printf(buff, "%s: 0x", title);
      for (glong i = 0; i < ustring->len; i++)
	{
	  g_string_append_printf(buff, "%x", ustring->str[i]);
	}
    }
  else
    {
      g_string_append_printf(buff, "%s: 0x0", title);
    }
}

void
sn_copy(GType type,
	void (setter)(GValue *value),
	void (getter)(GValue *value))
{
  GValue value = G_VALUE_INIT;
  g_value_init(&value, type);
  setter(&value);
  getter(&value);
  g_value_unset(&value);
}

guint
simple_notes_gulong_guint_cast (gulong value)
{
  if (value <= UINT32_MAX)
    {
      return (guint)value;
    }

  g_warn_if_reached ();
  return 0;
}

void
simple_notes_create_error (GError **pError,
                           gint code,
                           gchar *const format,
                           gint argsNumber,
                           ...)
{
  va_list args;
  va_start (args, argsNumber);
  SIMPLE_NOTES_CREATE_ERROR (pError, code, format, argsNumber, args);
  va_end (args);
}

gchar *
sn_copy_string(gchar *const string)
{
  gchar *mallocString = g_malloc(sizeof(gchar) * (strlen(string) + 1));
  strcpy(mallocString, string);
  return mallocString;
}

void
sn_print_guint64_value(gchar * buff, guint64 value)
{
  sprintf(buff, "%llu", value);
}

void
sn_print_gint64_value(gchar *buff, gint64 value)
{
  sprintf(buff, "%lld", value);
}

void
sn_print_boolean_value(gchar *buff, gboolean value)
{
  sprintf(buff, "%i", value);
}

void
sn_print_long_value(gchar *buff, glong value)
{
  sprintf(buff, "%ld", value);
}

void
sn_free_objects_array(gpointer *array, gulong count)
{
  for (glong i = 0; i < count; i++)
    {
      g_object_unref (array[i]);
    }
  if (array)
    {
      g_free (array);
    }
}

gulong
sn_notify_connect(GObject *subject,
		  gchar *const notify,
		  void (*callback)(GObject *, GParamSpec *, gpointer),
		  gpointer user_data)
{
  gchar *signalName = "notify::";
  gchar *signalDetail = notify;
  glong nameLen = strlen(signalName);
  glong detailLen = strlen(signalDetail);
  glong const fullLen = nameLen + detailLen +1;
  gchar buff[fullLen];
  sprintf(buff, "%s%s", signalName, signalDetail);

  gulong identifier = g_signal_connect(subject,
				       buff,
				       G_CALLBACK(callback),
				       user_data);

  return identifier;
}
