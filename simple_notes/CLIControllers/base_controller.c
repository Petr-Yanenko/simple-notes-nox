//
//  base_controller.c
//  simple_notes
//
//  Created by Petr Yanenko on 2/3/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "base_controller.h"

typedef struct _SimpleNotesBaseControllerPrivate {
  gchar *_command;
  gchar **_options;
  gchar ***_requiredOptions;

  SNBaseModel *_model;
} SimpleNotesBaseControllerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesBaseController, simple_notes_base_controller, G_TYPE_OBJECT)

static gchar *simple_notes_base_controller_create_command_name (SimpleNotesBaseController *object);
static gchar **simple_notes_base_controller_create_options_names (SimpleNotesBaseController *object);
static gchar ***simple_notes_base_controller_create_required_options_names (SimpleNotesBaseController *object);
static gboolean simple_notes_base_controller_check_value_for_option (SimpleNotesBaseController *object, gchar *option, gchar *value);
static gboolean simple_notes_base_controller_perform_command (SimpleNotesBaseController *object, gchar *command, GHashTable *options);

void simple_notes_base_controller_dispose (GObject *object) {
  SimpleNotesBaseControllerPrivate *private = simple_notes_base_controller_get_instance_private(SIMPLE_NOTES_BASE_CONTROLLER(object));
  g_free(private->_command);
  if (private->_options) {
    g_strfreev(private->_options);
  }
  if (private->_requiredOptions) {
    gchar **item = NULL;
    for (gulong i = 0; (item = private->_requiredOptions[i]) != NULL; i++) {
      g_strfreev(item);
    }
    g_free(private->_requiredOptions);
  }

  if (private->_model)
    {
      g_clear_object(&private->_model);
    }

  G_OBJECT_CLASS(simple_notes_base_controller_parent_class)->dispose(object);
}

static void simple_notes_base_controller_constructed (GObject *object) {
    SimpleNotesBaseController *self = SIMPLE_NOTES_BASE_CONTROLLER(object);
    SimpleNotesBaseControllerPrivate *private = simple_notes_base_controller_get_instance_private(self);
    
    private->_command = simple_notes_base_controller_create_command_name(self);
    private->_options = simple_notes_base_controller_create_options_names(self);
    private->_requiredOptions = simple_notes_base_controller_create_required_options_names(self);
    
    G_OBJECT_CLASS(simple_notes_base_controller_parent_class)->constructed(object);
}

void simple_notes_base_controller_class_init (SimpleNotesBaseControllerClass *klass) {
    g_type_add_instance_private(SIMPLE_NOTES_TYPE_BASE_CONTROLLER, sizeof(SimpleNotesBaseControllerPrivate));

    G_OBJECT_CLASS(klass)->dispose = simple_notes_base_controller_dispose;
    G_OBJECT_CLASS(klass)->constructed = simple_notes_base_controller_constructed;
}

void simple_notes_base_controller_init (SimpleNotesBaseController *object) {
    SimpleNotesBaseControllerPrivate *private = simple_notes_base_controller_get_instance_private(object);
    private->_command = NULL;
    private->_options = NULL;
    private->_requiredOptions = NULL;
    private->_model = NULL;
}

void simple_notes_base_controller_ref_model(SimpleNotesBaseController *self, SNBaseModel *model) {
  SimpleNotesBaseControllerPrivate *private = simple_notes_base_controller_get_instance_private(self);
  if (private->_model != model)
    {
      if (private->_model)
	{
	  g_object_unref(private->_model);
	}
      private->_model = g_object_ref(model);
    }
}

SimpleNotesBaseControllerEventResult simple_notes_base_controller_handle_event (
                                                                                SimpleNotesBaseController *object,
                                                                                SimpleNotesEvent *event,
                                                                                GError **error
                                                                                ) {
  SimpleNotesBaseControllerPrivate *private = simple_notes_base_controller_get_instance_private(object);
  GString *command = simple_notes_event_get_command(event);
  gchar *allowedCommand = private->_command;
  gint commandCmp = g_strcmp0(command->str, allowedCommand);
  if (commandCmp) {
    return SimpleNotesBaseControllerEventResultNotHandled;
  }

  gchar **allowedOptions = private->_options;
  GHashTable *options = simple_notes_event_get_options(event);
  if (g_hash_table_size(options) && allowedOptions == NULL) {
    simple_notes_create_error(error, 10, "This command have to be without options", 0);
    return SimpleNotesBaseControllerEventResultError;
  }
  GList *keys = g_hash_table_get_keys(options);
  GList *item = keys;
  while (item != NULL) {
    gchar *key = item->data;
    gboolean found = FALSE;
    for (gulong index = 0; allowedOptions[index] != NULL; index++) {
      if ((found = !g_strcmp0(key, allowedOptions[index]))) {
        break;
      }
    }
    if (!found) {
      simple_notes_create_error(error, 11, "Unknown option %s", 1, key);
      return SimpleNotesBaseControllerEventResultError;
    }
    gboolean valueCheck = simple_notes_base_controller_check_value_for_option(object, item->data, g_hash_table_lookup(options, item->data));
    if (!valueCheck) {
      simple_notes_create_error(error, 12, "Invalid value for option %s", 1, item->data);
      return SimpleNotesBaseControllerEventResultError;
    }
    item = item->next;
  }

  gchar ***requiredOptions = private->_requiredOptions;
  if (requiredOptions) {
    gchar **requiredItem = NULL;
    gboolean found = FALSE;
    for (gulong i = 0; (requiredItem = requiredOptions[i]) != NULL; i++) {
      gchar *requiredOption = NULL;
      found = FALSE;
      gulong j = 0;
      for (; (requiredOption = requiredItem[j]) != NULL; j++) {
        GList *item = keys;
        gboolean optionFound = FALSE;
        while (item != NULL) {
          if ((optionFound = !g_strcmp0(requiredOption, item->data))) {
            break;
          }
          item = item->next;
        }
        found = j == 0 ? optionFound : found && optionFound;
      }
      found = found && (j == g_list_length(keys));
      if (found) {
        break;
      }
    }
    if (!found) {
      simple_notes_create_error(error, 13, "Invalid syntax", 0, NULL);
      return SimpleNotesBaseControllerEventResultError;
    }
  }
  gboolean result = simple_notes_base_controller_perform_command(object, command->str, options);
  if (!result) {
      simple_notes_create_error(error, 14, "Command not handled", 0, NULL);
  }
  return result ? SimpleNotesBaseControllerEventResultHandled : SimpleNotesBaseControllerEventResultError;
}

static gchar *simple_notes_base_controller_create_command_name (SimpleNotesBaseController *object) {
    SimpleNotesBaseControllerClass *klass;
    SN_GET_CLASS_OR_RETURN_VAL(object, &klass, create_command_name, SimpleNotesBaseController, SIMPLE_NOTES, BASE_CONTROLLER, NULL);
    return klass->create_command_name(object);
}

static gchar **simple_notes_base_controller_create_options_names (SimpleNotesBaseController *object) {
    SimpleNotesBaseControllerClass *klass;
    SN_GET_CLASS_OR_RETURN_VAL(object, &klass, create_options_names, SimpleNotesBaseController, SIMPLE_NOTES, BASE_CONTROLLER, NULL);
    return klass->create_options_names(object);
}

static gchar ***simple_notes_base_controller_create_required_options_names (SimpleNotesBaseController *object) {
    SimpleNotesBaseControllerClass *klass;
    SN_GET_CLASS_OR_RETURN_VAL(object, &klass, create_required_options_names, SimpleNotesBaseController, SIMPLE_NOTES, BASE_CONTROLLER, NULL);
    return klass->create_required_options_names(object);
}

static gboolean simple_notes_base_controller_check_value_for_option (SimpleNotesBaseController *object, gchar *option, gchar *value) {
    SimpleNotesBaseControllerClass *klass;
    SN_GET_CLASS_OR_RETURN_VAL(object, &klass, check_value_for_option, SimpleNotesBaseController, SIMPLE_NOTES, BASE_CONTROLLER, FALSE);
    return klass->check_value_for_option(object, option, value);
}

static gboolean simple_notes_base_controller_perform_command (SimpleNotesBaseController *object, gchar *command, GHashTable *options) {
    SimpleNotesBaseControllerClass *klass;
    SN_GET_CLASS_OR_RETURN_VAL(object, &klass, perform_command, SimpleNotesBaseController, SIMPLE_NOTES, BASE_CONTROLLER, FALSE);
    return klass->perform_command(object, command, options);
}
