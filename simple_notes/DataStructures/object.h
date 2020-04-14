//
//  object.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/15/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef sn_object____FILEEXTENSION___
#define sn_object____FILEEXTENSION___

#include "simple_notes.h"


#define SN_TYPE_OBJECT sn_object_get_type()
G_DECLARE_DERIVABLE_TYPE(SNObject,
			 sn_object,
			 SN,
			 OBJECT,
			 GObject);


struct _SNObjectClass {
  GObjectClass _parent;

  GString * (*create_description)(SNObject *object);
};


SNObject *
sn_object_new(void);


GString *
sn_object_create_description(SNObject *object);

guint64
sn_object_get_id(SNObject *object);

void
sn_object_assign_id(SNObject *object, guint64 id);

gboolean
sn_object_get_selected(SNObject *object);

void
sn_object_assign_selected(SimpleNotesObject *object, gboolean selected);


#endif
