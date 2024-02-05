

#ifndef __FRU_VIRTUAL_PSU_H__
#define __FRU_VIRTUAL_PSU_H__

gint32 get_fru_by_ps_id(guint8 ps_id, PS_FRU_S *fru_data);

gint32 get_frudata_from_psu(guint8 ps_id, PS_FRU_S *fru_data);

#endif 
