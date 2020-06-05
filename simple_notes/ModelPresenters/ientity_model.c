//
// ientity_model.c
//


#include "ientity_model.h"


G_DEFINE_INTERFACE(SNIEntityModel, sn_ientity_model, SN_TYPE_IMODEL)


static void
sn_ientity_model_default_init(SNIEntityModelInterface *iface)
{
}

void
sn_ientity_model_changed(SNIEntityModel *self, guint64 id)
{
  SNIEntityModelInterface *iface = NULL;
  SN_GET_IFACE(self,
	       &iface,
	       changed,
	       SNIEntityModel,
	       SN,
	       IENTITY_MODEL);

  iface->changed(self, id);
}

void
sn_ientity_model_selected(SNIEntityModel *self, guint64 id)
{
  SNIEntityModelInterface *iface = NULL;
  SN_GET_IFACE(self,
	       &iface,
	       selected,
	       SNIEntityModel,
	       SN,
	       IENTITY_MODEL);

  iface->selected(self, id);
}
