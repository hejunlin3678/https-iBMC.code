

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include <linux/pci_regs.h>
#include "pme_app/pme_app.h"
#include "pcie_card.h"
#include "pcie_card_parse_bdf_thread.h"
#include "card_manage_module.h"
#include "pcie_card_comm.h"
#include "pme_app/common/debug_log_macro.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"
#include "pcie_card_update_class_info.h"


gint32 pcie_card_update_class_info(PCIE_CARD_GROUP_CLASS_INFO_S *cards_info, guint32 pcie_index,
    INFO_NEED_GET_PCIE_INFO *info, PCIE_CARD_CLASS_INFO_S *card_info_s)
{
    const guint8 retry_times = 20;
    guint32 response = 0;
    guint8 i;
    gint32 ret;
    if (TRUE == match_v5_partition_product_peci_enable()) {
        
        for (i = 0; i < retry_times; i++) { 
            ret = pcie_get_by_peci_from_cpu(info, card_info_s);
            if (ret == RET_OK) {
                break;
            }
            vos_task_delay(READ_CLASS_INFO_DELAY_TIME);
        }

        if (i == retry_times) { 
            goto ERR;
        }
    } else {
        
        for (i = 0; i < retry_times; i++) { // 若失败，可尝试20次获取
            ret = pcie_get_info_from_me(info, (guint8 *)&response);
            if (ret == RET_OK) {
                break;
            }
            vos_task_delay(READ_CLASS_INFO_DELAY_TIME);
        }

        if (i == retry_times) { 
            goto ERR;
        }

        card_info_s->class_code = (guint8)((response & 0xFF000000) >> 24);     
        card_info_s->sub_class_code = (guint8)((response & 0x00FF0000) >> 16); 
        card_info_s->prog = (guint8)((response & 0x0000FF00) >> 8);            
        card_info_s->revision = (guint8)(response & 0x000000FF);               
    }
    return RET_OK;
ERR:
    debug_log(DLOG_ERROR,
        "get pcie class info from me failed with solt:0x%02x,cpu_num:0x%02x, "
        "bus:0x%02x,dev:0x%02x,func:0x%02x.",
        pcie_index, info->cpu_num, info->bus, info->dev, info->func);
    cards_info->class_info[pcie_index].class_code = 0xFE;
    cards_info->class_info[pcie_index].sub_class_code = 0xFE;
    cards_info->class_info[pcie_index].revision = 0xFE;
    cards_info->class_info[pcie_index].prog = 0xFE;
    return RET_ERR;
}
