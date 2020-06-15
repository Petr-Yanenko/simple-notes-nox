//
//  table_window.c
//  simple_notes
//
//  Created by Petr Yanenko on 3/10/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "table_window.h"

typedef struct _SimpleNotesTableWindowPrivate {
    SimpleNotesTableView *_table;
    SimpleNotesBaseController *_controller;
    SNBaseModel *_model;

    gulong _connectSignalModel;
} SimpleNotesTableWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesTableWindow, simple_notes_table_window, SIMPLE_NOTES_TYPE_WINDOW)

static gboolean simple_notes_table_window_real_handle_event (SimpleNotesResponder *object, SimpleNotesEvent *event);
static void simple_notes_table_window_real_new_data (
        GObject    *object,
        GParamSpec *pspec,
        gpointer    user_data
);


static SimpleNotesTableView *simple_notes_table_window_create_table (SimpleNotesTableWindow *object);
static SimpleNotesTableViewCell *simple_notes_table_window_create_cell_for_row_column (SimpleNotesTableWindow *object, gulong row, gulong column);
static SimpleNotesTableViewCell *simple_notes_table_window_create_header_for_column (SimpleNotesTableWindow *object, gulong column);
static SimpleNotesBaseController *simple_notes_table_window_create_controller (SimpleNotesTableWindow *object, SNBaseModel *model);
static gulong simple_notes_table_window_get_rows_number (SimpleNotesTableWindow *object);
static gulong simple_notes_table_window_get_columns_number (SimpleNotesTableWindow *object);
static void simple_notes_table_window_connect_signal (SimpleNotesTableWindow *object, SNBaseModel *model);

static void simple_notes_table_window_dispose (GObject *object) {
    SimpleNotesTableWindow *window = SIMPLE_NOTES_TABLE_WINDOW(object);
    SimpleNotesTableWindowPrivate *private = simple_notes_table_window_get_instance_private(window);
    g_clear_object(&(private->_table));
    g_clear_object(&(private->_controller));
    g_clear_object(&private->_model);

    G_OBJECT_CLASS(simple_notes_table_window_parent_class)->dispose(object);
}

static void simple_notes_table_window_constructed (GObject *object) {
    SimpleNotesTableWindow *window = SIMPLE_NOTES_TABLE_WINDOW(object);
    SimpleNotesTableWindowPrivate *private = simple_notes_table_window_get_instance_private(window);
    SimpleNotesTableView *table = simple_notes_table_window_create_table(window);
    simple_notes_view_add_subview(SIMPLE_NOTES_VIEW(object), SIMPLE_NOTES_VIEW(table));
    simple_notes_table_view_assign_separator_style(table, SimpleNotesTableViewCellSeparatorStyleHorizontal | SimpleNotesTableViewCellSeparatorStyleVertical);
    private->_table = table;
}

void simple_notes_table_window_class_init (SimpleNotesTableWindowClass *klass) {
    SIMPLE_NOTES_RESPONDER_CLASS(klass)->handle_event = simple_notes_table_window_real_handle_event;

    G_OBJECT_CLASS(klass)->dispose = simple_notes_table_window_dispose;
    G_OBJECT_CLASS(klass)->constructed = simple_notes_table_window_constructed;

    klass->create_table = NULL;
    klass->create_controller = NULL;

    g_type_add_instance_private(SIMPLE_NOTES_TYPE_TABLE_WINDOW, sizeof(SimpleNotesTableWindowPrivate));
}

void simple_notes_table_window_init (SimpleNotesTableWindow *object) {
    SimpleNotesTableWindowPrivate *private = simple_notes_table_window_get_instance_private(object);

    private->_model = NULL;
    private->_controller = NULL;
    private->_table = NULL;
    private->_connectSignalModel = 0;
}

SimpleNotesLabelCell *simple_notes_table_window_create_cell (gchar *text, SimpleNotesLabelTextAlignment alignment) {
    SimpleNotesLabelCell *cell = simple_notes_label_cell_new();
    SimpleNotesLabel *cellLabel = simple_notes_label_cell_get_label(cell);
    simple_notes_label_assign_line_break_mode(cellLabel, SimpleNotesLabelLineBreakModeWord);
    simple_notes_label_assign_text_alignment(cellLabel, alignment);
    simple_notes_label_copy_text(cellLabel, text);
    return cell;
}

SimpleNotesTableWindow *simple_notes_table_window_new (
        GType type,
        SimpleNotesResponder *next,
        SNBaseModel *model
) {
    SimpleNotesTableWindow *object = SIMPLE_NOTES_TABLE_WINDOW(simple_notes_responder_new(type, next));
    SimpleNotesTableWindowPrivate *private = simple_notes_table_window_get_instance_private(object);
    private->_model = g_object_ref(model);
    private->_controller = simple_notes_table_window_create_controller(object, model);
    simple_notes_table_window_connect_signal(object, model);
    return object;
}

SimpleNotesTableView *simple_notes_table_window_get_table (SimpleNotesTableWindow *object) {
    SimpleNotesTableWindowPrivate *private = simple_notes_table_window_get_instance_private(object);
    return private->_table;
}

static void simple_notes_table_window_connect_signal (SimpleNotesTableWindow *object, SNBaseModel *model) {
  SNBaseModel *connectSignalModel = model;
  SimpleNotesTableWindowPrivate *private = simple_notes_table_window_get_instance_private(object);
  if (connectSignalModel && private->_connectSignalModel != (gulong)connectSignalModel)
    {
      g_signal_connect(connectSignalModel,
		       "notify::new-data",
		       G_CALLBACK(simple_notes_table_window_real_new_data),
		       object);
      private->_connectSignalModel = (gulong)connectSignalModel;
    }
}

SNBaseModel *simple_notes_table_window_get_model(SimpleNotesTableWindow *object)
{
  SimpleNotesTableWindowPrivate *private;
  private = simple_notes_table_window_get_instance_private(object);

  return private->_model;
}

static gboolean simple_notes_table_window_real_handle_event (SimpleNotesResponder *object, SimpleNotesEvent *event) {
    simple_notes_window_assign_active(SIMPLE_NOTES_WINDOW(object), TRUE);

    SimpleNotesTableWindow *window = SIMPLE_NOTES_TABLE_WINDOW(object);
    SimpleNotesTableWindowPrivate *private = simple_notes_table_window_get_instance_private(window);
    simple_notes_table_window_connect_signal(window, private->_model);
    GError *error = NULL;
    SimpleNotesBaseControllerEventResult result = simple_notes_base_controller_handle_event(
            SIMPLE_NOTES_BASE_CONTROLLER(private->_controller),
            event,
            &error
    );

    simple_notes_window_assign_active(SIMPLE_NOTES_WINDOW(object), FALSE);

    if (result == SimpleNotesBaseControllerEventResultHandled) {
        return TRUE;
    } else if (result == SimpleNotesBaseControllerEventResultError) {
        simple_notes_responder_handle_error(object, error);
        g_clear_error(&error);
        return TRUE;
    }
    return SIMPLE_NOTES_RESPONDER_CLASS(simple_notes_table_window_parent_class)->handle_event(object, event);
}

static void simple_notes_table_window_real_new_data (
        GObject    *object,
        GParamSpec *pspec,
        gpointer    user_data
) {
    SimpleNotesTableWindow *window = SIMPLE_NOTES_TABLE_WINDOW(user_data);

    GPtrArray *cells = g_ptr_array_new();
    gulong columnNumber = simple_notes_table_window_get_columns_number(window);
    gulong headerNumber = 1;
    gulong rowsNumber = simple_notes_table_window_get_rows_number(window) + headerNumber;
    gulong cellNumber = rowsNumber * columnNumber;
    for (gulong i = 0; i < cellNumber; i++) {
        gulong row = i / columnNumber - 1;
        gulong column = i % columnNumber;
        SimpleNotesTableViewCell *cell = NULL;
        if (i < columnNumber) {
            cell = simple_notes_table_window_create_header_for_column(window, column);
        } else {
            cell = simple_notes_table_window_create_cell_for_row_column(window, row, column);
        }
        g_ptr_array_add(cells, cell);
    }
    SimpleNotesTableViewCell **cellArray = (SimpleNotesTableViewCell **)g_ptr_array_free(cells, FALSE);
    simple_notes_table_view_copy_cells(
            simple_notes_table_window_get_table(SIMPLE_NOTES_TABLE_WINDOW(window)),
            cellArray,
            rowsNumber,
            columnNumber
    );
    sn_free_objects_array((gpointer *)cellArray, cellNumber);
    simple_notes_view_layout_if_needed(SIMPLE_NOTES_VIEW(window));
    simple_notes_view_size_to_fit(SIMPLE_NOTES_VIEW(window));
    simple_notes_view_display(SIMPLE_NOTES_VIEW(window));
}

static SimpleNotesTableView *simple_notes_table_window_create_table (SimpleNotesTableWindow *object) {
    SimpleNotesTableWindowClass *klass;
    SN_GET_CLASS_OR_RETURN_VAL(object, &klass, create_table, SimpleNotesTableWindow, SIMPLE_NOTES, TABLE_WINDOW, NULL);
    return klass->create_table(object);
}

static SimpleNotesTableViewCell *simple_notes_table_window_create_cell_for_row_column (SimpleNotesTableWindow *object, gulong row, gulong column) {
    SimpleNotesTableWindowClass *klass;
    SN_GET_CLASS_OR_RETURN_VAL(object, &klass, create_cell_for_row_column, SimpleNotesTableWindow, SIMPLE_NOTES, TABLE_WINDOW, NULL);
    return klass->create_cell_for_row_column(object, row, column);
}

static SimpleNotesTableViewCell *simple_notes_table_window_create_header_for_column (SimpleNotesTableWindow *object, gulong column) {
    SimpleNotesTableWindowClass *klass;
    SN_GET_CLASS_OR_RETURN_VAL(object, &klass, create_header_for_column, SimpleNotesTableWindow, SIMPLE_NOTES, TABLE_WINDOW, NULL);
    return klass->create_header_for_column(object, column);
}

static SimpleNotesBaseController *simple_notes_table_window_create_controller (SimpleNotesTableWindow *object, SNBaseModel *model) {
    SimpleNotesTableWindowClass *klass;
    SN_GET_CLASS_OR_RETURN_VAL(object, &klass, create_controller, SimpleNotesTableWindow, SIMPLE_NOTES, TABLE_WINDOW, NULL);
    return klass->create_controller(object, model);
}

static gulong simple_notes_table_window_get_rows_number (SimpleNotesTableWindow *object) {
    SimpleNotesTableWindowClass *klass;
    SN_GET_CLASS_OR_RETURN_VAL(object, &klass, get_rows_number, SimpleNotesTableWindow, SIMPLE_NOTES, TABLE_WINDOW, 0);
    return klass->get_rows_number(object);
}

static gulong simple_notes_table_window_get_columns_number (SimpleNotesTableWindow *object) {
    SimpleNotesTableWindowClass *klass;
    SN_GET_CLASS_OR_RETURN_VAL(object, &klass, get_columns_number, SimpleNotesTableWindow, SIMPLE_NOTES, TABLE_WINDOW, 0);
    return klass->get_columns_number(object);
}
