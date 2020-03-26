//
//  data_base.h
//  simple_notes
//
//  Created by Petr Yanenko on 10/26/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef sn_data_base____FILEEXTENSION___
#define sn_data_base____FILEEXTENSION___

#include "simple_notes.h"
#include "statement.h"


#define SN_TYPE_DATA_BASE  sn_data_base_get_type()
G_DECLARE_FINAL_TYPE(SNDataBase, sn_data_base, SN, DATA_BASE, GObject)

  
SNDataBase *
sn_data_base_new(void);


gboolean
sn_data_base_add(SNDataBase *self, gchar *const key, gchar *const stmt);

SNStatement *
sn_data_base_bind(SNDataBase *self, gchar *const key, glong paramCount, ...);

gboolean
sn_data_base_execute(SNDataBase *self, gchar *const key, glong paramCount, ...);

#endif
