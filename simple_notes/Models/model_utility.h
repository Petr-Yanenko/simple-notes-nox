//
//  model_utility.h
//  simple_notes
//
//  Created by Petr Yanenko on 11/30/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef sn_model_utility____FILEEXTENSION___
#define sn_model_utility____FILEEXTENSION___

#include "selected_list_model.h"


#define SN_COPY_DATA(object, items, out_count) {			\
    SNListModel *listModel = SN_LIST_MODEL(object);			\
    g_return_val_if_fail(out_count != NULL, NULL);			\
    g_return_val_if_fail(items != NULL, NULL);				\
    g_return_val_if_fail(listModel, NULL);				\
    GPtrArray *data = sn_list_model_copy_data(listModel, 0);		\
    guint len = 0;							\
    void *parray = NULL;						\
    if (data)								\
      {								\
	len = data->len;						\
	parray = g_ptr_array_free(data, FALSE);			\
      }								\
    *items = parray;							\
    *out_count = len;							\
}


#endif
