//
// ientity_model.h
//


#ifndef sn_ientity_model____FILEEXTENSION___
#define sn_ientity_model____FILEEXTENSION___

#include "imodel.h"


#define SN_TYPE_IENTITY_MODEL sn_ientity_model_get_type()
G_DECLARE_INTERFACE(SNIEntityModel,
		    sn_ientity_model,
		    SN,
		    IENTITY_MODEL,
		    SNIModel)


  struct _SNIEntityModel {
    SNIModel _parent;

    void (*changed)(SNIEntityModel *self, guint64 id);
  };


void
sn_ientity_model_changed(SNIEntityModel *self, guint64 id);


#endif
