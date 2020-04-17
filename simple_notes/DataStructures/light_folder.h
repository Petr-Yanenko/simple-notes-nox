//
//  light_folder.h
//  simple_notes
//
//  Created by Petr Yanenko on 11/22/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//


#ifndef sn_folder_iface____FILEEXTENSION___
#define sn_folder_iface____FILEEXTENSION___

#include "simple_notes.h"


#define SN_TYPE_LIGHT_FOLDER sn_light_folder_get_type()
G_DECLARE_INTERFACE(SNLightFolder, sn_light_folder, SN, LIGHT_FOLDER, GObject)

struct _SNLightFolderInterface {
  GTypeInterface _parent;

  GString * (*create_description)(SNLightFolder *object);
  guint64 (*get_id)(SNLightFolder *object);
  gboolean (*get_selected)(SNLightFolder *object);
  GString * (*get_copy_title)(SNLightFolder *object);
  glong (*get_count)(SNLightFolder *object);
};

GString *
sn_light_folder_create_description(SNLightFolder *object);

guint64
sn_light_folder_get_id(SNLightFolder *object);

gboolean
sn_light_folder_get_selected(SNLightFolder *object);

GString *
sn_light_folder_get_copy_title(SNLightFolder *object);

glong
sn_light_folder_get_count(SNLightFolder *object);

#endif
