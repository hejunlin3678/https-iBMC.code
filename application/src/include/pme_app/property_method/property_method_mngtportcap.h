/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: PRODUCT ID AND BOARD ID描述文件
 * Author: z30023860
 * Create: 2022-12-2
 */
#ifndef PME_PROPERTY_METHOD_MGNTPORTCAP_H
#define PME_PROPERTY_METHOD_MGNTPORTCAP_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* MngtPortCap 类定义  */
#define ETH_CLASS_NAME_MGNTPORTCAP                  "MngtPortCap"
#define OBJ_NAME_MGNTPORTCAP                        "MngtPortCap"
#define PROPERTY_MNGTPORTCAP_DEDICATEDENABLE        "DedicatedEnable"
#define PROPERTY_MNGTPORTCAP_AGGREGATIONENABLE      "AggregationEnable"
#define PROPERTY_MNGTPORTCAP_LOMNCSIENABLE          "LomNcsiEnable"
#define PROPERTY_MNGTPORTCAP_PCIENCSIENABLE         "PcieNcsiEnable"
#define PROPERTY_MNGTPORTCAP_LOM2NCSIENABLE         "Lom2NcsiEnable"
#define PROPERTY_MNGTPORTCAP_OCPENABLE              "OCPNcsiEnable"
#define PROPERTY_MNGTPORTCAP_OCP2ENABLE             "OCP2NcsiEnable"
#define PROPERTY_MNGTPORTCAP_ETHNUMOCP              "EthNumOfOcp"
#define PROPERTY_MNGTPORTCAP_DEDICATED_VLAN_ENABLE  "DedicatedVlanEnable"
#define PROPERTY_MNGTPORTCAP_POWER_DOMAIN           "NcsiPowerDomain"
#define PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE        "PCIeMgntEnable"
#define METHOD_MGNTPORTCAP_GET_SW_CONNS             "GetSwitchConnections"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PME_PROPERTY_METHOD_MGNTPORTCAP_H */