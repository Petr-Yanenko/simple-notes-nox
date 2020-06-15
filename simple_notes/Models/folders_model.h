//
//  folders_model.h
//  simple_notes
//
//  Created by Petr Yanenko on 11/11/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef sn_folders_model____FILEEXTENSION___
#define sn_folders_model____FILEEXTENSION___

#include "selected_list_model.h"
#include "light_folder.h"


#define SN_TYPE_FOLDERS_MODEL sn_folders_model_get_type()
G_DECLARE_FINAL_TYPE(SNFoldersModel,
		     sn_folders_model,
		     SN,
		     FOLDERS_MODEL,
		     SNSelectedListModel)


SNFoldersModel *
sn_folders_model_new(void);


void
sn_folders_model_insert_folder(SNFoldersModel *object, gchar *name);

SNLightFolder **
sn_folders_model_copy_folders(SNFoldersModel *object, guint *out_count);

void
sn_folders_model_delete_folder(SNFoldersModel *object, guint64 identifier);

void
sn_folders_model_select_folder(SNFoldersModel *object, guint64 identifier);

void
sn_folders_model_change_folder_title(SNFoldersModel *object,
				     guint64 identifier,
				     gchar *newTitle);


#endif
