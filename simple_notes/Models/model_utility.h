//
//  model_utility.h
//  simple_notes
//
//  Created by Petr Yanenko on 11/30/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_model_utility____FILEEXTENSION___
#define simple_notes_model_utility____FILEEXTENSION___

#include "selected_list_model.h"

#define SIMPLE_NOTES_COPY_DATA(object, items, out_count) {\
    SimpleNotesListModel *listModel = SIMPLE_NOTES_LIST_MODEL(object);\
    g_return_val_if_fail(out_count != NULL, NULL);\
    g_return_val_if_fail(items != NULL, NULL);\
    g_return_val_if_fail(listModel, NULL);\
    GPtrArray *data = simple_notes_list_model_copy_data(listModel, 0);\
    guint len = 0;\
    void *parray = NULL;\
    if (data) {\
        len = data->len;\
        parray = g_ptr_array_free(data, FALSE);\
    }\
    *items = parray;\
    *out_count = len;\
}

/*void simple_notes_delete_object (SimpleNotesSelectedListModel *object, guint64 identifier, void (^)(SimpleNotesObject *));
void simple_notes_save_objects (
        SimpleNotesSelectedListModel *object,
        SimpleNotesObject *items[],
        glong count,
        void (^handler) (SimpleNotesObject *item)
	);*/
void simple_notes_model_changed (SimpleNotesBaseModel *object);

#endif
