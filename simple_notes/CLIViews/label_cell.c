//
//  label_cell.c
//  simple_notes
//
//  Created by Petr Yanenko on 2/3/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "label_cell.h"

struct _SimpleNotesLabelCell {
  SimpleNotesTableViewCell parent;

  SimpleNotesLabel *_label;
};

G_DEFINE_TYPE(SimpleNotesLabelCell, simple_notes_label_cell, SIMPLE_NOTES_TYPE_TABLE_VIEW_CELL)

static void simple_notes_label_cell_real_layout_subviews (SimpleNotesView *object);
static SimpleNotesSize simple_notes_label_cell_real_size_that_fits (SimpleNotesView *object, SimpleNotesSize size);

void simple_notes_label_cell_dispose (GObject *object) {

  SimpleNotesLabelCell *cell = SIMPLE_NOTES_LABEL_CELL(object);
  simple_notes_view_remove_from_superview(SIMPLE_NOTES_VIEW(cell->_label));
  g_clear_object(&(cell->_label));

  G_OBJECT_CLASS(simple_notes_label_cell_parent_class)->dispose(object);
}

void simple_notes_label_cell_class_init (SimpleNotesLabelCellClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = simple_notes_label_cell_dispose;
    SimpleNotesViewClass *viewClass = SIMPLE_NOTES_VIEW_CLASS(klass);
    viewClass->layout_subviews = simple_notes_label_cell_real_layout_subviews;
    viewClass->size_that_fits = simple_notes_label_cell_real_size_that_fits;
}

void simple_notes_label_cell_init (SimpleNotesLabelCell *object) {
  SimpleNotesView *contentView = simple_notes_table_view_cell_get_content_view(SIMPLE_NOTES_TABLE_VIEW_CELL(object));
  object->_label = simple_notes_label_new("\0");
  simple_notes_view_add_subview(contentView, SIMPLE_NOTES_VIEW(object->_label));
}

SimpleNotesLabelCell *simple_notes_label_cell_new (void) {
  return g_object_new (SIMPLE_NOTES_TYPE_LABEL_CELL, NULL);
}

SimpleNotesLabel *simple_notes_label_cell_get_label (SimpleNotesLabelCell *object) {
  return object->_label;
}

static void simple_notes_label_cell_real_layout_subviews (SimpleNotesView *object) {
    SIMPLE_NOTES_VIEW_CLASS(simple_notes_label_cell_parent_class)->layout_subviews(object);
    SimpleNotesView *contentView = simple_notes_table_view_cell_get_content_view(SIMPLE_NOTES_TABLE_VIEW_CELL(object));
    SimpleNotesLabel *cellLabel = simple_notes_label_cell_get_label(SIMPLE_NOTES_LABEL_CELL(object));
    simple_notes_view_assign_frame(SIMPLE_NOTES_VIEW(cellLabel), simple_notes_view_get_bounds(contentView));
}

static SimpleNotesSize simple_notes_label_cell_real_size_that_fits (SimpleNotesView *object, SimpleNotesSize size) {
    SimpleNotesLabel *label = simple_notes_label_cell_get_label(SIMPLE_NOTES_LABEL_CELL(object));
    SimpleNotesSize sizeForLabel = simple_notes_make_size(size.width - 1, size.height);
    SimpleNotesSize labelSize = simple_notes_view_size_that_fits(SIMPLE_NOTES_VIEW(label), sizeForLabel);
    SimpleNotesSize cellSize = labelSize;
    cellSize.width += 1;
    cellSize.height += 1;
    return cellSize;
}
