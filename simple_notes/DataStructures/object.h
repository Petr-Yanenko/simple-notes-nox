//
//  object.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/15/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_object____FILEEXTENSION___
#define simple_notes_object____FILEEXTENSION___

#include "simple_notes.h"

#define SIMPLE_NOTES_TYPE_OBJECT simple_notes_object_get_type ()
G_DECLARE_DERIVABLE_TYPE (SimpleNotesObject, simple_notes_object, SIMPLE_NOTES, OBJECT, GObject);

struct _SimpleNotesObjectClass {
    GObjectClass parent;

    GString * (*create_description) (SimpleNotesObject *object);
};

SimpleNotesObject *simple_notes_object_new (void);

GString *simple_notes_object_create_description (SimpleNotesObject *object);

guint64 simple_notes_object_get_id (SimpleNotesObject *object);
void simple_notes_object_assign_id (SimpleNotesObject *object, guint64 id);

gboolean simple_notes_object_get_selected (SimpleNotesObject *object);
void simple_notes_object_assign_selected (SimpleNotesObject *object, gboolean selected);

#endif
