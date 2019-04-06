//
//  list_model.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/25/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_list_model____FILEEXTENSION___
#define simple_notes_list_model____FILEEXTENSION___

#include "base_model.h"

#define SIMPLE_NOTES_TYPE_LIST_MODEL simple_notes_list_model_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesListModel, simple_notes_list_model, SIMPLE_NOTES, LIST_MODEL, SimpleNotesBaseModel)

struct _SimpleNotesListModelClass {
    SimpleNotesBaseModelClass parent;

    GList * (*get_item) (SimpleNotesListModel *object, guint position);
    void (*append) (SimpleNotesListModel *object, gpointer item);
    void (*delete) (SimpleNotesListModel *object, gpointer item);

    //private
    GList * (*create_items) (SimpleNotesListModel *object);
};

void simple_notes_list_model_copy_list (SimpleNotesListModel *object, GList *list);

GList *simple_notes_list_model_get_item (SimpleNotesListModel *object, guint position);
GPtrArray *simple_notes_list_model_copy_data (SimpleNotesListModel *object, guint fromPosition);
void simple_notes_list_model_append (SimpleNotesListModel *object, gpointer item);
void simple_notes_list_model_delete (SimpleNotesListModel *object, gpointer item);

glong simple_notes_list_model_get_count (SimpleNotesListModel *object);

#endif
