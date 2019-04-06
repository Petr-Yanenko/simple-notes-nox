//
//  light_folder.c
//  simple_notes
//
//  Created by Petr Yanenko on 11/22/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "light_folder.h"

G_DEFINE_INTERFACE(SimpleNotesLightFolder, simple_notes_light_folder, G_TYPE_OBJECT)

static void simple_notes_light_folder_default_init (SimpleNotesLightFolderInterface *iface) {

}

GString *simple_notes_light_folder_create_description (SimpleNotesLightFolder *object) {
    SimpleNotesLightFolderInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(
            object, &iface, create_description, SimpleNotesLightFolder, SIMPLE_NOTES, LIGHT_FOLDER, NULL
    );
    return iface->create_description(object);
}

guint64 simple_notes_light_folder_get_id (SimpleNotesLightFolder *object) {
    SimpleNotesLightFolderInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(
            object, &iface, get_id, SimpleNotesLightFolder, SIMPLE_NOTES, LIGHT_FOLDER, 0
    );
    return iface->get_id(object);
}

gboolean simple_notes_light_folder_get_selected (SimpleNotesLightFolder *object) {
    SimpleNotesLightFolderInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(
            object, &iface, get_selected, SimpleNotesLightFolder, SIMPLE_NOTES, LIGHT_FOLDER, FALSE
    );
    return iface->get_selected(object);
}

GByteArray *simple_notes_light_folder_get_copy_title (SimpleNotesLightFolder *object) {
    SimpleNotesLightFolderInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(
            object, &iface, get_copy_title, SimpleNotesLightFolder, SIMPLE_NOTES, LIGHT_FOLDER, NULL
    );
    return iface->get_copy_title(object);
}

glong simple_notes_light_folder_get_count (SimpleNotesLightFolder *object) {
    SimpleNotesLightFolderInterface *iface;
    SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(
            object, &iface, get_count, SimpleNotesLightFolder, SIMPLE_NOTES, LIGHT_FOLDER, 0
    );
    return iface->get_count(object);
}
