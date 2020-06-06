//
//  note_controller.c
//  simple_notes
//
//  Created by Petr Yanenko on 3/17/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "note_controller.h"
#include "commands.h"
#include "store.h"
#include <stdlib.h>


struct _SimpleNotesNoteController {
  SimpleNotesListController _parent;

  SNNotesModel *_unsafe_model;
};

G_DEFINE_TYPE(SimpleNotesNoteController, simple_notes_note_controller, SIMPLE_NOTES_TYPE_LIST_CONTROLLER)


static gchar *simple_notes_note_controller_real_create_command_name (SimpleNotesBaseController *object);
static gboolean simple_notes_note_controller_real_perform_command (SimpleNotesBaseController *object, gchar *command, GHashTable *options);

void simple_notes_note_controller_class_init (SimpleNotesNoteControllerClass *klass) {
    SimpleNotesBaseControllerClass *baseClass = SIMPLE_NOTES_BASE_CONTROLLER_CLASS(klass);
    baseClass->create_command_name = simple_notes_note_controller_real_create_command_name;
    baseClass->perform_command = simple_notes_note_controller_real_perform_command;
}

void simple_notes_note_controller_init (SimpleNotesNoteController *object) {}

SimpleNotesNoteController *simple_notes_note_controller_new (SNNotesModel *model) {
    SimpleNotesNoteController *controller = g_object_new(SIMPLE_NOTES_TYPE_NOTE_CONTROLLER, NULL);
    SNBaseModel *baseModel = SN_BASE_MODEL(model);
    simple_notes_base_controller_ref_model (SIMPLE_NOTES_BASE_CONTROLLER(controller), baseModel);
    controller->_unsafe_model = model;

    return controller;
}

static gchar *simple_notes_note_controller_real_create_command_name (SimpleNotesBaseController *object) {
    return sn_copy_string("note\0");
}

static gboolean simple_notes_note_controller_real_perform_command (SimpleNotesBaseController *object, gchar *command, GHashTable *options) {
  SNNotesModel *model = SIMPLE_NOTES_NOTE_CONTROLLER(object)->_unsafe_model;
  if (model) {
    void *value = NULL;
    if (g_hash_table_lookup_extended(options, kAllOption, NULL, &value)) {
      sn_base_model_reset(SN_BASE_MODEL(model));
      sn_base_model_load_data(SN_BASE_MODEL(model));
      return TRUE;
    } else if (g_hash_table_lookup_extended(options, kInsertOption, NULL, &value)) {
      sn_notes_model_insert_note(model, value);
      return TRUE;
    } else if (g_hash_table_lookup_extended(options, kDeleteOption, NULL, &value)) {
      sn_notes_model_delete_note(model, strtoull(value, NULL, 0));
      return TRUE;
    } else if (g_hash_table_lookup_extended(options, kSelectOption, NULL, &value)) {
      sn_notes_model_select_note(model, strtoull(value, NULL, 0));
      return TRUE;
    } else if (g_hash_table_lookup_extended(options, kEditOption, NULL, &value)) {
      SNStore *store = sn_store_get_instance();
      GFile *note = sn_store_create_note_for_editing(store);
      gchar *path = g_file_get_path(note);
      g_object_unref(note);
      GError *error = NULL;
      GSubprocess *vim = g_subprocess_new(G_SUBPROCESS_FLAGS_STDIN_INHERIT, &error, "vim", path, NULL);
      g_free(path);
      SN_RETURN_VAL_IF_FAIL(vim, FALSE, NULL);
      gboolean vimWait = g_subprocess_wait(vim, NULL, &error);
      g_object_unref(vim);
      SN_RETURN_VAL_IF_FAIL(vimWait, FALSE, NULL);
      sn_store_end_editing(store);

      return TRUE;
    }
  }
    return FALSE;
}
