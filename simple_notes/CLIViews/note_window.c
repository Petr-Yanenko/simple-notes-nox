//
//  note_window.c
//  simple_notes
//
//  Created by Petr Yanenko on 3/10/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "note_window.h"
#include "note_controller.h"
#include "view_utility.h"
#include "light_note.h"


static const glong kContentColumn = 3;
static const glong kFolderIdColumn = 2;
static const gulong kLastEditedColumn = 4;

struct _SimpleNotesNoteWindow {
    SimpleNotesTableWindow parent;

    SNLightNote **_notes;
    gulong _count;
};

G_DEFINE_TYPE(SimpleNotesNoteWindow, simple_notes_note_window, SIMPLE_NOTES_TYPE_TABLE_WINDOW)

static gulong simple_notes_note_window_real_height_for_row (SimpleNotesTableView *object, gulong row);
static gulong simple_notes_note_window_real_width_for_column (SimpleNotesTableView *object, gulong column);
static SimpleNotesTableView *simple_notes_note_window_real_create_table (SimpleNotesTableWindow *object);
static SimpleNotesBaseController *simple_notes_note_window_real_create_controller (SimpleNotesTableWindow *object, SNBaseModel *model);
static gulong simple_notes_note_window_real_get_rows_number (SimpleNotesTableWindow *object);
static gulong simple_notes_note_window_real_get_columns_number (SimpleNotesTableWindow *object);
static SimpleNotesTableViewCell *simple_notes_note_window_real_create_header_for_column (SimpleNotesTableWindow *object, gulong column);
static SimpleNotesTableViewCell *simple_notes_note_window_real_create_cell_for_row_column (SimpleNotesTableWindow *object, gulong row, gulong column);

static void simple_notes_note_window_dispose (GObject *object) {
    SimpleNotesNoteWindow *window = SIMPLE_NOTES_NOTE_WINDOW(object);
    sn_free_objects_array((gpointer *)window->_notes, window->_count);

    G_OBJECT_CLASS(simple_notes_note_window_parent_class)->dispose(object);
}

void simple_notes_note_window_class_init (SimpleNotesNoteWindowClass *klass) {
    SimpleNotesTableWindowClass *parentClass = SIMPLE_NOTES_TABLE_WINDOW_CLASS(klass);
    parentClass->create_table = simple_notes_note_window_real_create_table;
    parentClass->create_controller = simple_notes_note_window_real_create_controller;
    parentClass->get_rows_number = simple_notes_note_window_real_get_rows_number;
    parentClass->get_columns_number = simple_notes_note_window_real_get_columns_number;
    parentClass->create_header_for_column = simple_notes_note_window_real_create_header_for_column;
    parentClass->create_cell_for_row_column = simple_notes_note_window_real_create_cell_for_row_column;

    G_OBJECT_CLASS(klass)->dispose = simple_notes_note_window_dispose;
}

void simple_notes_note_window_init (SimpleNotesNoteWindow *object) {
    object->_notes = NULL;
    object->_count = 0;
}

SimpleNotesNoteWindow *simple_notes_note_window_new (SimpleNotesResponder *next, SNNotesModel *model) {
    SimpleNotesNoteWindow *window = SIMPLE_NOTES_NOTE_WINDOW(simple_notes_table_window_new(
            SIMPLE_NOTES_TYPE_NOTE_WINDOW,
            next,
            SN_BASE_MODEL(model)
    ));
    return window;
}

static SimpleNotesTableView *simple_notes_note_window_real_create_table (SimpleNotesTableWindow *object) {
    return simple_notes_table_view_new(
            NULL,
            0,
            0,
            simple_notes_note_window_real_width_for_column,
            simple_notes_note_window_real_height_for_row
    );
}

static SimpleNotesBaseController *simple_notes_note_window_real_create_controller (SimpleNotesTableWindow *object, SNBaseModel *model) {
  SNNotesModel *notes = SN_NOTES_MODEL(model);
  SimpleNotesNoteController *controller = simple_notes_note_controller_new(notes);
  return SIMPLE_NOTES_BASE_CONTROLLER(controller);
}

static gulong simple_notes_note_window_real_height_for_row (SimpleNotesTableView *object, gulong row) {
    gulong width = simple_notes_note_window_real_width_for_column(object, kContentColumn);
    gulong height = simple_notes_height_for_row(object, row, kContentColumn, width);
    return height;
}

static gulong simple_notes_note_window_real_width_for_column (SimpleNotesTableView *object, gulong column) {
    if (column == kIdColumn || column == kFolderIdColumn || column == kLastEditedColumn) {
        return 25;
    } else if (column == kContentColumn) {
        return 75;
    }
    return 10;
}

static SimpleNotesTableViewCell *simple_notes_note_window_real_create_header_for_column (SimpleNotesTableWindow *object, gulong column) {
  if (column == kIdColumn) {
            return SIMPLE_NOTES_TABLE_VIEW_CELL(simple_notes_table_window_create_cell("id\0", SimpleNotesLabelTextAlignmentRight));
  }
  else if (column == kSelectedColumn) {
            return SIMPLE_NOTES_TABLE_VIEW_CELL(simple_notes_table_window_create_cell("selected\0", SimpleNotesLabelTextAlignmentRight));
  }
  else if (column == kContentColumn) {
            return SIMPLE_NOTES_TABLE_VIEW_CELL(simple_notes_table_window_create_cell("content\0", SimpleNotesLabelTextAlignmentLeft));
  }
  else if (column == kFolderIdColumn) {
            return SIMPLE_NOTES_TABLE_VIEW_CELL(simple_notes_table_window_create_cell("folder id\0", SimpleNotesLabelTextAlignmentRight));
  }
  else if (column == kLastEditedColumn) {
            return SIMPLE_NOTES_TABLE_VIEW_CELL(simple_notes_table_window_create_cell("last edited\0", SimpleNotesLabelTextAlignmentRight));
  }
  else {
            return NULL;
    }
}

static SimpleNotesTableViewCell *simple_notes_note_window_real_create_cell_for_row_column (SimpleNotesTableWindow *object, gulong row, gulong column) {
    SimpleNotesNoteWindow *window = SIMPLE_NOTES_NOTE_WINDOW(object);
    SimpleNotesLabelCell *cell = NULL;
    if (column == kIdColumn) {
            gchar buff[kLongLongSymbols];
            guint64 identifier = sn_light_note_get_id(window->_notes[row]);
            sn_print_guint64_value(buff, identifier);
            cell = simple_notes_table_window_create_cell(buff, SimpleNotesLabelTextAlignmentRight);
        }
    else if (column == kSelectedColumn) {
            gchar buff[kSelectedSymbols];
            sn_print_boolean_value(buff, sn_light_note_get_selected(window->_notes[row]));
            cell = simple_notes_table_window_create_cell(buff, SimpleNotesLabelTextAlignmentRight);
        }
    else if (column == kContentColumn) {
            GString *content = sn_light_note_get_copy_content(window->_notes[row]);
            gchar *text = content->str;
            cell = simple_notes_table_window_create_cell(text, SimpleNotesLabelTextAlignmentLeft);
            g_string_free(content, TRUE);
        }
    else if (column == kFolderIdColumn) {
            gchar buff[kLongLongSymbols];
            sn_print_guint64_value(buff, sn_light_note_get_folder_id(window->_notes[row]));
            cell = simple_notes_table_window_create_cell(buff, SimpleNotesLabelTextAlignmentRight);
        }
    else if (column == kLastEditedColumn) {
            GDateTime *lastEdited = sn_light_note_get_copy_last_edited(window->_notes[row]);
            gchar *date = g_date_time_format(lastEdited, "%c");
            cell = simple_notes_table_window_create_cell(date, SimpleNotesLabelTextAlignmentRight);
            g_free(date);
            g_date_time_unref(lastEdited);
        }
    else {
            cell = NULL;
    }
    return SIMPLE_NOTES_TABLE_VIEW_CELL(cell);
}

static gulong simple_notes_note_window_real_get_rows_number (SimpleNotesTableWindow *object) {
    SimpleNotesNoteWindow *window = SIMPLE_NOTES_NOTE_WINDOW(object);
    sn_free_objects_array((gpointer *)window->_notes, window->_count);
    SimpleNotesTableWindow *table = SIMPLE_NOTES_TABLE_WINDOW(window);
    SNBaseModel *model = simple_notes_table_window_get_model(table);
    SNNotesModel *notes = SN_NOTES_MODEL(model);
    guint count = 0;
    window->_notes = sn_notes_model_copy_notes(
            notes,
            &count
    );
    window->_count = count;
    return count;
}

static gulong simple_notes_note_window_real_get_columns_number (SimpleNotesTableWindow *object) {
    return 5;
}
