//
//  selected_list_model.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/31/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef sn_selected_list_model____FILEEXTENSION___
#define sn_selected_list_model____FILEEXTENSION___

#include "list_model.h"
#include "object.h"


static gchar *const kSelectedObjectID = "selected-object-id\0";


#define SN_TYPE_SELECTED_LIST_MODEL sn_selected_list_model_get_type()
G_DECLARE_DERIVABLE_TYPE(SNSelectedListModel,
			 sn_selected_list_model,
			 SN,
			 SELECTED_LIST_MODEL,
			 SNListModel)


struct _SNSelectedListModelClass {
  SNListModelClass _parent;

  SNObject * (*get_object)(SNSelectedListModel *object, guint position);
  void (*append_object)(SNSelectedListModel *object, SNObject *item);
  void (*delete_object)(SNSelectedListModel *object, SNObject *item);
  void (*save_objects)(SNSelectedListModel *object,
		       SNObject *items[],
		       glong count);
  SNObject ** (*copy_items)(SNSelectedListModel *object, guint *out_count);
};


SNObject *
sn_selected_list_model_get_object(SNSelectedListModel *object, guint position);

void
sn_selected_list_model_append_object(SNSelectedListModel *object,
				     SNObject *item);

void
sn_selected_list_model_delete_object(SNSelectedListModel *object,
				     SNObject *item);

SNObject **
sn_selected_list_model_copy_items(SNSelectedListModel *object,
				  guint *out_count);


guint64
sn_selected_list_model_get_selected_object_id(SNSelectedListModel *object);

void
sn_selected_list_model_assign_selected_object_id(SNSelectedListModel *object,
						 guint64 objectID);


SNObject *
sn_selected_list_model_find_object(SNSelectedListModel *object,
				   guint64 objectID);

SNObject *
sn_selected_list_model_find_selected_object(SNSelectedListModel *object);


#endif
