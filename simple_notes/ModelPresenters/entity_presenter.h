//
// entity_presenter.h
//


#ifndef sn_entity_presenter____FILEEXTENSION___
#define sn_entity_presenter____FILEEXTENSION___

#include "simple_notes.h"
#include "store.h"


#define SN_TYPE_ENTITY_PRESENTER sn_entity_presenter_get_type()
G_DECLARE_DERIVABLE_TYPE(SNEntityPresenter,
			 sn_entity_presenter,
			 SN,
			 ENTITY_PRESENTER,
			 GObject)


  struct _SNEntityPresenterClass {
    GObjectClass _parent;
  };


#endif
