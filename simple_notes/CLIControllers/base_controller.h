//
//  base_controller.h
//  simple_notes
//
//  Created by Petr Yanenko on 2/3/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_base_controller____FILEEXTENSION___
#define simple_notes_base_controller____FILEEXTENSION___

#include "event.h"
#include "mediator.h"

typedef enum _SimpleNotesBaseControllerEventResult {
  SimpleNotesBaseControllerEventResultError = -1,
  SimpleNotesBaseControllerEventResultNotHandled = 0,
  SimpleNotesBaseControllerEventResultHandled = 1
} SimpleNotesBaseControllerEventResult;

#define SIMPLE_NOTES_TYPE_BASE_CONTROLLER simple_notes_base_controller_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesBaseController, simple_notes_base_controller, SIMPLE_NOTES, BASE_CONTROLLER, GObject)

struct _SimpleNotesBaseControllerClass {
  GObjectClass parent;

  //private
  gchar * (*create_command_name) (SimpleNotesBaseController *object);

  gchar ** (*create_options_names) (SimpleNotesBaseController *object);

  gchar *** (*create_required_options_names)(SimpleNotesBaseController *object);

  gboolean (*check_value_for_option) (SimpleNotesBaseController *object, gchar *option, gchar *value);

  gboolean (*perform_command) (SimpleNotesBaseController *object, gchar *command, GHashTable *options);
};

SimpleNotesBaseControllerEventResult simple_notes_base_controller_handle_event (
                                                                                SimpleNotesBaseController *object,
                                                                                SimpleNotesEvent *event,
                                                                                GError **error
                                                                                );

SimpleNotesMediator *simple_notes_base_controller_get_model (SimpleNotesBaseController *object);
void simple_notes_base_controller_set_ref_model (SimpleNotesBaseController *object, SimpleNotesMediator *model);

#endif
