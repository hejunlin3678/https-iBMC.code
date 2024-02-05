/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: BMC网络管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __NETWORK_DEFINE_H__
#define __NETWORK_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAC_ADDR_DEFAULT "00:00:00:00:00:00"
#define INNER_IP_ADDR "127.0.0.1"
#define IPV4_LOOPBACK_ADDR_MAPPED "::ffff:127.0.0.1"

#define NET_MODE_MANUAL 1
#define NET_MODE_AUTO 2

#define NCSI_NET_MODE_ENABLE 1
#define NCSI_NET_MODE_DISENABLE 0

#define NAT_PROTOCOL_IPV4 0
#define NAT_PROTOCOL_IPV6 1

#define NAT_PROTOCOL_HTTP 0
#define NAT_PROTOCOL_HTTPS 1

#define DEFAULT_PORTAL_PORT 30080            // 默认portal端口
#define SECOND_NGINX_KVM_PORT 30081          // 板卡固件上传端口
#define DEFAULT_BMA_NAT_PORT 30023           // 默认nat端口
#define DEFAULT_MM_NAT_PORT 30022            // 默认nat端口
#define DEFAULT_MM2_NAT_PORT 30122           // 默认nat2端口
#define DEFAULT_MM_MAPPING_PORT 20022        // 默认MM板跳转端口
#define DEFAULT_MM_MAPPING_IP "172.31.1.38"  // 默认MM板IP地址
#define DEFAULT_MM2_MAPPING_IP "172.31.2.39" // 默认MM2板IP地址
#define BMA_VETH_IPADDR "fe80::9e7d:a3ff:fe28:6ff9"
#define BMC_VETH_IPADDR "fe80::9e7d:a3ff:fe28:6ffa"
#define BMC_VETH_IPADDR_SUB "fe80::9e7d:a3ff:fe28:6ffa%veth"
#define BMC_VETH_PORT_SUB "30023"

enum BobChannelType {
    PCIE_VETH = 0,
    PCIE_CDEV,
    USB_LINUX,
    USB_WINDOWS,
    USB_VMWARE,
    INVALID_CHANNEL_TYPE // 无效的BOB 通道类型
};

#define NET_TYPE_NONE 0            // 设置VLAN参数，不指定特定网口
#define NET_TYPE_LOM 1             // 板载1网卡，对应eth0，部分网卡支持NCSI
#define NET_TYPE_DEDICATED 2       // 专用网卡,对应eth2，不支持NCSI
#define NET_TYPE_PCIE 3            // PCIE网卡,对应eth1，部分网卡支持NCSI
#define NET_TYPE_AGGREGATION 4     // 聚合网卡，对应eth3，不支持NCSI
#define NET_TYPE_LOM2 5            // 板载2网卡,V5专用，对应eth1 ，部分网卡支持NCSI
#define NET_TYPE_INNER_DEDICATED 6 // 非对外专用网卡,不支持NCSI
#define NET_TYPE_CABINET_VLAN 7       // 机柜Vlan网口，不支持NCSI
#define NET_TYPE_MEZZ_CATED    9   // mezz卡
#define NET_TYPE_OCP_CARD 10       // OCPCard, 对应eth1，与PCIe的NCSI通过开关切换
#define NET_TYPE_OCP2_CARD 11      // OCP2Card
#define NET_TYPE_MAX 12

#define INNER_GROUP_TYPE 1
#define OUTTER_GROUP_TYPE 2
#define VETH_GROUP_TYPE 3
#define FLOAT_GROUP_TYPE 6

#define ETH_MAX_COUNT 2 /* 网口个数 */
#define INNER_NETWORK 3
#define IPV4_IP_STR_SIZE 15
#define IPV4_MASK_STR_SIZE 15
#define IPV4_GATEWAY_STR_SIZE 15
#define IPV4_IP_SIZE 4
#define IPV4_MASK_SIZE 4
#define IPV4_GATEWAY_SIZE 4
#define IPV6_IP_SIZE 16
#define IPV6_GATEWAY_SIZE 16
#define IPV6_IP_STR_SIZE 45
#define IPV6_GATEWAY_STR_SIZE 45

/* IPMI网口切换 */
#define LAN_MODE_DEDICATE 1    /* 专用 */
#define LAN_MODE_ONBOARD 2     /* 板载网卡 */
#define LAN_MODE_ADAPTIVE 3    /* 自适应 */
#define LAN_MODE_PCIE 4        /* PCIE网卡 */
#define LAN_MODE_AGGREGATION 5 /* 汇聚网口 */
#define LAN_MODE_ONBOARD2 6    /* 板载网卡2 */
#define LAN_MODE_OCPCARD1 7    /* OCP1 */
#define LAN_MODE_OCPCARD2 8    /* OCP2 */
#define LAN_MODE_MAX 9

#define LOM_NCSI_TYPE 1 /* 只有这个类型才会显示mac地址 */
#define PCIE_NCSI_TYPE 3

typedef enum {
    VLAN_PORT_CONFIG_NCSI = 0, /* 传统VLAN设置 */
    VLAN_PORT_CONFIG_DEDICATED,

    VLAN_PORT_TYPE_NUM
} VLAN_CONFIG_PORT_TYPE;

#define IPMI_LOM_NCSI_TYPE 0
#define IPMI_PCIE_NCSI_TYPE 1
#define IPMI_MANAGEMENT_TYPE 2
#define IPMI_LOM2_NCSI_TYPE 3 // IPMI设置LOM2参与自适应
#define IPMI_OCP_NCSI_TYPE 4
#define IPMI_OCP2_NCSI_TYPE 5
#define IPMI_CARD_MAX_TYPE 6 // 表示BMC对外网口类型的个数，宏名称很烂，待优化

/* 网口类型 */
#define INNER_ETHERNET 1 /* 内部网口 */
#define OUT_ETHERNET 2   /* 外部网口 */
#define VETH_ETHERNET 3  /* VETH */

/* Atlas产品近端维护方案 */
#define INNER_ETHERNET_FOR_ATLAS 4 /* 内部网口，atlas计算节点近端维护使用 */
#define OUT_ETHERNET_FOR_ATLAS 5   /* 外出网口，atlas计算节点近端维护使用 */
#define FLOATIP_ETHERNET 6         /* smm板浮动IP网口 */
#define OTHER_SMM_INNER_ETHERNET 7 /* smm板出对板的内部网口 */
#define MAINT_DEDICATED_ETHERNET  8
#define RACK_INNER_COMM_ETHERNET  9   /* 机柜内部通信网口 */
#define RACK_NODE_INNER_ETHERNET  10  /* 机柜内节点和管理板通讯的内网，和BMC管理网络共物理网卡 */

#define ENABLE_ETH 0x1
#define DISABLE_ETH 0x0

#define ENABLE_IPV4_ONLY 0
#define ENABLE_IPV6_ONLY 1
#define ENABLE_IPV4_AND_IPV6 2

#define IPV6_IP_TYPE_ANY       0x0000U
#define IPV6_IP_TYPE_LINKLOCAL 0x0020U
#define IPV6_IP_SCOPE_MASK     0x00f0U
#define IPV6_ADDR_SITELOCAL    0x0040U

#define IP_ADDRESS 1
#define MASK_ADDRESS 2
#define MAC_ADDRESS 3
#define INIT_MAC_ADDRESS 4

#define MAC_ADDR_LEN 6        /* mac地址长度 */
#define IP_ADDR_LEN 4         /* ip地址长度 */
#define MAC_ETH_NUM 2         /* 单板网口数 */
#define MAX_IPADDR_LENGTH 128 /* 存放IP地址的buffer长度 */

/* IP模式 */
#define IP_UNSPECIFIC_MODE 0
#define IP_STATIC_MODE 1
#define IP_DHCP_MODE 2
#define IP_LOAD_BY_SMS_MODE 3
#define IP_OTHER_PROTOCOL_MODE 4
#define IP_MODE_NUM 5

/* Set LAN Configuration Parameters的返回码 */
#define PARA_UNSUPPORT 0x80
#define PARA_SETTING 0x81 /* 当前正在设置 */
#define SET_READ_ONLY_PARA 0x82
#define READ_WRITE_ONLY_PARA 0x83

#define LAN_CONFIG_ETH_PORT_ENABLE 0xC5
#define LAN_CONFIG_IPv6_ADDR 0xCC
#define LAN_CONFIG_IPv6_PREFIX_LEN 0xCD
#define LAN_CONFIG_IPv6_DEFAULT_GATWAY 0xCE
#define LAN_CONFIG_IPv6_LOCAL_LINK 0xCF
#define LAN_CONFIG_IPv6_ADDR_SRC 0xD0
#define LAN_CONFIG_BMC_NETWORK_MODE 0xD7

#define LAN_COMMAND_NORMALLY 0x00
#define LAN_SET_PARA_NOTSUPPORT 0x80
#define LAN_SET_IN_PROGRESS 0x81
#define LAN_SET_WRITE_READONLY 0x82
#define LAN_SET_READ_WRITEONLY 0x83
#define LAN_GET_PARA_NOTSUPPORT 0x80
#define LAN_SET_TEST_FAILED 0x84
#define LAN_PARAMETER_REVISION 0x11

#define SSDP_IPV6_SCOPE_LINK "Link"
#define SSDP_IPV6_SCOPE_SITE "Site"
#define SSDP_IPV6_SCOPE_ORG "Organization"
#define SSDP_IPV6_LINK "FF02::C"
#define SSDP_IPV6_SITE "FF05::C"
#define SSDP_IPV6_ORG "FF08::C"

/* 浮动IP的网口名后缀 */
#define FLOAT_IP_ETH_NAME_TAIL ":0"
#define VIRTUAL_NETWORK_NAME_TAIL ":1"

#define NET_ADDR_OK 0          /* 合法的网络地址 */
#define NET_ADDR_WRONG -1      /* 错误的网络地址 */
#define NET_ADDR_INVALID_IP -2 /* 不合法的IP地址 */
#define NET_ADDR_INVALID_DN -3 /* 不合法的域名 */

#define ETH_NAME_LEN 16 // IFNAMSIZ
#define ETH_PARAM_IP_CONFLICT_ERROR -6


/* 网络配置参数 */
enum {
    BMC_LANP_SET_IN_PROGRESS,
    BMC_LANP_AUTH_TYPE,        /* 规格不支持 */
    BMC_LANP_AUTH_TYPE_ENABLE, /* 规格不支持 */
    BMC_LANP_IP_ADDR,
    BMC_LANP_IP_ADDR_SRC,
    BMC_LANP_MAC_ADDR, /* 5 */
    BMC_LANP_SUBNET_MASK,
    BMC_LANP_IP_HEADER,
    BMC_LANP_PRI_RMCP_PORT,
    BMC_LANP_SEC_RMCP_PORT,
    BMC_LANP_BMC_ARP, /* 10 */
    BMC_LANP_GRAT_ARP,
    BMC_LANP_DEF_GATEWAY_IP,
    BMC_LANP_DEF_GATEWAY_MAC, /* 规格不支持 */
    BMC_LANP_BAK_GATEWAY_IP,  /* 规格不支持 */
    BMC_LANP_BAK_GATEWAY_MAC, /* 15    规格不支持 */
    BMC_LANP_SNMP_STRING,
    BMC_LANP_NUM_DEST,
    BMC_LANP_DEST_TYPE, /* 规格不支持 */
    BMC_LANP_DEST_ADDR, /* 规格不支持 */
    BMC_LANP_VLAN_ID,   /* 20 */
    BMC_LANP_VLAN_PRIORITY,
    BMC_LANP_RMCP_CIPHER_SUPPORT,
    BMC_LANP_RMCP_CIPHERS,
    BMC_LANP_RMCP_PRIV_LEVELS,
    BMC_LANP_VLANTAG /* 规格不支持 */
    /* BMC_LANP_MOTO_OEM = 0XC0,  设置网口方向 */
};

enum busy_port_availableflag {
    AVAILABLE_FLAG_DISABLED = 0,
    AVAILABLE_FLAG_ENABLED,
    AVAILABLE_FLAG_VIRTUAL = 3
};

#define ETH_UNSUPPORT -3
#define ETH_AVAFLAG_DISABLED -8

#define CHASSIS_SET_SERIAL_NODE_INVALID_ID  (-2)
#define CHASSIS_SET_SERIAL_NODE_NOT_PRESENT (-3)
#define CHASSIS_SET_SERIAL_NODE_UNKNOWN     (-255)

#define MAX_MAC_STR_LEN 32 /* mac地址的字符串形式最大长度 */
#define MAX_MAC_NUM 10     /* 每张网卡上最大的MAC地址数量 */

#define LSW_TYPE_BMC_MGNT 0
#define LSW_TYPE_FABRIC 1
#define LSW_TYPE_BASIC 2

/* 单张网卡最大端口个数 */
#define MAX_NETCARD_PORT 8


#pragma pack(1)
/* IP头 */
typedef struct tg_IPV4_FRAME {
    unsigned char ttl; /* 从1开始，默认64 */
#ifdef BD_BIG_ENDIAN
    unsigned char flag : 3; /* 默认010b */
    unsigned char __reserved : 5;
#else
    unsigned char __reserved : 5;
    unsigned char flag : 3;
#endif

#ifdef BD_BIG_ENDIAN
    unsigned char precedence : 3;  /* 默认0 */
    unsigned char server_type : 4; /* 默认为1000b，最小延迟 */
    unsigned char __reserve2 : 1;
#else
    unsigned char __reserve2 : 1;
    unsigned char server_type : 4;
    unsigned char precedence : 3;
#endif
} IPV4frame, *pIPV4frame;
#pragma pack()

/* Functiontype: 0-表示管理网口，通过管理网口与外置OM对接；1-表示BMC与内置OM对接网口；2-表示BMC与BMC、EMM之间通信网络 */
#define FN_TYPE_OUTTER_OM    0
#define FN_TYPE_INNER_OM     1
#define FN_TYPE_INNER_SHELF  2
#define FN_TYPE_SECOND_OUTTER_OM 3
#define FN_MULTI_PLANE_1     0
#define FN_MULTI_PLANE_2     1

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __NETWORK_DEFINE_H__ */
