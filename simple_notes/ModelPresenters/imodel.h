//
// imodel.h
//


#ifndef sn_imodel____FILEEXTENSION___
#define sn_imodel____FILEEXTENSION___

#include "simple_notes.h"


#define SN_TYPE_IMODEL sn_imodel_get_type()
G_DECLARE_INTERFACE(SNIModel, sn_imodel, SN, IMODEL, GObject)


  struct _SNIModel {
    GTypeInterface _parent;

    void (*error)(SNIModel *self, SNError error);
    void (*new_data)(SNIModel *self, void *data);
  };


void
sn_imodel_error(SNIModel *self, SNError error);

void
sn_imodel_new_data(SNIModel *self, void *data);


#endif
