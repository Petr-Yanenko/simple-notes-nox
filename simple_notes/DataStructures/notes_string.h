//
//  symbol.h
//  simple_notes
//
//  Created by Petr Yanenko on 12/4/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_string____FILEEXTENSION___
#define simple_notes_string____FILEEXTENSION___

#include <glib-object.h>
#include "string_style.h"
#include "string_type.h"

/*
<simple_notes_string type="1">
<text>Text</text>
<style/>
</simple_notes_string>
 */

static gchar *const kStringTag = "simple_notes_string\0";
static gchar *const kTextTag = "text\0";
static gchar *const kStyleTag = "style\0";
static gchar *const kTypeAttribute = "type\0";

#define SIMPLE_NOTES_TYPE_STRING simple_notes_string_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesString, simple_notes_string, SIMPLE_NOTES, STRING, GObject)

SimpleNotesString *simple_notes_string_new (SimpleNotesStringTypeIdentifier type);

GByteArray *simple_notes_string_get_text (SimpleNotesString *object);
void simple_notes_string_ref_text (SimpleNotesString *object, GByteArray *text);

SimpleNotesStringType *simple_notes_string_get_string_type (SimpleNotesString *object);
SimpleNotesStringStyle *simple_notes_string_get_string_style (SimpleNotesString *object);

gchar *simple_notes_string_create_xml (SimpleNotesString *object);

#endif
