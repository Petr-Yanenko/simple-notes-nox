//
//  selected_list_model.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/31/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "selected_list_model.h"
#include "model_utility.h"
#include "mediator.h"
#include "stdint.h"

enum {
    PROP_SELECTED_OBJECT_ID = 1,
    N_PROPERTIES
};

typedef struct {
    SimpleNotesMediator *_weak_mediator;
} SimpleNotesSelectedListModelPrivate;

static GParamSpec *objProperties[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesSelectedListModel, simple_notes_selected_list_model, SIMPLE_NOTES_TYPE_LIST_MODEL)

static SimpleNotesObject *simple_notes_selected_list_model_real_get_object (SimpleNotesSelectedListModel *object, guint position);
static SimpleNotesObject **simple_notes_selected_list_model_real_copy_items (SimpleNotesSelectedListModel *object, guint *out_count);
static void simple_notes_selected_list_model_real_append_object (SimpleNotesSelectedListModel *object, SimpleNotesObject *item);
static void simple_notes_selected_list_model_real_delete_object (SimpleNotesSelectedListModel *object, SimpleNotesObject *item);
static void simple_notes_selected_list_model_real_changed (SimpleNotesBaseModel *object);

/*static SimpleNotesObject *simple_notes_selected_list_model_find_object_with_condition (
        SimpleNotesSelectedListModel *object, gboolean (^condition) (SimpleNotesObject *item)
	);*/

static void simple_notes_base_model_set_property (
        GObject      *object,
        guint         property_id,
        const GValue *value,
        GParamSpec   *pspec
) {
    SimpleNotesSelectedListModel *self = SIMPLE_NOTES_SELECTED_LIST_MODEL(object);
    guint64 objectID;

    switch (property_id)
    {
        case PROP_SELECTED_OBJECT_ID:
            objectID = g_value_get_uint64(value);
            SimpleNotesObject *item = simple_notes_selected_list_model_find_object(self, objectID);
            if (item) {
	      SimpleNotesObject *previousSelected = simple_notes_selected_list_model_find_selected_object(self);
                if (previousSelected) {
                    simple_notes_object_assign_selected(previousSelected, FALSE);
                }
                simple_notes_object_assign_selected(item, TRUE);

                glong const size = 2;
                glong count = size;
                SimpleNotesObject *items[2] = { item, previousSelected };
                
                if (!previousSelected) {
                    count--;
                }
                simple_notes_selected_list_model_save_objects(self, items, count);
            } else {
                g_printerr("\nSelected object %llu is missing\n", objectID);
            }
            break;

        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void simple_notes_base_model_get_property (
        GObject    *object,
        guint       property_id,
        GValue     *value,
        GParamSpec *pspec
) {
    SimpleNotesSelectedListModel *self = SIMPLE_NOTES_SELECTED_LIST_MODEL(object);

    guint64 objectID = 0;
    SimpleNotesObject *selectedObject = NULL;

    switch (property_id)
    {
        case PROP_SELECTED_OBJECT_ID:
	  selectedObject = simple_notes_selected_list_model_find_selected_object(self);
            if (selectedObject) {
                objectID = simple_notes_object_get_id(selectedObject);
            }
            g_value_set_uint64(value, objectID);
            break;

        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void simple_notes_selected_list_model_dispose (GObject *object) {
    SimpleNotesSelectedListModel *self = SIMPLE_NOTES_SELECTED_LIST_MODEL(object);
    SimpleNotesSelectedListModelPrivate *private = simple_notes_selected_list_model_get_instance_private(self);
    g_object_remove_weak_pointer(object, (gpointer *)&(private->_weak_mediator));

    G_OBJECT_CLASS(simple_notes_selected_list_model_parent_class)->dispose(object);
}

static void simple_notes_selected_list_model_class_init (SimpleNotesSelectedListModelClass *klass) {
    g_type_add_instance_private(SIMPLE_NOTES_TYPE_SELECTED_LIST_MODEL, sizeof(SimpleNotesSelectedListModelPrivate));

    GObjectClass *objectClass = G_OBJECT_CLASS(klass);
    SimpleNotesBaseModelClass *baseModelClass = SIMPLE_NOTES_BASE_MODEL_CLASS(klass);

    klass->get_object = simple_notes_selected_list_model_real_get_object;
    klass->copy_items = simple_notes_selected_list_model_real_copy_items;
    klass->append_object = simple_notes_selected_list_model_real_append_object;
    klass->delete_object = simple_notes_selected_list_model_real_delete_object;
    klass->save_objects = NULL;

    baseModelClass->changed = simple_notes_selected_list_model_real_changed;

    objectClass->set_property = simple_notes_base_model_set_property;
    objectClass->get_property = simple_notes_base_model_get_property;
    objectClass->dispose = simple_notes_selected_list_model_dispose;

    objProperties[PROP_SELECTED_OBJECT_ID] =
            g_param_spec_uint64(
                    kSelectedObjectID,
                    "Selected Object ID",
                    "Selected Object ID.",
                    0,
                    UINT64_MAX,
                    0  /* default value */,
                    G_PARAM_READWRITE
            );

    g_object_class_install_properties (
            objectClass,
            N_PROPERTIES,
            objProperties
    );
}

static void simple_notes_selected_list_model_init (SimpleNotesSelectedListModel *object) {
    SimpleNotesSelectedListModelPrivate *private = simple_notes_selected_list_model_get_instance_private(object);
    private->_weak_mediator = NULL;
    simple_notes_base_model_load_data(SIMPLE_NOTES_BASE_MODEL(object));
}

void simple_notes_selected_list_model_set_weak_ref_mediator (SimpleNotesSelectedListModel *object, SimpleNotesMediator *mediator) {
    SimpleNotesSelectedListModelPrivate *private = simple_notes_selected_list_model_get_instance_private(object);
    private->_weak_mediator = mediator;
    g_object_add_weak_pointer(G_OBJECT(object), (gpointer *)&(private->_weak_mediator));
}

SimpleNotesObject *simple_notes_selected_list_model_get_object (SimpleNotesSelectedListModel *object, guint position) {
    SimpleNotesSelectedListModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC_WITH_RETURN_VAL(
    object,
    &klass,
    get_object,
    SimpleNotesSelectedListModel,
    SIMPLE_NOTES,
    SELECTED_LIST_MODEL,
    NULL
    );
    return klass->get_object(object, position);
}

SimpleNotesObject **simple_notes_selected_list_model_copy_items (SimpleNotesSelectedListModel *object, guint *out_count) {
    SimpleNotesSelectedListModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC_WITH_RETURN_VAL(
    object,
    &klass,
    copy_items,
    SimpleNotesSelectedListModel,
    SIMPLE_NOTES,
    SELECTED_LIST_MODEL,
    NULL
    );
    return klass->copy_items(object, out_count);
}

void simple_notes_selected_list_model_append_object (SimpleNotesSelectedListModel *object, SimpleNotesObject *item) {
    SimpleNotesSelectedListModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC(
    object,
    &klass,
    append_object,
    SimpleNotesSelectedListModel,
    SIMPLE_NOTES,
    SELECTED_LIST_MODEL
    );
    klass->append_object(object, item);
}

void simple_notes_selected_list_model_delete_object (SimpleNotesSelectedListModel *object, SimpleNotesObject *item) {
    SimpleNotesSelectedListModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC(
    object,
    &klass,
    delete_object,
    SimpleNotesSelectedListModel,
    SIMPLE_NOTES,
    SELECTED_LIST_MODEL
    );
    klass->delete_object(object, item);
}

void simple_notes_selected_list_model_save_objects (
        SimpleNotesSelectedListModel *object,
        SimpleNotesObject *items[],
        glong count
) {
    SimpleNotesSelectedListModelClass *klass;
    SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC(
    object,
    &klass,
    save_objects,
    SimpleNotesSelectedListModel,
    SIMPLE_NOTES,
    SELECTED_LIST_MODEL
    );
    klass->save_objects(object, items, count);
}

guint64 simple_notes_selected_list_model_get_selected_object_id (SimpleNotesSelectedListModel *object) {
    guint64 value;
    g_object_get(G_OBJECT(object), kSelectedObjectID, &value, NULL);
    return value;
}

void simple_notes_selected_list_model_assign_selected_object_id (SimpleNotesSelectedListModel *object, guint64 objectID) {
    g_object_set(G_OBJECT(object), kSelectedObjectID, objectID, NULL);
}

SimpleNotesObject *simple_notes_selected_list_model_find_object (SimpleNotesSelectedListModel *object, guint64 objectID) {
    return NULL/*simple_notes_selected_list_model_find_object_with_condition(object, ^gboolean(SimpleNotesObject *item) {
        return simple_notes_object_get_id(item) == objectID;
	})*/;
}

SimpleNotesObject *simple_notes_selected_list_model_find_selected_object (SimpleNotesSelectedListModel *object) {
    return NULL/*simple_notes_selected_list_model_find_object_with_condition(object, ^gboolean(SimpleNotesObject *item) {
        return simple_notes_object_get_selected(item);
	})*/;
}

/*static SimpleNotesObject *simple_notes_selected_list_model_find_object_with_condition (
        SimpleNotesSelectedListModel *object, gboolean (^condition) (SimpleNotesObject *item)
) {
    GList *item = simple_notes_list_model_get_item(SIMPLE_NOTES_LIST_MODEL(object), 0);
    SimpleNotesObject *foundObject = NULL;
    while (item) {
        SimpleNotesObject *data = SIMPLE_NOTES_OBJECT(item->data);
        if (condition(data)) {
            foundObject = data;
            break;
        }
        item = item->next;
    }
    return foundObject;
    }*/

static SimpleNotesObject *simple_notes_selected_list_model_real_get_object (SimpleNotesSelectedListModel *object, guint position) {
    GList *item = simple_notes_list_model_get_item(SIMPLE_NOTES_LIST_MODEL(object), position);
    if (item) {
        return SIMPLE_NOTES_OBJECT(item->data);
    }
    return NULL;
}

static SimpleNotesObject **simple_notes_selected_list_model_real_copy_items (SimpleNotesSelectedListModel *object, guint *out_count) {
    void **items;
    SIMPLE_NOTES_COPY_DATA(object, &items, out_count);
    return (SimpleNotesObject **)items;
}

static void simple_notes_selected_list_model_real_append_object (SimpleNotesSelectedListModel *object, SimpleNotesObject *item) {
    simple_notes_list_model_append(SIMPLE_NOTES_LIST_MODEL(object), item);
}

static void simple_notes_selected_list_model_real_delete_object (SimpleNotesSelectedListModel *object, SimpleNotesObject *item) {
    simple_notes_list_model_delete(SIMPLE_NOTES_LIST_MODEL(object), item);
}

static void simple_notes_selected_list_model_real_changed (SimpleNotesBaseModel *object) {
    SIMPLE_NOTES_BASE_MODEL_CLASS(simple_notes_selected_list_model_parent_class)->changed(object);

    SimpleNotesSelectedListModel *self = SIMPLE_NOTES_SELECTED_LIST_MODEL(object);
    g_return_if_fail(self);

    SimpleNotesSelectedListModelPrivate *private = simple_notes_selected_list_model_get_instance_private(self);
    simple_notes_mediator_model_changed(private->_weak_mediator, object);
}
