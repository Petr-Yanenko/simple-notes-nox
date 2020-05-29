//
//  list_model.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/25/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef sn_list_model____FILEEXTENSION___
#define sn_list_model____FILEEXTENSION___

#include "base_model.h"


#define SN_TYPE_LIST_MODEL sn_list_model_get_type()
G_DECLARE_DERIVABLE_TYPE(SNListModel,
			 sn_list_model,
			 SN,
			 LIST_MODEL,
			 SNBaseModel)


struct _SNListModelClass {
  SNBaseModelClass _parent;

  GList * (*get_item)(SNListModel *object, guint position);
  void (*append)(SNListModel *object, gpointer item);
  void (*delete)(SNListModel *object, gpointer item);

  //private
  GList * (*create_items)(SNListModel *object);
};


void
sn_list_model_copy_list(SNListModel *object, GList *list);

GList *
sn_list_model_get_item(SNListModel *object, guint position);

GPtrArray *
sn_list_model_copy_data(SNListModel *object, guint fromPosition);

void
sn_list_model_append(SNListModel *object, gpointer item);

void
sn_list_model_delete(SNListModel *object, gpointer item);

glong
sn_list_model_get_count(SNListModel *object);


#endif
