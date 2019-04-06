//
//  list_controller.h
//  simple_notes
//
//  Created by Petr Yanenko on 3/25/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_list_controller____FILEEXTENSION___
#define simple_notes_list_controller____FILEEXTENSION___

#include "base_controller.h"

#define SIMPLE_NOTES_TYPE_LIST_CONTROLLER simple_notes_list_controller_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesListController, simple_notes_list_controller, SIMPLE_NOTES, LIST_CONTROLLER, SimpleNotesBaseController)

struct _SimpleNotesListControllerClass {
    SimpleNotesBaseControllerClass parent;
};

#endif
