//
//  event.h
//  simple_notes
//
//  Created by Petr Yanenko on 1/7/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_event____FILEEXTENSION___
#define simple_notes_event____FILEEXTENSION___

#include "simple_notes.h"

#define SIMPLE_NOTES_TYPE_EVENT simple_notes_event_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesEvent, simple_notes_event, SIMPLE_NOTES, EVENT, GObject)

SimpleNotesEvent *simple_notes_event_new (gchar *const buffer, gssize count, GError **error);

GString *simple_notes_event_get_command (SimpleNotesEvent *event);
GHashTable *simple_notes_event_get_options (SimpleNotesEvent *event);
gchar *simple_notes_event_get_value_for_option (SimpleNotesEvent *event, gchar *option);

#endif
