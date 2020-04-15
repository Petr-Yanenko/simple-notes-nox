//
//  light_folder.c
//  simple_notes
//
//  Created by Petr Yanenko on 11/22/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "light_folder.h"


G_DEFINE_INTERFACE(SNLightFolder, sn_light_folder, G_TYPE_OBJECT)


static void
sn_light_folder_default_init(SNLightFolderInterface *iface)
{
}

GString *
sn_light_folder_create_description(SNLightFolder *object)
{
  SNLightFolderInterface *iface;
  SN_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object,
					      &iface,
					      create_description,
					      SNLightFolder,
					      SN,
					      LIGHT_FOLDER,
					      NULL);

  return iface->create_description(object);
}

guint64
sn_light_folder_get_id(SNLightFolder *object)
{
  SNLightFolderInterface *iface;
  SN_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object,
					      &iface,
					      get_id,
					      SNLightFolder,
					      SN,
					      LIGHT_FOLDER,
					      0);

  return iface->get_id(object);
}

gboolean
sn_light_folder_get_selected(SNLightFolder *object)
{
  SNLightFolderInterface *iface;
  SN_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object,
					      &iface,
					      get_selected,
					      SNLightFolder,
					      SN,
					      LIGHT_FOLDER,
					      FALSE);

  return iface->get_selected(object);
}

GByteArray *
sn_light_folder_get_copy_title(SNLightFolder *object)
{
  SNLightFolderInterface *iface;
  SN_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object,
					      &iface,
					      get_copy_title,
					      SNLightFolder,
					      SN,
					      LIGHT_FOLDER,
					      NULL);

  return iface->get_copy_title(object);
}

glong
sn_light_folder_get_count(SNLightFolder *object)
{
  SNLightFolderInterface *iface;
  SN_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(object,
					      &iface,
					      get_count,
					      SNLightFolder,
					      SN,
					      LIGHT_FOLDER,
					      0);

  return iface->get_count(object);
}
