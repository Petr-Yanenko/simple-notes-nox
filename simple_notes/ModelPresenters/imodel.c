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
sn_imodel_new_data(SNIModel *self, gpointer data)
{
  SNIModelInterface *iface = NULL;
  SN_GET_IFACE(self,
	       &iface,
	       new_data,
	       SNIModel,
	       SN,
	       IMODEL);

  iface->new_data(self, data);
}

void
sn_imodel_error(SNIModel *self, SNError error, gpointer data)
{
  SNIModelInterface *iface = NULL;
  SN_GET_IFACE(self,
	       &iface,
	       error,
	       SNIModel,
	       SN,
	       IMODEL);

  iface->error(self, error, data);
}
