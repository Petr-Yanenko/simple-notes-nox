//
//  light_note.h
//  simple_notes
//
//  Created by Petr Yanenko on 11/23/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//


#ifndef sn_light_note____FILEEXTENSION___
#define sn_light_note____FILEEXTENSION___

#include "simple_notes.h"


#define SN_TYPE_LIGHT_NOTE sn_light_note_get_type()
G_DECLARE_INTERFACE(SNLightNote, sn_light_note, SN, LIGHT_NOTE, GObject)


struct _SNLightNoteInterface {
  GTypeInterface _parent;

  GString * (*create_description)(SNLightNote *object);
  guint64 (*get_id)(SNLightNote *object);
  gboolean (*get_selected)(SNLightNote *object);
  GString * (*get_copy_content)(SNLightNote *object);
  guint64 (*get_folder_id)(SNLightNote *object);
  GDateTime * (*get_copy_last_edited)(SNLightNote *object);
};


GString *
sn_light_note_create_description(SNLightNote *object);

guint64
sn_light_note_get_id(SNLightNote *object);

gboolean
sn_light_note_get_selected(SNLightNote *object);

GString *
sn_light_note_get_copy_content(SNLightNote *object);

guint64
sn_light_note_get_folder_id(SNLightNote *object);

GDateTime *
sn_light_note_get_copy_last_edited(SNLightNote *object);


#endif
