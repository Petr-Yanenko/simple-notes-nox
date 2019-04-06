//
//  selected_list_model.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/31/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_selected_list_model____FILEEXTENSION___
#define simple_notes_selected_list_model____FILEEXTENSION___

#include "list_model.h"
#include "../DataStructures/object.h"

static gchar *const kSelectedObjectID = "selected-object-id\0";

typedef struct _SimpleNotesMediator SimpleNotesMediator;

#define SIMPLE_NOTES_TYPE_SELECTED_LIST_MODEL simple_notes_selected_list_model_get_type ()
G_DECLARE_DERIVABLE_TYPE(
SimpleNotesSelectedListModel,
simple_notes_selected_list_model,
SIMPLE_NOTES,
SELECTED_LIST_MODEL,
SimpleNotesListModel
)

struct _SimpleNotesSelectedListModelClass {
    SimpleNotesListModelClass parent;

    SimpleNotesObject * (*get_object) (SimpleNotesSelectedListModel *object, guint position);
    void (*append_object) (SimpleNotesSelectedListModel *object, SimpleNotesObject *item);
    void (*delete_object) (SimpleNotesSelectedListModel *object, SimpleNotesObject *item);
    void (*save_objects) (SimpleNotesSelectedListModel *object, SimpleNotesObject *items[], glong count);
    SimpleNotesObject ** (*copy_items) (SimpleNotesSelectedListModel *object, guint *out_count);
};

void simple_notes_selected_list_model_set_weak_ref_mediator (SimpleNotesSelectedListModel *object, SimpleNotesMediator *mediator);

SimpleNotesObject *simple_notes_selected_list_model_get_object (SimpleNotesSelectedListModel *object, guint position);
void simple_notes_selected_list_model_append_object (SimpleNotesSelectedListModel *object, SimpleNotesObject *item);
void simple_notes_selected_list_model_delete_object (SimpleNotesSelectedListModel *object, SimpleNotesObject *item);
void simple_notes_selected_list_model_save_objects (
        SimpleNotesSelectedListModel *object,
        SimpleNotesObject *items[],
        glong count
);
SimpleNotesObject **simple_notes_selected_list_model_copy_items (SimpleNotesSelectedListModel *object, guint *out_count);

guint64 simple_notes_selected_list_model_get_selected_object_id (SimpleNotesSelectedListModel *object);
void simple_notes_selected_list_model_assign_selected_object_id (SimpleNotesSelectedListModel *object, guint64 objectID);

SimpleNotesObject *simple_notes_selected_list_model_find_object (SimpleNotesSelectedListModel *object, guint64 objectID);
SimpleNotesObject *simple_notes_selected_list_model_find_selected_object (SimpleNotesSelectedListModel *object);

#endif
