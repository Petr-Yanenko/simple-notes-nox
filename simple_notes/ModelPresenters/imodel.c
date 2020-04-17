//
// imodel.c
//


#include "imodel.h"


G_DEFINE_INTERFACE(SNIModel, sn_imodel, G_TYPE_OBJECT)


static void
sn_imodel_default_init(SNIModelInterface *self)
{
}

void
sn_imodel_new_data(SNIModel *self, void *data)
{
  SNIModelInterface *iface = NULL;
  SN_GET_IFACE(self,
	       &iface,
	       new_data,
	       SNIModel,
	       SN,
	       IModel);

  iface->new_data(self, data);
}

void
sn_imodel_error(SNIModel *self, SNError error, void *data)
{
  SNIModelInterface *iface = NULL;
  SN_GET_IFACE(self,
	       &iface,
	       error,
	       SNIModel,
	       SN,
	       IModel);

  iface->error(self, error, data);
}
