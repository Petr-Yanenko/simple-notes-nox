//
//  note.h
//  simple_notes
//
//  Created by Petr Yanenko on 11/23/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//


#ifndef sn_note____FILEEXTENSION___
#define sn_note____FILEEXTENSION___

#include "object.h"


#define SN_TYPE_NOTE sn_note_get_type()
G_DECLARE_FINAL_TYPE(SNNote, sn_note, SN, NOTE, SNObject)


SNNote *
sn_note_new(void);


GString *
sn_note_create_description(SNNote *object);

guint64
sn_note_get_id(SNNote *object);

void
sn_note_assign_id(SNNote *object, guint64 id);

gboolean
sn_note_get_selected(SNNote *object);

void
sn_note_assign_selected(SNNote *object, gboolean selected);

GString *
sn_note_get_copy_content(SNNote *object);

void
sn_note_copy_content(SNNote *object, gchar *content);

guint64
sn_note_get_folder_id(SNNote *object);

void
sn_note_assign_folder_id(SNNote *object, guint64 folderID);

GDateTime *
sn_note_get_copy_last_edited(SNNote *object);

void
sn_note_copy_last_edited(SNNote *object, GDateTime *time);


#endif
