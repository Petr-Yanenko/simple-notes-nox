//
//  light_folder.h
//  simple_notes
//
//  Created by Petr Yanenko on 11/22/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_folder_iface____FILEEXTENSION___
#define simple_notes_folder_iface____FILEEXTENSION___

#include "PrefixHeader.pch"

#define SIMPLE_NOTES_TYPE_LIGHT_FOLDER simple_notes_light_folder_get_type ()
G_DECLARE_INTERFACE(SimpleNotesLightFolder, simple_notes_light_folder, SIMPLE_NOTES, LIGHT_FOLDER, GObject)

struct _SimpleNotesLightFolderInterface {
    GTypeInterface parent;

    GString * (*create_description) (SimpleNotesLightFolder *object);
    guint64 (*get_id) (SimpleNotesLightFolder *object);
    gboolean (*get_selected) (SimpleNotesLightFolder *object);
    GByteArray * (*get_copy_title) (SimpleNotesLightFolder *object);
    glong (*get_count) (SimpleNotesLightFolder *object);
};

GString *simple_notes_light_folder_create_description (SimpleNotesLightFolder *object);

guint64 simple_notes_light_folder_get_id (SimpleNotesLightFolder *object);

gboolean simple_notes_light_folder_get_selected (SimpleNotesLightFolder *object);

GByteArray *simple_notes_light_folder_get_copy_title (SimpleNotesLightFolder *object);

glong simple_notes_light_folder_get_count (SimpleNotesLightFolder *object);

#endif
