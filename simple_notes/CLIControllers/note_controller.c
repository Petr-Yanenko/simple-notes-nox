//
//  note_controller.c
//  simple_notes
//
//  Created by Petr Yanenko on 3/17/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "note_controller.h"
#include "commands.h"
#include <stdlib.h>

struct _SimpleNotesNoteController {
    SimpleNotesListController parent;
};

G_DEFINE_TYPE(SimpleNotesNoteController, simple_notes_note_controller, SIMPLE_NOTES_TYPE_LIST_CONTROLLER)

static SimpleNotesNotesModel *simple_notes_note_controller_get_model (SimpleNotesNoteController *object);
static gchar *simple_notes_note_controller_real_create_command_name (SimpleNotesBaseController *object);
static gboolean simple_notes_note_controller_real_perform_command (SimpleNotesBaseController *object, gchar *command, GHashTable *options);

void simple_notes_note_controller_class_init (SimpleNotesNoteControllerClass *klass) {
    SimpleNotesBaseControllerClass *baseClass = SIMPLE_NOTES_BASE_CONTROLLER_CLASS(klass);
    baseClass->create_command_name = simple_notes_note_controller_real_create_command_name;
    baseClass->perform_command = simple_notes_note_controller_real_perform_command;
}

void simple_notes_note_controller_init (SimpleNotesNoteController *object) {}

SimpleNotesNoteController *simple_notes_note_controller_new (SimpleNotesMediator *model) {
    SimpleNotesNoteController *controller = g_object_new(SIMPLE_NOTES_TYPE_NOTE_CONTROLLER, NULL);
    simple_notes_base_controller_set_ref_model (SIMPLE_NOTES_BASE_CONTROLLER(controller), model);
    return controller;
}

static SimpleNotesNotesModel *simple_notes_note_controller_get_model (SimpleNotesNoteController *object) {
    SimpleNotesMediator *model = simple_notes_base_controller_get_model(SIMPLE_NOTES_BASE_CONTROLLER(object));
    return simple_notes_mediator_get_notes_model(model);
}

static gchar *simple_notes_note_controller_real_create_command_name (SimpleNotesBaseController *object) {
    return simple_notes_create_string("note\0");
}

static gboolean simple_notes_note_controller_real_perform_command (SimpleNotesBaseController *object, gchar *command, GHashTable *options) {
    SimpleNotesNotesModel *model = simple_notes_note_controller_get_model(SIMPLE_NOTES_NOTE_CONTROLLER(object));
    if (model) {
        void *value = NULL;
        if (g_hash_table_lookup_extended(options, kAllOption, NULL, &value)) {
            simple_notes_base_model_reset(SIMPLE_NOTES_BASE_MODEL(model));
            simple_notes_base_model_load_data(SIMPLE_NOTES_BASE_MODEL(model));
            return TRUE;
        } else if (g_hash_table_lookup_extended(options, kInsertOption, NULL, &value)) {
            simple_notes_notes_model_insert_note(model, value);
            return TRUE;
        } else if (g_hash_table_lookup_extended(options, kDeleteOption, NULL, &value)) {
            simple_notes_notes_model_delete_note(model, strtoull(value, NULL, 0));
            return TRUE;
        } else if (g_hash_table_lookup_extended(options, kSelectOption, NULL, &value)) {
            simple_notes_notes_model_select_note(model, strtoull(value, NULL, 0));
            return TRUE;
        } else if (g_hash_table_lookup_extended(options, kEditOption, NULL, &value)) {
            SimpleNotesObject *selectedObject = simple_notes_selected_list_model_find_selected_object(SIMPLE_NOTES_SELECTED_LIST_MODEL(model));
            SIMPLE_NOTES_TRY_WITHOUT_ERROR(selectedObject);
            SimpleNotesNote *selectedNote = SIMPLE_NOTES_NOTE(selectedObject);
            GByteArray *content = simple_notes_note_get_copy_content(selectedNote);
            if (content->len) {
                content->data[content->len - 1] = '\n';
            } else {
                g_byte_array_append(content, (guint8 *)"\n", 1);
            }
            gchar *path = "simple_notes_temp.txt";
            GFile *temptFile = g_file_new_for_path(path);

            GError *error = NULL;
            gboolean replaced = g_file_replace_contents(temptFile, (char *)content->data, content->len, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL, &error);

            g_object_unref(temptFile);
            g_byte_array_unref(content);
            SIMPLE_NOTES_TRY_AND_CLEAR_ERROR(replaced, &error);

            GSubprocess *vim = g_subprocess_new(G_SUBPROCESS_FLAGS_STDIN_INHERIT, &error, "vim", path, NULL);
            SIMPLE_NOTES_TRY_AND_CLEAR_ERROR(vim, &error);
            gboolean vimWait = g_subprocess_wait(vim, NULL, &error);
            g_object_unref(vim);
            SIMPLE_NOTES_TRY_AND_CLEAR_ERROR(vimWait, &error);

            temptFile = g_file_new_for_path(path);
            gchar *buff = NULL;
            gsize len = 0;
            gboolean loadContent = g_file_load_contents(temptFile, NULL, &buff, &len, NULL, &error);
            g_object_unref(temptFile);
            if (!loadContent) {
                if (buff) {
                    g_free(buff);
                }
            }
            SIMPLE_NOTES_TRY_AND_CLEAR_ERROR(loadContent, &error);

            if (len) {
                buff[len - 1] = '\0';
            }
            simple_notes_notes_model_change_note_content(model, simple_notes_object_get_id(selectedObject), (guint8 *)buff, simple_notes_gulong_guint_cast(len));
            if (buff) {
                g_free(buff);
            }

            return TRUE;
        }
    }
    return FALSE;
}
