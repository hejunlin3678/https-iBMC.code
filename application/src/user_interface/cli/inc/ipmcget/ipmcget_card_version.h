
#ifndef IPMCGET__CARD_VERSION_H
#define IPMCGET__CARD_VERSION_H

#include <math.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pme_app/uip/uip_network_config.h>
#include <pme_app/smm/smm_public.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/smlib/sml_base.h"
#include "pme_app/smlib/sml.h"
#include "uip.h"
#include "ipmctypedef.h"
#include "ipmcgetcommand.h"
#include "ipmcgethelp.h"
#include "ipmc_public.h"
#include "ipmc_shelf_common.h"

void print_pangea_card_mcu_ver(void);
void print_vrd_version(void);

#endif 