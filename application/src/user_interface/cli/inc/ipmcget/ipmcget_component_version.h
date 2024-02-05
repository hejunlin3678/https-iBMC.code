
#ifndef IPMCGET__COMPONENT_VERSION_H
#define IPMCGET__COMPONENT_VERSION_H

#include <math.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/smlib/sml_base.h"
#include "pme_app/smlib/sml.h"
#include <pme_app/uip/uip_network_config.h>
#include "uip.h"
#include "ipmctypedef.h"
#include "ipmcgetcommand.h"
#include "ipmcgethelp.h"
#include "ipmc_public.h"
#include "ipmc_shelf_common.h"
#include <pme_app/smm/smm_public.h>

void print_passthrough_card_version(void);
void print_bmc_version(void);
void print_bios_version(void);
void print_ipmb_address(void);
void print_fabric_version(void);
void print_m2_transform_card_version(void);
void print_nic_boardid_pcb_version(void);
void print_raid_boardid_pcb_version(void);
void print_hddbackplane_boardid_pcb_version(void);
void print_memboard_boardid_pcb_version(void);
gint32 print_resource_components_list_info(guchar domain_id);
void print_resource_compositions_list_info(OBJ_HANDLE handle);
void print_ioboard_boardid_pcb_version(void);
void print_ncboard_boardid_pcb_version(void);
void print_lcd_cpld_version(void);
void print_ps_version(void);
void print_retimer_version(void);
void print_mezz_boardid_pcb_version(void);
void print_card_boardid_pcb_version(void);
void print_liquid_detect_card_info(void);
void print_mother_boardid_pcb_version(void);
void print_fanboard_boardid_pcb_version(void);
void print_product_version(void);
void print_rimm_version(void);
void print_mm_backplaneBoard_version(void);
void print_phy_fw_version(void);
void print_security_module_info(void);
void print_pcie_transform_card_version(void);
void print_pcie_card_version(void);
void print_gpuboard_version(void);
void print_asset_locator_version(void);
void print_horizontal_connection_board_version(void);
void print_component_uid(OBJ_HANDLE obj_handle, const gchar* component_name, guint32 slot_id,
    guint32 left_alignment);
void get_M3_version(void);
void print_retimer_location_version(OBJ_HANDLE obj_handle);
void print_converge_board_version(void);

#endif 