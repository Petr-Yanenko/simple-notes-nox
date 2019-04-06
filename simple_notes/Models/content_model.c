//
//  content_model.c
//  simple_notes
//
//  Created by Petr Yanenko on 12/1/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "content_model.h"
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "model_utility.h"
#include "mediator.h"
#include "notes_string.h"

struct _SimpleNotesContentModel {
    SimpleNotesListModel parent;

    guint64 _noteID;
    guint64 _folderID;
    GFile *_note;

    SimpleNotesMediator *_weak_mediator;
};

G_DEFINE_TYPE(SimpleNotesContentModel, simple_notes_content_model, SIMPLE_NOTES_TYPE_LIST_MODEL)

static GList *simple_notes_content_model_real_create_items (SimpleNotesListModel *object);
static void simple_notes_content_model_real_changed (SimpleNotesBaseModel *object);
static void simple_notes_content_model_save_changes (SimpleNotesContentModel *object);
static void simple_notes_content_model_changed (SimpleNotesContentModel *object);

static void simple_notes_content_model_dispose (GObject *object) {
    SimpleNotesContentModel *contentModel = SIMPLE_NOTES_CONTENT_MODEL(object);
    g_object_unref(contentModel->_note);
    g_object_remove_weak_pointer(object, (gpointer *)&(contentModel->_weak_mediator));

    G_OBJECT_CLASS(simple_notes_content_model_parent_class)->dispose(object);
}

static void simple_notes_content_model_class_init (SimpleNotesContentModelClass *klass) {
    GObjectClass *objectClass = G_OBJECT_CLASS(klass);
    objectClass->dispose = simple_notes_content_model_dispose;

    SimpleNotesBaseModelClass *baseClass = SIMPLE_NOTES_BASE_MODEL_CLASS(klass);
    baseClass->changed = simple_notes_content_model_real_changed;

    SimpleNotesListModelClass *listModelClass = SIMPLE_NOTES_LIST_MODEL_CLASS(klass);
    listModelClass->create_items = simple_notes_content_model_real_create_items;
}

static void simple_notes_content_model_init (SimpleNotesContentModel *object) {
    object->_note = NULL;
    object->_noteID = 0;
    object->_folderID = 0;

    object->_weak_mediator = NULL;

    GFile *folder = g_file_new_for_path(kNoteFolder);
    g_file_make_directory(folder, NULL, NULL);
    g_object_unref(folder);
}

SimpleNotesContentModel *simple_notes_content_model_new (
                                                         guint64 noteID,
                                                         guint64 folderID,
                                                         SimpleNotesMediator *mediator
                                                         ) {
    SimpleNotesContentModel *instance = g_object_new(SIMPLE_NOTES_TYPE_CONTENT_MODEL, NULL);
    simple_notes_content_model_change_note_id(instance, noteID, folderID);

    instance->_weak_mediator = mediator;
    g_object_add_weak_pointer(G_OBJECT(instance), (gpointer *)&(instance->_weak_mediator));
    return instance;
}

void simple_notes_content_model_change_note_id (
                                                SimpleNotesContentModel *object,
                                                guint64 noteID,
                                                guint64 folderID
                                                ) {
    object->_noteID = noteID;
    object->_folderID = folderID;
    
    if (folderID) {
        gchar folderPath[kFolderPathSymbols + 1];
        sprintf(folderPath, kFolderPathFormat, kNoteFolder, folderID);
        GFile *folder = g_file_new_for_path(folderPath);
        g_file_make_directory(folder, NULL, NULL);
        g_object_unref(folder);
    }
    
    gchar notePath[kNotePathSymbols + 1];
    sprintf(notePath, kNotePathFormat, kNoteFolder, folderID, noteID);
    g_clear_object(&(object->_note));
    object->_note = g_file_new_for_path(notePath);
    SimpleNotesBaseModel *baseModel = SIMPLE_NOTES_BASE_MODEL(object);
    simple_notes_base_model_reset(baseModel);
    simple_notes_base_model_load_data(baseModel);
}

void simple_notes_content_model_insert_text (
        SimpleNotesContentModel *object,
        guint8 *text,
        gulong textLen,
        gulong location,
        gulong len,
        SimpleNotesStringStyle *style
) {
    if (object->_noteID == 0) return;

    guint count = 0;
    SimpleNotesLightString **items = simple_notes_content_model_copy_items(object, &count);
    if (style == NULL) {
        GByteArray *string = NULL;
        if (count) {
            string = simple_notes_light_string_get_text(items[0]);
        } else {
            string = g_byte_array_sized_new(simple_notes_gulong_guint_cast(textLen));
        }
        if (len) {
            g_return_if_fail(string->len >= location + len);
            g_byte_array_remove_range(string, simple_notes_gulong_guint_cast(location), simple_notes_gulong_guint_cast(len));
        }
        gulong const currentLen = string->len;
        if (location == 0) {
            g_byte_array_prepend(string, text, simple_notes_gulong_guint_cast(textLen));
        } else if (location == currentLen) {
            g_byte_array_append(string, text, simple_notes_gulong_guint_cast(textLen));
        } else {
            g_return_if_fail(location < string->len);
            gulong remainderLen = string->len - location;
            guint8 *remainder = g_malloc(remainderLen);
            memcpy(remainder, &(string->data[location]), remainderLen);
            g_byte_array_remove_range(string, simple_notes_gulong_guint_cast(location), simple_notes_gulong_guint_cast(remainderLen));
            g_byte_array_append(string, text, simple_notes_gulong_guint_cast(textLen));
            g_byte_array_append(string, remainder, simple_notes_gulong_guint_cast(remainderLen));
            g_free(remainder);
        }
        if (count) {
            simple_notes_string_ref_text(SIMPLE_NOTES_STRING(items[0]), string);
        } else {
            SimpleNotesString *item = simple_notes_string_new(SimpleNotesStringTypeIdentifierText);
            simple_notes_string_ref_text(item, string);
            simple_notes_list_model_append(SIMPLE_NOTES_LIST_MODEL(object), item);
            g_object_unref(item);
            g_byte_array_unref(string);
        }
        simple_notes_content_model_save_changes(object);
    }
    simple_notes_free_objects_array((gpointer *)items, count);
}

SimpleNotesLightString **simple_notes_content_model_copy_items (SimpleNotesContentModel *object, guint *out_count) {
    void **items;
    SIMPLE_NOTES_COPY_DATA(object, &items, out_count);
    return (SimpleNotesLightString **)items;
}

guint64 simple_notes_content_model_get_note_id (SimpleNotesContentModel *object) {
    return object->_noteID;
}

static void simple_notes_content_model_changed (SimpleNotesContentModel *object) {
    simple_notes_model_changed(SIMPLE_NOTES_BASE_MODEL(object));
}

static void simple_notes_content_model_real_changed (SimpleNotesBaseModel *object) {
    SIMPLE_NOTES_BASE_MODEL_CLASS(simple_notes_content_model_parent_class)->changed(object);

    SimpleNotesContentModel *self = SIMPLE_NOTES_CONTENT_MODEL(object);
    g_return_if_fail(self);

    simple_notes_mediator_model_changed(self->_weak_mediator, object);
}

static void simple_notes_content_model_save_changes (SimpleNotesContentModel *object) {
    g_return_if_fail(object->_note);

    GError *error = NULL;
    gchar *xml = NULL;
    GList *list = simple_notes_list_model_get_item(SIMPLE_NOTES_LIST_MODEL(object), 0);
    for (GList *item = list; item != NULL; item = item->next) {
        gchar *stringXML = simple_notes_string_create_xml(SIMPLE_NOTES_STRING(item->data));
        if (!xml) {
            xml = stringXML;
        } else {
            gchar *buff = g_malloc(strlen(xml) + strlen(stringXML) + 1);
            strcpy(buff, xml);
            strcat(buff, stringXML);
            g_free(xml);
            g_free(stringXML);
            xml = buff;
        }
    }

    if (xml) {
        gulong len = strlen(xml);
        if (!g_file_replace_contents(object->_note, xml, len, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL, &error)) {
            SIMPLE_NOTES_PRINT_ERROR(&error);
            g_clear_error(&error);
        }

        simple_notes_content_model_changed(object);
        g_free(xml);
    }
}

static GList *simple_notes_content_model_real_create_items (SimpleNotesListModel *object) {
    SimpleNotesContentModel *self = SIMPLE_NOTES_CONTENT_MODEL(object);
    g_return_val_if_fail(self, NULL);

    gchar *content = NULL;
    gsize len;
    GError *error = NULL;
    g_return_val_if_fail(self->_note, NULL);

    gboolean result = g_file_load_contents(self->_note, NULL, &content, &len, NULL, &error);
    SIMPLE_NOTES_TRY_AND_CLEAR_ERROR(result, &error);
    SimpleNotesParser *parser = simple_notes_parser_new();
    GList *data = simple_notes_parser_parse_and_create_content(parser, content, len, &error);
    if (error) {
        SIMPLE_NOTES_PRINT_ERROR(&error);
        SIMPLE_NOTES_CLEAR_ERROR(&error);
    }

    g_object_unref(parser);
    g_free(content);

    return data;
}
