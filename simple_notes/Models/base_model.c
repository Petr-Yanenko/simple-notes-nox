//
//  base_model.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/24/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "base_model.h"

enum {
    PROP_LOADING = 1,
    PROP_NEW_DATA,
    PROP_ERROR_REASON,
    N_PROPERTIES
};

typedef struct {
    gboolean _loading;
    gboolean _newData;
    gchar *_errorReason;
} SimpleNotesBaseModelPrivate;

static GParamSpec *objProperties[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesBaseModel, simple_notes_base_model, G_TYPE_OBJECT)

static void simple_notes_base_model_load (SimpleNotesBaseModel *object);
static void simple_notes_base_model_real_load_data (SimpleNotesBaseModel *object);
static void simple_notes_base_model_real_load (SimpleNotesBaseModel *object);
static void simple_notes_base_model_real_reset (SimpleNotesBaseModel *object);
static void simple_notes_base_model_real_cancel (SimpleNotesBaseModel *object);
static void simple_notes_base_model_real_changed (SimpleNotesBaseModel *object);
static void simple_notes_base_model_changed (SimpleNotesBaseModel *object);

static void simple_notes_base_model_set_property (
        GObject      *object,
        guint         property_id,
        const GValue *value,
        GParamSpec   *pspec
) {
    SimpleNotesBaseModel *self = SIMPLE_NOTES_BASE_MODEL(object);
    SimpleNotesBaseModelPrivate *modelPrivate = simple_notes_base_model_get_instance_private(self);

    switch (property_id)
    {
        case PROP_LOADING:
            modelPrivate->_loading = g_value_get_boolean(value);
            break;

        case PROP_NEW_DATA:
            modelPrivate->_newData = g_value_get_boolean(value);
            break;

        case PROP_ERROR_REASON:
            g_free(modelPrivate->_errorReason);
            modelPrivate->_errorReason = g_value_dup_string(value);
            g_print("\nerror reason: %s\n", modelPrivate->_errorReason);
            break;

        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void simple_notes_base_model_get_property (
        GObject    *object,
        guint       property_id,
        GValue     *value,
        GParamSpec *pspec
) {
    SimpleNotesBaseModel *self = SIMPLE_NOTES_BASE_MODEL(object);
    SimpleNotesBaseModelPrivate *modelPrivate = simple_notes_base_model_get_instance_private(self);

    switch (property_id)
    {
        case PROP_LOADING:
            g_value_set_boolean(value, modelPrivate->_loading);
            break;

        case PROP_NEW_DATA:
            g_value_set_boolean(value, modelPrivate->_newData);
            break;

        case PROP_ERROR_REASON:
            g_value_set_string(value, modelPrivate->_errorReason);
            break;

        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void simple_notes_base_model_dispose (GObject *object) {
    SimpleNotesBaseModelPrivate *modelPrivate = simple_notes_base_model_get_instance_private(SIMPLE_NOTES_BASE_MODEL(object));
    g_free(modelPrivate->_errorReason);

    G_OBJECT_CLASS(simple_notes_base_model_parent_class)->dispose(object);
}

static void simple_notes_base_model_class_init (SimpleNotesBaseModelClass *klass) {

    g_type_add_instance_private(SIMPLE_NOTES_TYPE_BASE_MODEL, sizeof(SimpleNotesBaseModelPrivate));

    GObjectClass *objectClass = G_OBJECT_CLASS(klass);
    objectClass->dispose = simple_notes_base_model_dispose;

    klass->load_data = simple_notes_base_model_real_load_data;
    klass->load = simple_notes_base_model_real_load;
    klass->changed = simple_notes_base_model_real_changed;
    klass->reset = simple_notes_base_model_real_reset;
    klass->cancel = simple_notes_base_model_real_cancel;

    objectClass->set_property = simple_notes_base_model_set_property;
    objectClass->get_property = simple_notes_base_model_get_property;

    objProperties[PROP_LOADING] =
            g_param_spec_boolean(
                    kLoading,
                    "Loading",
                    "Loading flag.",
                    FALSE  /* default value */,
                    G_PARAM_READWRITE
    );

    objProperties[PROP_NEW_DATA] =
            g_param_spec_boolean(
                    kNewData,
                    "New Data",
                    "New data flag.",
                    FALSE,
                    G_PARAM_READWRITE
            );
    objProperties[PROP_ERROR_REASON] =
            g_param_spec_string(
                    kErrorReason,
                    "Error Reason",
                    "Error reason of loading data.",
                    NULL,
                    G_PARAM_READWRITE
            );

    g_object_class_install_properties(
            objectClass,
            N_PROPERTIES,
            objProperties
    );
}

static void simple_notes_base_model_init (SimpleNotesBaseModel *object) {
    SimpleNotesBaseModelPrivate *modelPrivate = simple_notes_base_model_get_instance_private(object);
    modelPrivate->_loading = FALSE;
    modelPrivate->_newData = FALSE;
    modelPrivate->_errorReason = NULL;
}

void simple_notes_base_model_load_data (SimpleNotesBaseModel *object) {
    SimpleNotesBaseModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC(object, &klass, load_data, SimpleNotesBaseModel, SIMPLE_NOTES, BASE_MODEL);
    klass->load_data(object);
}

void simple_notes_base_model_reset (SimpleNotesBaseModel *object) {
    SimpleNotesBaseModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC(object, &klass, reset, SimpleNotesBaseModel, SIMPLE_NOTES, BASE_MODEL);
    klass->reset(object);
}

void simple_notes_base_model_cancel (SimpleNotesBaseModel *object) {
    SimpleNotesBaseModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC(object, &klass, cancel, SimpleNotesBaseModel, SIMPLE_NOTES, BASE_MODEL);
    klass->cancel(object);
}

gboolean simple_notes_base_model_get_loading (SimpleNotesBaseModel *object) {
    gboolean value;
    g_object_get(G_OBJECT(object), kLoading, &value, NULL);
    return value;
}

void simple_notes_base_model_assign_loading (SimpleNotesBaseModel *object, gboolean loading) {
    g_object_set(G_OBJECT(object), kLoading, loading, NULL);
}

gboolean simple_notes_base_model_get_new_data (SimpleNotesBaseModel *object) {
    gboolean value;
    g_object_get(G_OBJECT(object), kNewData, &value, NULL);
    return value;
}

void simple_notes_base_model_assign_new_data (SimpleNotesBaseModel *object, gboolean newData) {
    g_object_set(G_OBJECT(object), kNewData, newData, NULL);
}

gchar *simple_notes_base_model_get_copy_error_reason (SimpleNotesBaseModel *object) {
    gchar *copy;
    g_object_get(G_OBJECT(object), kErrorReason, &copy, NULL);
    return copy;
}

void simple_notes_base_model_copy_error_reason (SimpleNotesBaseModel *object, gchar *errorReason) {
    g_object_set(G_OBJECT(object), kErrorReason, errorReason, NULL);
}

static void simple_notes_base_model_load (SimpleNotesBaseModel *object) {
    SimpleNotesBaseModelClass *klass;

    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC(object, &klass, load, SimpleNotesBaseModel, SIMPLE_NOTES, BASE_MODEL);

    klass->load(object);
}

static void simple_notes_base_model_changed (SimpleNotesBaseModel *object) {
    SimpleNotesBaseModelClass *klass;

    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC(object, &klass, changed, SimpleNotesBaseModel, SIMPLE_NOTES, BASE_MODEL);

    klass->changed(object);
}

static void simple_notes_base_model_real_load_data (SimpleNotesBaseModel *object) {
    simple_notes_base_model_assign_loading(object, TRUE);
    simple_notes_base_model_load(object);
    simple_notes_base_model_assign_new_data(object, TRUE);
    simple_notes_base_model_assign_loading(object, FALSE);
}

static void simple_notes_base_model_real_load (SimpleNotesBaseModel *object) {

}

static void simple_notes_base_model_real_changed (SimpleNotesBaseModel *object) {
    
}

static void simple_notes_base_model_real_reset (SimpleNotesBaseModel *object)  {

}

static void simple_notes_base_model_real_cancel (SimpleNotesBaseModel *object)  {

}
