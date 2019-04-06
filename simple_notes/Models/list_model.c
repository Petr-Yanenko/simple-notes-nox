//
//  list_model.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/25/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "list_model.h"

typedef struct {
    GList *_list;
} SimpleNotesListModelPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesListModel, simple_notes_list_model, SIMPLE_NOTES_TYPE_BASE_MODEL)

static GList *simple_notes_list_model_real_get_item (SimpleNotesListModel *object, guint position);
static void simple_notes_list_model_real_append (SimpleNotesListModel *object, gpointer item);
static void simple_notes_list_model_real_delete (SimpleNotesListModel *object, gpointer item);
static void simple_notes_list_model_real_load (SimpleNotesBaseModel *object);
static void simple_notes_list_model_real_reset (SimpleNotesBaseModel *object);
static void simple_notes_list_model_real_changed (SimpleNotesBaseModel *object);
static GList *simple_notes_list_model_real_create_items (SimpleNotesListModel *object);
static void simple_notes_list_model_delete_list (SimpleNotesListModelPrivate *modelPrivate);
static GList *simple_notes_list_model_create_items (SimpleNotesListModel *object);

static void simple_notes_list_model_dispose (GObject *object) {
    SimpleNotesListModel *self = SIMPLE_NOTES_LIST_MODEL(object);
    SimpleNotesListModelPrivate *modelPrivate = simple_notes_list_model_get_instance_private(self);
    simple_notes_list_model_delete_list(modelPrivate);
    G_OBJECT_CLASS(simple_notes_list_model_parent_class)->dispose(object);
}

static void simple_notes_list_model_class_init (SimpleNotesListModelClass *klass) {

    g_type_add_instance_private(SIMPLE_NOTES_TYPE_LIST_MODEL, sizeof(SimpleNotesListModelPrivate));

    GObjectClass *objectClass = G_OBJECT_CLASS(klass);
    objectClass->dispose = simple_notes_list_model_dispose;

    SimpleNotesBaseModelClass *baseModelClass = SIMPLE_NOTES_BASE_MODEL_CLASS(klass);
    baseModelClass->load = simple_notes_list_model_real_load;
    baseModelClass->reset = simple_notes_list_model_real_reset;
    baseModelClass->changed = simple_notes_list_model_real_changed;

    klass->get_item = simple_notes_list_model_real_get_item;
    klass->append = simple_notes_list_model_real_append;
    klass->delete =simple_notes_list_model_real_delete;
    klass->create_items = simple_notes_list_model_real_create_items;
}

static void simple_notes_list_model_init (SimpleNotesListModel *object) {
    SimpleNotesListModelPrivate *modelPrivate = simple_notes_list_model_get_instance_private(object);
    modelPrivate->_list = NULL;
}

void simple_notes_list_model_copy_list (SimpleNotesListModel *object, GList *list) {
    SimpleNotesListModelPrivate *modelPrivate = simple_notes_list_model_get_instance_private(object);
    simple_notes_list_model_delete_list(modelPrivate);
    modelPrivate->_list = g_list_copy_deep(list, (GCopyFunc)g_object_ref, NULL);
}

GList *simple_notes_list_model_get_item (SimpleNotesListModel *object, guint position) {
    SimpleNotesListModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC_WITH_RETURN_VAL(
            object,
            &klass,
            get_item,
            SimpleNotesListModel,
            SIMPLE_NOTES,
            LIST_MODEL,
            NULL
    );
    return klass->get_item(object, position);
}

GPtrArray *simple_notes_list_model_copy_data (SimpleNotesListModel *object, guint fromPosition) {
    GPtrArray *data = g_ptr_array_new();
    g_ptr_array_set_free_func(data, g_object_unref);
    GList *list = simple_notes_list_model_get_item(object, fromPosition);
    for (GList *l = list; l != NULL; l = l->next) {
        g_ptr_array_add(data, g_object_ref(l->data));
    }
    return data;
}

void simple_notes_list_model_append (SimpleNotesListModel *object, gpointer item) {
    SimpleNotesListModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC(object, &klass, append, SimpleNotesListModel, SIMPLE_NOTES, LIST_MODEL);
    klass->append(object, item);
}

void simple_notes_list_model_delete (SimpleNotesListModel *object, gpointer item) {
    SimpleNotesListModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC(object, &klass, delete, SimpleNotesListModel, SIMPLE_NOTES, LIST_MODEL);
    klass->delete(object, item);
}

glong simple_notes_list_model_get_count (SimpleNotesListModel *object) {
    SimpleNotesListModelPrivate *modelPrivate = simple_notes_list_model_get_instance_private(object);
    return g_list_length(modelPrivate->_list);
}

static GList *simple_notes_list_model_create_items (SimpleNotesListModel *object) {
    SimpleNotesListModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC_WITH_RETURN_VAL(
            object, &klass, create_items, SimpleNotesListModel, SIMPLE_NOTES, LIST_MODEL, NULL
    );
    return klass->create_items(object);
}

static GList *simple_notes_list_model_real_get_item (SimpleNotesListModel *object, guint position) {
    SimpleNotesListModelPrivate *modelPrivate = simple_notes_list_model_get_instance_private(object);
    return g_list_nth(modelPrivate->_list, position);
}

static void simple_notes_list_model_real_append (SimpleNotesListModel *object, gpointer item) {
    SimpleNotesListModelPrivate *modelPrivate = simple_notes_list_model_get_instance_private(object);
    modelPrivate->_list = g_list_append(modelPrivate->_list, g_object_ref(item));
}

static void simple_notes_list_model_real_delete (SimpleNotesListModel *object, gpointer item) {
    SimpleNotesListModelPrivate *modelPrivate = simple_notes_list_model_get_instance_private(object);
    modelPrivate->_list = g_list_remove(modelPrivate->_list, item);
    g_object_unref(item);
}

static void simple_notes_list_model_delete_list (SimpleNotesListModelPrivate *modelPrivate) {
    if (modelPrivate->_list) {
        g_list_free_full(modelPrivate->_list, g_object_unref);
        modelPrivate->_list = NULL;
    }
}

static void simple_notes_list_model_real_load (SimpleNotesBaseModel *object) {
    SimpleNotesListModel *self = SIMPLE_NOTES_LIST_MODEL(object);
    g_return_if_fail(self);

    GList *items = simple_notes_list_model_create_items(self);
    simple_notes_list_model_copy_list(SIMPLE_NOTES_LIST_MODEL(object), items);
    g_list_free_full(items, g_object_unref);
}

static void simple_notes_list_model_real_reset (SimpleNotesBaseModel *object)  {
    SimpleNotesListModel *listModel = SIMPLE_NOTES_LIST_MODEL(object);
    g_return_if_fail(listModel);

    simple_notes_list_model_copy_list(listModel, NULL);
}

static void simple_notes_list_model_real_changed (SimpleNotesBaseModel *object) {
    SIMPLE_NOTES_BASE_MODEL_CLASS(simple_notes_list_model_parent_class)->changed(object);

    SimpleNotesListModel *self = SIMPLE_NOTES_LIST_MODEL(object);
    g_return_if_fail(self);

    simple_notes_base_model_reset(object);
    simple_notes_base_model_load_data(object);
}

static GList *simple_notes_list_model_real_create_items (SimpleNotesListModel *object) {
    return NULL;
}
