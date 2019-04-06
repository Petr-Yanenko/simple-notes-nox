//
//  light_note.c
//  simple_notes
//
//  Created by Petr Yanenko on 11/23/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "light_note.h"

G_DEFINE_INTERFACE(SimpleNotesLightNote, simple_notes_light_note, G_TYPE_OBJECT)

static void simple_notes_light_note_default_init (SimpleNotesLightNoteInterface *iface) {

}

GString *simple_notes_light_note_create_description (SimpleNotesLightNote *object) {
    SimpleNotesLightNoteInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object, &iface, create_description, SimpleNotesLightNote, SIMPLE_NOTES, LIGHT_NOTE, NULL);
    return iface->create_description(object);
}

guint64 simple_notes_light_note_get_id (SimpleNotesLightNote *object) {
    SimpleNotesLightNoteInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object, &iface, get_id, SimpleNotesLightNote, SIMPLE_NOTES, LIGHT_NOTE, 0);
    return iface->get_id(object);
}

gboolean simple_notes_light_note_get_selected (SimpleNotesLightNote *object) {
    SimpleNotesLightNoteInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object, &iface, get_selected, SimpleNotesLightNote, SIMPLE_NOTES, LIGHT_NOTE, FALSE);
    return iface->get_selected(object);
}

GByteArray *simple_notes_light_note_get_copy_content (SimpleNotesLightNote *object) {
    SimpleNotesLightNoteInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object, &iface, get_copy_content, SimpleNotesLightNote, SIMPLE_NOTES, LIGHT_NOTE, NULL);
    return iface->get_copy_content(object);
}

guint64 simple_notes_light_note_get_folder_id (SimpleNotesLightNote *object) {
    SimpleNotesLightNoteInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object, &iface, get_folder_id, SimpleNotesLightNote, SIMPLE_NOTES, LIGHT_NOTE, 0);
    return iface->get_folder_id(object);
}

GDateTime *simple_notes_light_note_get_copy_last_edited (SimpleNotesLightNote *object) {
    SimpleNotesLightNoteInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object, &iface, get_copy_last_edited, SimpleNotesLightNote, SIMPLE_NOTES, LIGHT_NOTE, NULL);
    return iface->get_copy_last_edited(object);
}
