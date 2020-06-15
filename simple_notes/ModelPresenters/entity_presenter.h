//
// entity_presenter.h
//


#ifndef sn_entity_presenter____FILEEXTENSION___
#define sn_entity_presenter____FILEEXTENSION___

#include "simple_notes.h"
#include "store.h"
#include "ientity_model.h"
#include "object.h"


#define SN_ENTITY_PRESENTER_SEND_CHANGED(self, changed, id)	\
  {								\
  sn_entity_presenter_send_changed(SN_ENTITY_PRESENTER(self),	\
				   changed,			\
				   id,				\
				   kError);			\
  }


#define SN_TYPE_ENTITY_PRESENTER sn_entity_presenter_get_type()
G_DECLARE_DERIVABLE_TYPE(SNEntityPresenter,
			 sn_entity_presenter,
			 SN,
			 ENTITY_PRESENTER,
			 GObject)


  struct _SNEntityPresenterClass {
    GObjectClass _parent;

    //private
    SNEntityIterator * (*create_iterator)(SNEntityPresenter *);
    SNObject * (*create_item)(SNEntityPresenter *, SNEntityIterator *);
  };


SNIEntityModel *
sn_entity_presenter_get_model(SNEntityPresenter *self);

void
sn_entity_presenter_assign_model(SNEntityPresenter *self,
				 SNIEntityModel *model);

SNStore *
sn_entity_presenter_get_store(SNEntityPresenter *self);

void
sn_entity_presenter_fetch(SNEntityPresenter *self);

void
sn_entity_presenter_send_changed(SNEntityPresenter *self,
				 gboolean changed,
				 guint64 id,
				 SNError error);


#endif
