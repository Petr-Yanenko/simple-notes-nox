//
//  label_cell.h
//  simple_notes
//
//  Created by Petr Yanenko on 2/3/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_label_cell____FILEEXTENSION___
#define simple_notes_label_cell____FILEEXTENSION___

#include "table_view_cell.h"
#include "label.h"

#define SIMPLE_NOTES_TYPE_LABEL_CELL simple_notes_label_cell_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesLabelCell, simple_notes_label_cell, SIMPLE_NOTES, LABEL_CELL, SimpleNotesTableViewCell)

SimpleNotesLabelCell *simple_notes_label_cell_new (void);

SimpleNotesLabel *simple_notes_label_cell_get_label (SimpleNotesLabelCell *object);

#endif
