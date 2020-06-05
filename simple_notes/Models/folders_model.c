//
//  folders_model.c
//  simple_notes
//
//  Created by Petr Yanenko on 11/11/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "folders_model.h"
#include "model_utility.h"
#include "folder.h"
#include "folder_presenter.h"
#include <stdio.h>
#include <string.h>


struct _SNFoldersModel {
  SNSelectedListModel _parent;

  SNFolderPresenter *_pres;
};


G_DEFINE_TYPE(SNFoldersModel, sn_folders_model, SN_TYPE_SELECTED_LIST_MODEL)


static void
sn_folders_model_real_fetch(SNListModel *object);


static void
sn_folders_model_class_init(SNFoldersModelClass *klass)
{
  SNListModelClass *listModelClass = SN_LIST_MODEL_CLASS(klass);
  listModelClass->fetch = sn_folders_model_real_fetch;
}

static void
sn_folders_model_init(SNFoldersModel *object)
{
  SNIEntityModel *imodel = SN_IENTITY_MODEL(object);
  object->_pres = sn_folder_presenter_new(imodel);
}

SNFoldersModel *
sn_folders_model_new(void)
{
  SNFoldersModel *instance = g_object_new(SN_TYPE_FOLDERS_MODEL, NULL);

  return instance;
}

void
sn_folders_model_insert_folder(SNFoldersModel *object, gchar *name)
{
  sn_folder_presenter_add_folder(object->_pres, name);
}

SNLightFolder **
sn_folders_model_copy_folders(SNFoldersModel *object, guint *out_count)
{
  SNSelectedListModelClass *parent;
  parent = SN_SELECTED_LIST_MODEL_CLASS(sn_folders_model_parent_class);
  return (SNLightFolder **)parent->copy_items(SN_SELECTED_LIST_MODEL(object),
					      out_count);
}

void
sn_folders_model_delete_folder(SNFoldersModel *object, guint64 identifier)
{
  sn_folder_presenter_delete_folder(object->_pres, identifier);
}

void
sn_folders_model_select_folder(SNFoldersModel *object, guint64 identifier)
{
  sn_folder_presenter_select_folder(object->_pres, identifier);
}

void
sn_folders_model_change_folder_title(SNFoldersModel *object,
				     guint64 identifier,
				     gchar *newTitle)
{
  sn_folder_presenter_change_title(object->_pres, identifier, newTitle);
}

static void
sn_folders_model_real_fetch(SNListModel *object)
{
  sn_folder_presenter_fetch(SN_FOLDERS_MODEL(object)->_pres);
}
