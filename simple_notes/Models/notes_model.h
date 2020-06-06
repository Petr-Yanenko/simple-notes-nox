//
//  notes_model.h
//  simple_notes
//
//  Created by Petr Yanenko on 11/30/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef sn_notes_model____FILEEXTENSION___
#define sn_notes_model____FILEEXTENSION___

#include "selected_list_model.h"
#include "light_note.h"


#define SN_TYPE_NOTES_MODEL sn_notes_model_get_type()
G_DECLARE_FINAL_TYPE(SNNotesModel,
		     sn_notes_model,
		     SN,
		     NOTES_MODEL,
		     SNSelectedListModel)


SNNotesModel *
sn_notes_model_new(void);


void
sn_notes_model_insert_note(SNNotesModel *object, gchar *content);

SNLightNote **
sn_notes_model_copy_notes(SNNotesModel *object, guint *out_count);

void
sn_notes_model_delete_note(SNNotesModel *object, guint64 identifier);

void
sn_notes_model_select_note(SNNotesModel *object, guint64 identifier);


#endif
