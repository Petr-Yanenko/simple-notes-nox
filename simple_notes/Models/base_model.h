//
//  base_model.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/24/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_base_model____FILEEXTENSION___
#define simple_notes_base_model____FILEEXTENSION___

#include "simple_notes.h"

static gchar *const kLoading = "loading\0";
static gchar *const kNewData = "new-data\0";
static gchar *const kErrorReason = "error-reason\0";

#define SIMPLE_NOTES_TYPE_BASE_MODEL simple_notes_base_model_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesBaseModel, simple_notes_base_model, SIMPLE_NOTES, BASE_MODEL, GObject)

typedef void (*SimpleNotesBaseModelVirtualFunc) (SimpleNotesBaseModel *);

struct _SimpleNotesBaseModelClass {
    GObjectClass parent;

    SimpleNotesBaseModelVirtualFunc reset;
    SimpleNotesBaseModelVirtualFunc load_data;
    SimpleNotesBaseModelVirtualFunc cancel;

    //private
    SimpleNotesBaseModelVirtualFunc load;
    SimpleNotesBaseModelVirtualFunc changed;
};

void simple_notes_base_model_load_data (SimpleNotesBaseModel *object);
void simple_notes_base_model_reset (SimpleNotesBaseModel *object);
void simple_notes_base_model_cancel (SimpleNotesBaseModel *object);

gboolean simple_notes_base_model_get_loading (SimpleNotesBaseModel *object);
void simple_notes_base_model_assign_loading (SimpleNotesBaseModel *object, gboolean loading);

gboolean simple_notes_base_model_get_new_data (SimpleNotesBaseModel *object);
void simple_notes_base_model_assign_new_data (SimpleNotesBaseModel *object, gboolean newData);

gchar *simple_notes_base_model_get_copy_error_reason (SimpleNotesBaseModel *object);
void simple_notes_base_model_copy_error_reason (SimpleNotesBaseModel *object, gchar *errorReason);

#endif
