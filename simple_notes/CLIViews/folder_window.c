//
//  folder_window.c
//  simple_notes
//
//  Created by Petr Yanenko on 1/21/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "folder_window.h"
#include "folder_controller.h"
#include "view_utility.h"

static glong const kTitleColumn = 2;
static glong const kNotesColumn = 3;

struct _SimpleNotesFolderWindow {
    SimpleNotesWindow parent;

    SimpleNotesLightFolder **_folders;
    gulong _count;
};

G_DEFINE_TYPE(SimpleNotesFolderWindow, simple_notes_folder_window, SIMPLE_NOTES_TYPE_TABLE_WINDOW)

static gulong simple_notes_folder_window_real_height_for_row (SimpleNotesTableView *object, gulong row);
static gulong simple_notes_folder_window_real_width_for_column (SimpleNotesTableView *object, gulong column);
static SimpleNotesTableView *simple_notes_folder_window_real_create_table (SimpleNotesTableWindow *object);
static SimpleNotesBaseController *simple_notes_folder_window_real_create_controller (SimpleNotesTableWindow *object, SimpleNotesMediator *model);
static SimpleNotesBaseModel *simple_notes_folder_window_real_get_model_to_connect_signal (SimpleNotesTableWindow *object, SimpleNotesMediator *model);
static gulong simple_notes_folder_window_real_get_rows_number (SimpleNotesTableWindow *object);
static gulong simple_notes_folder_window_real_get_columns_number (SimpleNotesTableWindow *object);
static SimpleNotesTableViewCell *simple_notes_folder_window_real_create_header_for_column (SimpleNotesTableWindow *object, gulong column);
static SimpleNotesTableViewCell *simple_notes_folder_window_real_create_cell_for_row_column (SimpleNotesTableWindow *object, gulong row, gulong column);

static void simple_notes_folder_window_dispose (GObject *object) {
    SimpleNotesFolderWindow *window = SIMPLE_NOTES_FOLDER_WINDOW(object);
    simple_notes_free_objects_array((gpointer *)window->_folders, window->_count);

    G_OBJECT_CLASS(simple_notes_folder_window_parent_class)->dispose(object);
}

void simple_notes_folder_window_class_init (SimpleNotesFolderWindowClass *klass) {
    SimpleNotesTableWindowClass *parentClass = SIMPLE_NOTES_TABLE_WINDOW_CLASS(klass);
    parentClass->create_table = simple_notes_folder_window_real_create_table;
    parentClass->create_controller = simple_notes_folder_window_real_create_controller;
    parentClass->get_model_to_connect_signal = simple_notes_folder_window_real_get_model_to_connect_signal;
    parentClass->get_rows_number = simple_notes_folder_window_real_get_rows_number;
    parentClass->get_columns_number = simple_notes_folder_window_real_get_columns_number;
    parentClass->create_header_for_column = simple_notes_folder_window_real_create_header_for_column;
    parentClass->create_cell_for_row_column = simple_notes_folder_window_real_create_cell_for_row_column;
    
    G_OBJECT_CLASS(klass)->dispose = simple_notes_folder_window_dispose;
}

void simple_notes_folder_window_init (SimpleNotesFolderWindow *object) {
    object->_folders = NULL;
    object->_count = 0;
}

SimpleNotesFolderWindow *simple_notes_folder_window_new (SimpleNotesResponder *next, SimpleNotesMediator *model) {
    SimpleNotesFolderWindow *window = SIMPLE_NOTES_FOLDER_WINDOW(simple_notes_table_window_new(
            SIMPLE_NOTES_TYPE_FOLDER_WINDOW,
            next,
            model
    ));
    return window;
}

static SimpleNotesTableView *simple_notes_folder_window_real_create_table (SimpleNotesTableWindow *object) {
    return simple_notes_table_view_new(
            NULL,
            0,
            0,
            simple_notes_folder_window_real_width_for_column,
            simple_notes_folder_window_real_height_for_row
    );
}

static SimpleNotesBaseController *simple_notes_folder_window_real_create_controller (SimpleNotesTableWindow *object, SimpleNotesMediator *model) {
    SimpleNotesFolderController *controller = simple_notes_folder_controller_new(model);
    return SIMPLE_NOTES_BASE_CONTROLLER(controller);
}

static SimpleNotesBaseModel *simple_notes_folder_window_real_get_model_to_connect_signal (SimpleNotesTableWindow *object, SimpleNotesMediator *model) {
    SimpleNotesFoldersModel *foldersModel = simple_notes_mediator_get_folders_model(model);
    return SIMPLE_NOTES_BASE_MODEL(foldersModel);
}

static gulong simple_notes_folder_window_real_height_for_row (SimpleNotesTableView *object, gulong row) {
    gulong width = simple_notes_folder_window_real_width_for_column(object, kTitleColumn);
    gulong height = simple_notes_height_for_row(object, row, kTitleColumn, width);
    return height;
}

static gulong simple_notes_folder_window_real_width_for_column (SimpleNotesTableView *object, gulong column) {
    if (column == kIdColumn) {
        return 25;
    } else if (column == kTitleColumn) {
        return 75;
    } else if (column == kNotesColumn) {
        return 15;
    }
    return 10;
}

static SimpleNotesTableViewCell *simple_notes_folder_window_real_create_header_for_column (SimpleNotesTableWindow *object, gulong column) {
  if (column == kIdColumn) {
            return SIMPLE_NOTES_TABLE_VIEW_CELL(simple_notes_table_window_create_cell("id\0", SimpleNotesLabelTextAlignmentRight));
  }
  else if (column == kSelectedColumn) {
            return SIMPLE_NOTES_TABLE_VIEW_CELL(simple_notes_table_window_create_cell("selected\0", SimpleNotesLabelTextAlignmentRight));
  }
  else if (column == kTitleColumn) {
            return SIMPLE_NOTES_TABLE_VIEW_CELL(simple_notes_table_window_create_cell("title\0", SimpleNotesLabelTextAlignmentLeft));
  }
  else if (column == kNotesColumn) {
            return SIMPLE_NOTES_TABLE_VIEW_CELL(simple_notes_table_window_create_cell("notes number\0", SimpleNotesLabelTextAlignmentRight));
  }
  else {
            return NULL;
    }
}

static SimpleNotesTableViewCell *simple_notes_folder_window_real_create_cell_for_row_column (SimpleNotesTableWindow *object, gulong row, gulong column) {
    SimpleNotesFolderWindow *window = SIMPLE_NOTES_FOLDER_WINDOW(object);
    SimpleNotesLabelCell *cell = NULL;
    if (column == kIdColumn) {
            gchar buff[kLongLongSymbols];
            guint64 identifier = simple_notes_light_folder_get_id(window->_folders[row]);
            simple_notes_print_guint64_value(buff, identifier);
            cell = simple_notes_table_window_create_cell(buff, SimpleNotesLabelTextAlignmentRight);
        }
    else if (column == kSelectedColumn) {
            gchar buff[kSelectedSymbols];
            simple_notes_print_boolean_value(buff, simple_notes_light_folder_get_selected(window->_folders[row]));
            cell = simple_notes_table_window_create_cell(buff, SimpleNotesLabelTextAlignmentRight);
        }
    else if (column == kTitleColumn) {
            GByteArray *title = simple_notes_light_folder_get_copy_title(window->_folders[row]);
            gchar *text = (gchar *)title->data;
            cell = simple_notes_table_window_create_cell(text, SimpleNotesLabelTextAlignmentLeft);
            g_byte_array_unref(title);
        }
    else if (column == kNotesColumn) {
            gchar buff[kLongLongSymbols];
            simple_notes_print_long_value(buff, simple_notes_light_folder_get_count(window->_folders[row]));
            cell = simple_notes_table_window_create_cell(buff, SimpleNotesLabelTextAlignmentRight);
        }
    else {
            cell = NULL;
    }
    return SIMPLE_NOTES_TABLE_VIEW_CELL(cell);
}

static gulong simple_notes_folder_window_real_get_rows_number (SimpleNotesTableWindow *object) {
    SimpleNotesFolderWindow *window = SIMPLE_NOTES_FOLDER_WINDOW(object);
    simple_notes_free_objects_array((gpointer *)window->_folders, window->_count);
    guint count = 0;
    window->_folders = simple_notes_folders_model_copy_folders(
            simple_notes_mediator_get_folders_model(simple_notes_table_window_get_model(SIMPLE_NOTES_TABLE_WINDOW(window))),
            &count
    );
    window->_count = count;
    return count;
}

static gulong simple_notes_folder_window_real_get_columns_number (SimpleNotesTableWindow *object) {
    return 4;
}
