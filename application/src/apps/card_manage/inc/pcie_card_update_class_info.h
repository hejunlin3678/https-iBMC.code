
#ifndef __PCIE_CARD_UPDATE_CLASS_INFO_H__
#define __PCIE_CARD_UPDATE_CLASS_INFO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 
#define READ_CLASS_INFO_DELAY_TIME 100
gint32 pcie_card_update_class_info(PCIE_CARD_GROUP_CLASS_INFO_S *cards_info, guint32 pcie_index,
    INFO_NEED_GET_PCIE_INFO *info, PCIE_CARD_CLASS_INFO_S *card_info_s);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
