//
//  table_window.h
//  simple_notes
//
//  Created by Petr Yanenko on 3/10/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_table_window____FILEEXTENSION___
#define simple_notes_table_window____FILEEXTENSION___

#include "window.h"
#include "table_view.h"
#include "label_cell.h"
#include "base_controller.h"
#include "commands.h"

typedef void (*SimpleNotesConnectCallback) (GObject *object, GParamSpec *pspec, gpointer user_data);

#define SIMPLE_NOTES_TYPE_TABLE_WINDOW simple_notes_table_window_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesTableWindow, simple_notes_table_window, SIMPLE_NOTES, TABLE_WINDOW, SimpleNotesWindow)

struct _SimpleNotesTableWindowClass {
    SimpleNotesWindowClass parent;

    //private
    SimpleNotesTableView * (*create_table) (SimpleNotesTableWindow *object);
    SimpleNotesTableViewCell * (*create_cell_for_row_column) (SimpleNotesTableWindow *object, gulong row, gulong column);
    SimpleNotesTableViewCell * (*create_header_for_column) (SimpleNotesTableWindow *object, gulong column);
    gulong (*get_rows_number) (SimpleNotesTableWindow *object);
    gulong (*get_columns_number) (SimpleNotesTableWindow *object);
    SimpleNotesBaseController * (*create_controller) (SimpleNotesTableWindow *object, SNBaseModel *model);
};

SimpleNotesTableWindow *simple_notes_table_window_new (
        GType type,
        SimpleNotesResponder *next,
        SNBaseModel *model
);

SimpleNotesLabelCell *simple_notes_table_window_create_cell (gchar *text, SimpleNotesLabelTextAlignment alignment);
SimpleNotesTableView *simple_notes_table_window_get_table (SimpleNotesTableWindow *object);
SNBaseModel *simple_notes_table_window_get_model (SimpleNotesTableWindow *object);


#endif
