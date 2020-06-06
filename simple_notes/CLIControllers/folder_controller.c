#include "folder_controller.h"
#include <stdlib.h>
#include "commands.h"

struct _SimpleNotesFolderController {
  SimpleNotesListController _parent;

  SNFoldersModel *_unsafe_model;
};

G_DEFINE_TYPE(SimpleNotesFolderController, simple_notes_folder_controller, SIMPLE_NOTES_TYPE_LIST_CONTROLLER)


static gchar *simple_notes_folder_controller_real_create_command_name (SimpleNotesBaseController *object);
static gboolean simple_notes_folder_controller_real_perform_command (SimpleNotesBaseController *object, gchar *command, GHashTable *options);

void simple_notes_folder_controller_class_init (SimpleNotesFolderControllerClass *klass) {
    SimpleNotesBaseControllerClass *baseClass = SIMPLE_NOTES_BASE_CONTROLLER_CLASS(klass);
    baseClass->create_command_name = simple_notes_folder_controller_real_create_command_name;
    baseClass->perform_command = simple_notes_folder_controller_real_perform_command;
}

void simple_notes_folder_controller_init (SimpleNotesFolderController *object) {}

SimpleNotesFolderController *simple_notes_folder_controller_new (SNFoldersModel *model) {
  SimpleNotesFolderController *controller = g_object_new(SIMPLE_NOTES_TYPE_FOLDER_CONTROLLER, NULL);
  SimpleNotesBaseController *base = SIMPLE_NOTES_BASE_CONTROLLER(controller);
  simple_notes_base_controller_ref_model(base, SN_BASE_MODEL(model));
  controller->_unsafe_model = model;

  return controller;
}

static gchar *simple_notes_folder_controller_real_create_command_name (SimpleNotesBaseController *object) {
  return sn_copy_string("folder\0");
}

static gboolean simple_notes_folder_controller_real_perform_command (SimpleNotesBaseController *object, gchar *command, GHashTable *options) {
  SNFoldersModel *model = SIMPLE_NOTES_FOLDER_CONTROLLER(object)->_unsafe_model;
  g_return_val_if_fail(model, FALSE);
  void *value = NULL;
  if (g_hash_table_lookup_extended(options, kAllOption, NULL, &value)) {
    sn_base_model_reset(SN_BASE_MODEL(model));
    sn_base_model_load_data(SN_BASE_MODEL(model));
    return TRUE;
  } else if (g_hash_table_lookup_extended(options, kInsertOption, NULL, &value)) {
    sn_folders_model_insert_folder(model, value);
    return TRUE;
  } else if (g_hash_table_lookup_extended(options, kDeleteOption, NULL, &value)) {
    sn_folders_model_delete_folder(model, strtoull(value, NULL, 0));
    return TRUE;
  } else if (g_hash_table_lookup_extended(options, kSelectOption, NULL, &value)) {
    sn_folders_model_select_folder(model, strtoull(value, NULL, 0));
    return TRUE;
  }
  return FALSE;
}
