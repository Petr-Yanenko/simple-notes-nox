//
//  light_note.h
//  simple_notes
//
//  Created by Petr Yanenko on 11/23/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_light_note____FILEEXTENSION___
#define simple_notes_light_note____FILEEXTENSION___

#include <glib-object.h>

#define SIMPLE_NOTES_TYPE_LIGHT_NOTE simple_notes_light_note_get_type ()
G_DECLARE_INTERFACE(SimpleNotesLightNote, simple_notes_light_note, SIMPLE_NOTES, LIGHT_NOTE, GObject)

struct _SimpleNotesLightNoteInterface {
    GTypeInterface parent;

    GString * (*create_description) (SimpleNotesLightNote *object);
    guint64 (*get_id) (SimpleNotesLightNote *object);
    gboolean (*get_selected) (SimpleNotesLightNote *object);
    GByteArray * (*get_copy_content) (SimpleNotesLightNote *object);
    guint64 (*get_folder_id) (SimpleNotesLightNote *object);
    GDateTime * (*get_copy_last_edited) (SimpleNotesLightNote *object);
};

GString *simple_notes_light_note_create_description (SimpleNotesLightNote *object);

guint64 simple_notes_light_note_get_id (SimpleNotesLightNote *object);

gboolean simple_notes_light_note_get_selected (SimpleNotesLightNote *object);

GByteArray *simple_notes_light_note_get_copy_content (SimpleNotesLightNote *object);

guint64 simple_notes_light_note_get_folder_id (SimpleNotesLightNote *object);

GDateTime *simple_notes_light_note_get_copy_last_edited (SimpleNotesLightNote *object);

#endif
