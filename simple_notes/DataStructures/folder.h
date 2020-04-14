//
//  folder.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/15/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef sn_folder____FILEEXTENSION___
#define sn_folder____FILEEXTENSION___

#include "object.h"


#define SN_TYPE_FOLDER sn_folder_get_type()
G_DECLARE_FINAL_TYPE(SNFolder,
		     sn_folder,
		     SN,
		     FOLDER,
		     SNObject);


SNFolder *
sn_folder_new(void);


GString *
sn_folder_create_description(SNFolder *object);

guint64
sn_folder_get_id(SNFolder *object);

void
sn_folder_assign_id(SNFolder *object, guint64 id);

gboolean
sn_folder_get_selected(SNFolder *object);

void
sn_folder_assign_selected(SNFolder *object, gboolean selected);

GByteArray *
sn_folder_get_copy_title(SNFolder *object);

void
sn_folder_copy_title(SNFolder *object, GByteArray *title);

glong
sn_folder_get_count(SNFolder *object);

void
sn_folder_assign_count(SNFolder *object, glong count);


#endif
