//
// entity_presenter.h
//


#ifndef sn_entity_presenter____FILEEXTENSION___
#define sn_entity_presenter____FILEEXTENSION___

#include "simple_notes.h"
#include "store.h"
#include "SNIModel.h"


#define SN_TYPE_ENTITY_PRESENTER sn_entity_presenter_get_type()
G_DECLARE_DERIVABLE_TYPE(SNEntityPresenter,
			 sn_entity_presenter,
			 SN,
			 ENTITY_PRESENTER,
			 GObject)


  struct _SNEntityPresenterClass {
    GObjectClass _parent;
  };


SNIModel *
sn_entity_presenter_get_model(SNEntityPresenter *self);

void
sn_entity_presenter_assign_model(SNEntityPresenter *self, SNIModel *model);

SNStore *
sn_entity_presenter_get_store(SNEntityPresenter *self);

void
sn_entity_presenter_fetch(SNEntityPresenter *self,
			  SNEntityIterator *itr,
			  SNObject *(*create_item)(void));


#endif
