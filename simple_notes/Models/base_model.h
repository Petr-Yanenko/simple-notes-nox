//
//  base_model.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/24/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef sn_base_model____FILEEXTENSION___
#define sn_base_model____FILEEXTENSION___

#include "simple_notes.h"


extern gchar *const kLoading;
extern gchar *const kNewData;
extern gchar *const kErrorCode;


#define SN_TYPE_BASE_MODEL sn_base_model_get_type()
G_DECLARE_DERIVABLE_TYPE(SNBaseModel,
			 sn_base_model,
			 SN,
			 BASE_MODEL,
			 GObject)


typedef void (*SNBaseModelVirtualFunc)(SNBaseModel *);

struct _SNBaseModelClass {
  GObjectClass _parent;

  SNBaseModelVirtualFunc reset;
  SNBaseModelVirtualFunc load_data;
  SNBaseModelVirtualFunc cancel;

  //private
  SNBaseModelVirtualFunc load;
  SNBaseModelVirtualFunc changed;
};

void
sn_base_model_load_data(SNBaseModel *object);

void
sn_base_model_reset(SNBaseModel *object);

void
sn_base_model_cancel(SNBaseModel *object);


gboolean
sn_base_model_get_loading(SNBaseModel *object);

void
sn_base_model_assign_loading(SNBaseModel *object, gboolean loading);


gboolean
sn_base_model_get_new_data(SNBaseModel *object);

void
sn_base_model_assign_new_data(SNBaseModel *object, gboolean newData);


SNError
sn_base_model_get_error_code(SNBaseModel *object);

void
sn_base_model_assign_error_code(SNBaseModel *object, SNError errorCode);


//Internal
//ToDo: Create separate header for internal or protected methods.
void
sn_base_model_changed(SNBaseModel *object);


#endif
