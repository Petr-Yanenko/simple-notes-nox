//
//  model_utility.c
//  simple_notes
//
//  Created by Petr Yanenko on 11/30/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "model_utility.h"
#include "sqlite_controller.h"


void simple_notes_delete_object (SimpleNotesSelectedListModel *object, guint64 identifier, void (handler)(SimpleNotesObject *)) {
    SimpleNotesObject *foundItem = simple_notes_selected_list_model_find_object(object, identifier);
    if (foundItem) {
        handler(foundItem);
        simple_notes_selected_list_model_delete_object(object, foundItem);
    } else {
        g_printerr("\nDeleted object %llu is missing\n", identifier);
    }
}

void simple_notes_save_objects (
        SimpleNotesSelectedListModel *object,
        SimpleNotesObject *items[],
        glong count,
        void (handler)(SimpleNotesObject *item)
) {
    simple_notes_sqlite_controller_begin_transaction();
    for (glong i = 0; i < count; i++) {
        handler(items[i]);
    }
    simple_notes_sqlite_controller_commit_transaction();
}

void simple_notes_model_changed (SimpleNotesBaseModel *object) {
    SimpleNotesBaseModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC(object, &klass, changed, SimpleNotesBaseModel, SIMPLE_NOTES, BASE_MODEL);
    klass->changed(object);
}
