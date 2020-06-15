//
//  light_note.c
//  simple_notes
//
//  Created by Petr Yanenko on 11/23/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//


#include "light_note.h"


G_DEFINE_INTERFACE(SNLightNote, sn_light_note, G_TYPE_OBJECT)


static void
sn_light_note_default_init(SNLightNoteInterface *iface)
{
}

GString *
sn_light_note_create_description(SNLightNote *object)
{
  SNLightNoteInterface *iface;
  SN_GET_IFACE_OR_RETURN_VAL(object,
			     &iface,
			     create_description,
			     SNLightNote,
			     SN,
			     LIGHT_NOTE,
			     NULL);

  return iface->create_description(object);
}

guint64
sn_light_note_get_id(SNLightNote *object)
{
  SNLightNoteInterface *iface;
  SN_GET_IFACE_OR_RETURN_VAL(object,
			     &iface,
			     get_id,
			     SNLightNote,
			     SN,
			     LIGHT_NOTE,
			     0);

  return iface->get_id(object);
}

gboolean
sn_light_note_get_selected(SNLightNote *object)
{
  SNLightNoteInterface *iface;
  SN_GET_IFACE_OR_RETURN_VAL(object,
			     &iface,
			     get_selected,
			     SNLightNote,
			     SN,
			     LIGHT_NOTE,
			     FALSE);

  return iface->get_selected(object);
}

GString *
sn_light_note_get_copy_content(SNLightNote *object)
{
  SNLightNoteInterface *iface;
  SN_GET_IFACE_OR_RETURN_VAL(object,
			     &iface,
			     get_copy_content,
			     SNLightNote,
			     SN,
			     LIGHT_NOTE,
			     NULL);

  return iface->get_copy_content(object);
}

guint64
sn_light_note_get_folder_id(SNLightNote *object)
{
  SNLightNoteInterface *iface;
  SN_GET_IFACE_OR_RETURN_VAL(object,
			     &iface,
			     get_folder_id,
			     SNLightNote,
			     SN,
			     LIGHT_NOTE,
			     0);

  return iface->get_folder_id(object);
}

GDateTime *
sn_light_note_get_copy_last_edited(SNLightNote *object)
{
  SNLightNoteInterface *iface;
  SN_GET_IFACE_OR_RETURN_VAL(object,
			     &iface,
			     get_copy_last_edited,
			     SNLightNote,
			     SN,
			     LIGHT_NOTE,
			     NULL);

  return iface->get_copy_last_edited(object);
}
