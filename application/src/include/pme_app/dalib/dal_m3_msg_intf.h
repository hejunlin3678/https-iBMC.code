/* 一级命令定义 */
#define CMD_1_SEC_DICE    0x01
#define CMD_1_SEC_FW      0x02

/* 安全核下的开发场景证书管理二级命令 */
#define CMD_2_IMPORT_PARTNER         0x11      /* 导入伙伴证书 */
#define CMD_2_EXPORT_PARTNER_INFO    0x12      /* 导出伙伴信息 */
#define CMD_2_EXPORT_REPAIR_INFO     0x13      /* 导出维修信息 */
#define CMD_2_IMPORT_REPAIR_CERT     0x14      /* 导入维修凭证 */
#define CMD_2_SET_M3_VERIFY_INVALID  0x15      /* 失能M3校验 */

// response的CMD1 = 发送的CMD1 + 0x80

/* 伙伴根类型 */
#define PARTNER_PCERT_TYPE_PKCS       0x77aa77aa  /* pkcs算法伙伴根类型 */
#define PARTNER_PCERT_TYPE_PSS        0x77aa77ab  /* pss算法伙伴根类型 */

/* SEC_DICE下的二级命令 */
#define CMD_2_SET_CERT0          0x01      /* 导入CERT0证书 */
#define CMD_2_GET_CERT_N         0x02      /* 导出指定的CERT证书 */
#define CMD_2_GET_LAST_CERT      0x03      /* 导出最后一级CERT证书 */
#define CMD_2_GET_CSR0           0x04      /* 导出CSR0 */

/* SEC_FW下的二级命令 */
#define CMD_2_GET_BIOS_MUX_STATUS    	     0x01  /* 获取BIOS MUX控制状态 */
#define CMD_2_SET_BIOS_MUX_STATUS    	     0x02  /* 设置BIOS MUX控制状态 */
#define CMD_2_VERIFY_BIOS              	     0x03  /* 请求校验BIOS */
#define CMD_2_GET_BIOS_VERIFY_STATUS  	     0x04  /* 获取BIOS校验结果、状态 */
#define CMD_2_SET_CUSTOMER_INFO       	     0x05  /* 设置客户密钥信息 */			//not ready
#define CMD_2_GET_CUSTOMER_INFO       	     0x06  /* 获取客户密钥信息 */
#define CMD_2_SET_BMC_UPDATE_FLAG            0x07  /* 设置升级标志 */
#define CMD_2_GET_BMC_SECUREBOOT_INFO        0x08  /* 获取安全启动信息 */
#define CMD_2_GET_BMC_LAST_UPGADE_RESULT     0x0c  /* 获取BMC上次升级结果 */
#define CMD_2_SET_GUEST_INFO                 0x0d
#define CMD_2_FW_MSG_BMC_ROOTKEY_UPDATE      0x18  /* KMC硬件根秘钥更新 */
#define CMD_2_FW_MSG_BMC_MASTER_KEY_ENCODE   0X19  /* KMC硬件根秘钥加密主秘钥 */
#define CMD_2_FW_MSG_BMC_MASTER_KEY_DECODE   0x20  /* KMC硬件根秘钥解密主秘钥 */
#define CMD_2_FW_MSG_PARTNER_VERIFY_H2P_CERT 0x21  /* 校验伙伴的h2p证书 */
#define CMD_2_FW_MSG_TPCM_COMMAND            0x22  /* TPCM 主命令 */

enum tpcm_cmd {
    TPCM_GET_LOG,
    TPCM_GET_PCR,
    TPCM_GET_RANDOM,
    TPCM_GET_SM3_HASH,
    TPCM_CMD_QUEUE_TCM_IS_ENABLE,
    TPCM_INVALID_CMD,
};

#define M3_CMD_MAX_LEN    4094
#define M3_CMD_MIN_LEN    4

/* error type */
typedef enum {
    /* partner info import */
    MSG_ERR_PARTNER_CHECK_FAIL = 0X7001, /* partner info field check fail */
    MSG_ERR_PARTNER_MEM_FAIL,            /* partner info get memory fail */
    MSG_ERR_PARTNER_LOAD_FAIL,           /* partner info read fail */
    MSG_ERR_PARTNER_NON_SEC,             /* firmware & bios are both non secure boot */
    MSG_ERR_PARTNER_MODE_ERR,            /* partner mode is not enable */
    MSG_ERR_PARTNER_FW_V_NALLOWED,       /* firmware is not secure boot, not allowd to enable verify */
    MSG_ERR_PARTNER_BIOS_V_NALLOWED,     /* bios is not secure boot, not allowd to enable verify */
    MSG_ERR_PARTNER_S1_IMPORTED,         /* partner info S1 have been imported */
    MSG_ERR_PARTNER_S2_IMPORTED,         /* partner info S2 have been imported */
    MSG_ERR_PARTNER_ALL_IMPORTED,        /* partner infos have been imported */
    MSG_ERR_PARTNER_H2P_LOAD_FAIL,       /* h2p cert load fail */
    MSG_ERR_PARTNER_H2P_VERIFY_FAIL,     /* h2p cert verify fail */
    MSG_ERR_PARTNER_PINFO_VERIFY_FAIL,   /* partner info verify fail */
    MSG_ERR_PARTNER_PINFO_WRITE_FAIL,    /* partner info write fail */
    /* partner mode repair */
    MSG_ERR_PARTNER_NOT_IMPORTED,        /* partner is not imported */
    MSG_ERR_PARTNER_RCERT_DIEID_ERR,     /* partner repair cert dieid is not matched */
    MSG_ERR_PARTNER_RCERT_ALLREVOKED,    /* partner all repair forbid bits has been used */
    MSG_ERR_PARTNER_RCERT_REVOKED,       /* partner repair cert has been used */
    MSG_ERR_PARTNER_RCERT_VERIFY_VFAIL,  /* partner repair cert verify fail */
    MSG_ERR_PARTNER_PINFO_CLR_VFAIL,     /* partner info write to rpmb fail */
    MSG_ERR_PARTNER_FORBID_UPDATE_VFAIL, /* partner info write forbid bit to efuse fail */
} MSG_A55_SCM3_ERROR_CODE_E;

/* 返回的错误码，由开发补充完整 */
typedef enum
{
    RESP_SUCCESS = 0,
    CMD_NOT_SUPPORT,
    CMD_PARAM_ERROR,
    CMD_1_RESP_CODE = 0x0100,
    CMD_2_RESP_CODE = 0x0200,
}sec_response_code_e;

#pragma pack(1)

#define HASH_SIZE               32  /* 根公钥HASH 256bit */
#define RSA_ROOTKEY_WIDTH_BYTES 512  /* 二级密钥签名结果, 长度为512byte */
#define M3_RESP_MAX_LEN 4096

typedef struct tag_partner_info_s
{
    /* 请求体入参 */
    guint32 preamble; /* 文件头前导码 0x77aa77aa 读取启动参数需要检查该值 */
    guint32 data_len;                                    /* 数据长度 */
    guint32 verify_en;                                   /* 伙伴安全启动使能 */
    guint32 bios_verify_en;                              /* 伙伴BIOS安全启动使能 */
    guint32 pkcs1_5_forbid;                              /* 填充算法pkcs1_5禁用 */
    guint32 rsa_alg_param;                               /* 填充算法参数 */
    guint32 bios_category;                               /* bios 二级类别 */
    guint32 forbid_list_offset;                          /* hash黑名单偏移，给info预留2K */
    guint8 root_pubk_hash[HASH_SIZE];                    /* RootKeyHash根公钥HASH 256bit */
    guint8 root_cert_hash[HASH_SIZE];                    /* 根公钥cert HASH 256bit */
    guint8 info_sign[RSA_ROOTKEY_WIDTH_BYTES]; /* H2P密钥签名结果, 长度为512byte */
} PARTNER_INFO_S;


// 驱动返回的error code定义
typedef enum
{
    MSG_TX_LEN_ERROR = 0,		/* 不能超过4K，不能小于6B */
    MSG_RX_LEN_ERROR,			/* 不能超过4K，不能小于8B */
}sec_msg_error_code_e;
	
typedef struct tag_sec_cmd_header_s
{
    guint8 		cmd_1;  		/* 一级命令 */
    guint8 		cmd_2;  		/* 二级命令 */
    guint8 		reserved[2];  	/* 预留 */
} SEC_CMD_HEADER_S;

/* 设置可变更证书 */
typedef struct tag_sec_set_cert0_req_s
{
    SEC_CMD_HEADER_S cmd_header;
    guint16 cert_len;
    guint8 *p_cert_data[0];
} SEC_SET_CERT0_REQ_S;

typedef struct tag_sec_set_cert0_resp_s
{
    SEC_CMD_HEADER_S cmd_header;
    guint16 resp_code;
} SEC_SET_CERT0_RESP_S;

typedef struct tag_sec_import_partner_cert_req {
    SEC_CMD_HEADER_S hd;
    PARTNER_INFO_S info;
} SEC_IMPORT_PARTNER_CERT_REQ_S;

typedef struct tag_sec_import_partner_cert_resp_s
{
    SEC_CMD_HEADER_S cmd_header;
    guint16 resp_code;
} SEC_IMPORT_PARTNER_CERT_RESP_S;

typedef struct tag_SEC_VERIFY_PARTNER_H2P_REQ {
    SEC_CMD_HEADER_S cmd_header;
    guint16 crc_16;
} SEC_VERIFY_PARTNER_H2P_REQ_S;

typedef struct tag_SEC_VERIFY_PARTNER_H2P_RSP {
    SEC_CMD_HEADER_S cmd_header;
    guint16 resp_code;
    guint16 crc_16;
} SEC_VERIFY_PARTNER_H2P_RSP_S;

typedef enum {
    SEC_FW_NON_SECURE_BOOT,
    SEC_FW_FACTORY_BOOT,
    SEC_FW_GUEST_BOOT,
    SEC_FW_DOUBLE_ROOT_BOOT,
} SEC_FW_BOOT_MODE_E;

typedef struct tag_sec_get_efuse_state_req_s {
    SEC_CMD_HEADER_S cmd_header;
    guint16 crc_16;
} SEC_GET_EFUSE_STATE_REQ_S;

typedef struct tag_sec_get_efuse_state_rsp_s {
    SEC_CMD_HEADER_S cmd_header;
    guint16 resp_code;
    guint16 boot_mode;
    guint16 boot_partition;
    guint16 recover_status;
    guint16 crc_16;
} SEC_GET_EFUSE_STATE_RSP_S;

typedef struct tag_sec_export_partner_cert_req {
    SEC_CMD_HEADER_S hd;
    gint32 preamble;
} SEC_EXPORT_PARTNER_CERT_REQ_S;

typedef struct tag_sec_export_partner_cert_s
{
    SEC_CMD_HEADER_S cmd_header;
    guint16 rsp_code;
    guint32 partner_mode_en;
    PARTNER_INFO_S info;
} SEC_EXPORT_PARTNER_CERT_RESP_S;

typedef struct tag_set_m3_verify_invalid_req {
    SEC_CMD_HEADER_S hd;
} SEC_M3_VERIFY_INVALID_REQ_S;

typedef struct tag_set_m3_verify_invalid_resp_s
{
    SEC_CMD_HEADER_S cmd_header;
    guint16 resp_code;
} SEC_M3_VERIFY_INVALID_RESP_S;

typedef struct tag_sec_export_partner_repair_req {
    SEC_CMD_HEADER_S hd;
} SEC_EXPORT_PARTNER_REPAIR_REQ_S;

typedef struct {
    guint8 dieid[20];       /* dieid 160 bits */
    guint8 repair_forbid;     /* repair forbid bitmap 8 bits */
    guint8 reverse[3];       /* 对齐预留 */
} PARTNER_REPAIR_INFO_S;

typedef struct tag_sec_export_partner_repair_s {
    SEC_CMD_HEADER_S cmd_header;
    guint16 rsp_code;
    PARTNER_REPAIR_INFO_S rinfo;
} SEC_EXPORT_PARTNER_REPAIR_RESP_S;

typedef struct tag_partner_repair_cert_s {
    PARTNER_REPAIR_INFO_S info;
    guint32 sign_alg;
    guint32 rsa_sign[RSA_ROOTKEY_WIDTH_BYTES];
} PARTNER_REPAIR_CERT_S;

typedef struct tag_sec_clr_partner_pinfo_req {
    SEC_CMD_HEADER_S hd;
    PARTNER_REPAIR_CERT_S rcert;
}SEC_CLR_PARTNER_PINFO_REQ;

typedef struct tag_sec_get_partner_rinfo_rsp_s {
    SEC_CMD_HEADER_S hd;
    guint16 rsp_code;
    PARTNER_REPAIR_INFO_S rinfo;
}SEC_GET_PARTNER_RINFO_RSP_S;

/* 获取指定序号证书 */
typedef struct tag_sec_get_cert_n_req_s
{
    SEC_CMD_HEADER_S cmd_header;
    guint16 index;
} SEC_GET_CERT_N_REQ_S;

typedef struct tag_sec_get_cert_n_resp_s
{
    SEC_CMD_HEADER_S cmd_header;
    guint16 resp_code;
    guint16 index;
    guint16 cert_len;
    guint8 *p_cert_data[0];
} SEC_GET_CERT_N_RESP_S;

/* 获取最后一级证书 */
typedef struct tag_sec_get_last_cert_req_s
{
    SEC_CMD_HEADER_S cmd_header;
    guint16 nonce_len;
    guint8 *p_nonce_data[0];
} SEC_GET_LAST_CERT_REQ_S;

typedef struct tag_sec_get_last_cert_resp_s
{
    SEC_CMD_HEADER_S cmd_header;
    guint16 resp_code;
    guint16 cert_len;
    guint8 *p_cert_data[0];
} SEC_GET_LAST_CERT_RESP_S;

/* 获取0层CSR */
typedef struct tag_sec_get_csr0_req_s
{
    SEC_CMD_HEADER_S cmd_header;
} SEC_GET_CSR0_REQ_S;

typedef struct tag_sec_get_csr0_resp_s
{
    SEC_CMD_HEADER_S cmd_header;
    guint16 resp_code;
    guint16 csr_len;
    guint8 *p_cert_data[0];
} SEC_GET_CSR0_RESP_S;

/* 获取客户密钥信息 */
typedef struct tag_sec_get_customer_info_req_s
{
    SEC_CMD_HEADER_S 	cmd_header;
} SEC_GET_CUSTOMER_INFO_REQ_S;

typedef struct tag_sec_get_customer_info_resp_s
{
    SEC_CMD_HEADER_S 	cmd_header;
    guint16 		resp_code;
    guint8 			hash_algorithm;					/* 0: sha256; others: reserved*/
    guint8*			guest_rootpk_hash;				/* the size depends on the hash algorithm, for sha256, the size = 32byte */
    guint32			guest_subkeycategory;			/* 客户二级密钥分类 */
    guint32			guest_cms_root_cert_hash;		/* 客户CMS签名根证书哈希 */
    guint8			drsb_ksw_en;			    /* 双根、密钥切换是能位状态信息；0: 厂家单根；1：双根；2：客户单根；others：保留 */
} SEC_GET_CUSTOMER_INFO_RESP_S ;

/* 设置升级标志*/
typedef struct tag_sec_set_upgrade_flag_req_s
{
    SEC_CMD_HEADER_S cmd_header;
	guint16				update_flag;		/* bit0: m3 fw updated(include l0 fw/l1 fw); bit1: uboot fw updated; bit2: uboot updated; bit2: rootfs upadted */
} SEC_SET_UPGRADE_FLAG_REQ_S;

typedef struct tag_sec_set_upgrade_flag_resp_s
{
    SEC_CMD_HEADER_S cmd_header;
    guint16 resp_code;
} SEC_SET_UPGRADE_FLAG_RESP_S;

/* 获取安全启动信息*/
typedef struct tag_sec_get_secureboot_info_req_s
{
    SEC_CMD_HEADER_S cmd_header;
} SEC_GET_SECURE_BOOT_INFO_REQ_S;

/*
BOOTMODE(1BYTE)
0：Secure Boot Not Supported
1：厂家单根启动
2：客户单根启动
3：双根启动
others: reserved"	"BOOTPARTITION(1BYTE)
bit0: 0, SEC_M3 FW boot from active partition
      1, SEC_M3 FW boot from gold partition
bit1: 0, BMC bootloader boot from active partition
      1, BMC bootloader boot from gold partition
bit2: 0, BMC ROOTFS boot from active partition
      1, BMC ROOTFS boot from gold partition
others: reserved."	"RECOVER STATUS(1byte)
bit0 = 1: SEC_M3 recovered
bit1 = 1: BMC bootloader recovered
bit2 = 1: BMC ROOTFS recovered
others: reserved
（非eMMC场景，或产品不需要安全M3做主动恢复的场景时，安全M3不做恢复动作） */
typedef struct tag_sec_get_secureboot_info_resp_s
{
    SEC_CMD_HEADER_S 	cmd_header;
    guint16 			resp_code;
    guint16 			boot_mode;
    guint16 			boot_partition;
    guint16 			recover_status;
} SEC_GET_SECURE_BOOT_INFO_RESP_S;

#ifndef TPCM_LOG_MAX_LEN
#define TPCM_LOG_MAX_LEN 1024  /* TPCM日志最大长度, 当前长度设置为1024byte，注意最大应当小于4K */
#endif

#define TPCM_TCM_CMD_READ_PCR      0x01
#define TPCM_TCM_CMD_SM3           0x02
#define TPCM_TCM_CMD_READ_LOG      0x03
#define TPCM_TCM_CMD_READ_RANDOM   0x04

typedef struct tag_sec_get_tpcm_info_req_s
{
    SEC_CMD_HEADER_S cmd_header;
} SecGetTPCMInfoRrq;
typedef struct tag_sec_get_tpcm_info_resp_s
{
    SEC_CMD_HEADER_S 	cmd_header;
    guint16 			resp_code;
    guint8 			    data[TPCM_LOG_MAX_LEN];
    guint32             len;
} SecGetTPCMInfoResp;

#define TPCM_TCM_REQ_DATA_MAX_LEN  2048  /* TCM输入数据最大长度, 当前长度设置为32byte，担心M3资源不够用，暂不使用太大buff，注意最大应当小于4K */
#define TPCM_TCM_RESP_DATA_MAX_LEN 2048  /* TCM输出数据最大长度, 当前长度设置为1024byte，注意最大应当小于4K */

typedef struct {
    SEC_CMD_HEADER_S    cmd_header;
    guint32              reserved;
} SecGetTPCMLogReq;

typedef struct {
    SEC_CMD_HEADER_S    cmd_header;
    guint32             reserved;
    guint16             pcr_index;
} SecGetTPCMPcrReq;

typedef struct {
    SEC_CMD_HEADER_S    cmd_header;
    guint32             reserved;
    guint16             random_len;
} SecGetTPCMRandomReq;

typedef struct {
    SEC_CMD_HEADER_S    cmd_header;
    guint32             reserved;
    guint16             data_len;
    guint8              data_buf[TPCM_LOG_MAX_LEN];
} SecGetTPCMSM3Req;

typedef struct tag_sec_get_tpcm_tcm_req_s {
    SEC_CMD_HEADER_S    cmd_header;
    guint16 			tcm_command;
    guint16             tcm_sub_command;
    guint32             len; // 实际有效的数据长度
    guint8 			    data[TPCM_TCM_REQ_DATA_MAX_LEN]; // 给出的data buffer长度
} SecGetTPCMTCMReq;
typedef struct tag_sec_get_tpcm_tcm_resp_s {
    SEC_CMD_HEADER_S 	cmd_header;
    guint16 			resp_code;
    guint16             len;// 实际返回的有效数据长度
    guint8 			    data[TPCM_TCM_RESP_DATA_MAX_LEN];
} SecGetTPCMTCMResp;

typedef struct tag_sec_get_tpcm_tcm_enable_resp_s {
    SEC_CMD_HEADER_S 	cmd_header;
    guint16 			resp_code;
    guint8 			    data[TPCM_TCM_RESP_DATA_MAX_LEN];;
} SecGetTPCMTCMEnableResp;

/* 设置BMC 启动OK的标志*/
typedef struct tag_sec_set_bmc_boot_ok_req_s {
    SEC_CMD_HEADER_S cmd_header;
} SEC_SET_BMC_BOOT_OK_REQ_S;

typedef struct tag_sec_set_bmc_boot_ok_resp_s {
    SEC_CMD_HEADER_S cmd_header;
    guint16 resp_code;
} SEC_SET_BMC_BOOT_OK_RESP_S;

typedef struct tag_sec_set_efuse_pwr_ctrl_status_resp_s {
    SEC_CMD_HEADER_S 	cmd_header;
    guint16 			       resp_code;
    guint16				status; 			/* 0: efuse power off; 1: efuse power on*/
} SEC_SET_EFUSE_PWR_CTRL_STATUS_RESP_S;

/* 获取上次升级时的信息 */
typedef struct tag_sec_get_last_update_info_req_s {
    SEC_CMD_HEADER_S 	cmd_header;
} SEC_GET_LAST_UPDATE_INFO_REQ_S;
/*
status
bit0: 1: M3 FW BP1 UPDATE FAILED
bit1: 1: UBOOT BP1 UPDATE FAILED
bit2: 1: M3 FW BP2 UPDATE FAILED
bit3: 1: UBOOT BP2 UPDATE FAILED
bit4: 1: GPP TEMP REGION VERIFY FAILED
*/
typedef struct tag_sec_get_last_update_info_resp_s {
    SEC_CMD_HEADER_S 	cmd_header;
    guint16  resp_code;
    guint16  status;
} SEC_GET_LAST_UPDATE_INFO_RESP_S;

typedef struct tag_sec_set_customer_info_req_s {
    SEC_CMD_HEADER_S cmd_header;
    guint16 reserved; /* 保留，填0 */
} SEC_SET_CUSTOMER_INFO_REQ_S;
typedef struct tag_sec_set_customer_info_resp_s {
    SEC_CMD_HEADER_S cmd_header;
    guint16 resp_code; /* 0: 命令正确且成功收到 */
} SEC_SET_CUSTOMER_INFO_RESP_S;


//SEC GPIO0用于控制BIOS MUX切换；（IO输出0，切换到HOST CPU侧；输出1（默认），切换到BMC侧）
/* 获取BIOS MUX控制状态 */
typedef struct tag_sec_get_bios_mux_ctrl_status_req_s {
    SEC_CMD_HEADER_S 	cmd_header;
} SEC_GET_BIOS_MUX_CTRL_STATUS_REQ_S;

/*
status
0: bios spi channel switched to host cpu;
1: bios spi channel switched to bmc;
*/
typedef struct tag_sec_get_bios_mux_ctrl_status_resp_s {
    SEC_CMD_HEADER_S 	cmd_header;
    guint16 			resp_code;
    guint16				status;
} SEC_GET_BIOS_MUX_CTRL_STATUS_RESP_S;

/* 设置BIOS MUX控制状态，如果未校验，或是校验失败，禁止切到host侧 */
/*
status
0: bios spi channel switched to host cpu;
1: bios spi channel switched to bmc;
*/
#define BIOS_MUX_TO_CPU 0
#define BIOS_MUX_TO_BMC 1

typedef struct tag_sec_set_bios_mux_ctrl_status_req_s {
    SEC_CMD_HEADER_S 	cmd_header;
    guint16				status; 			/* 0: bios spi channel switched to host cpu; 1: bios spi channel switched to bmc*/
} SEC_SET_BIOS_MUX_CTRL_STATUS_REQ_S;

/*
status
0: bios spi channel switched to host cpu;
1: bios spi channel switched to bmc;
*/
typedef struct tag_sec_set_bios_mux_ctrl_status_resp_s {
    SEC_CMD_HEADER_S 	cmd_header;
    guint16 			resp_code;
    guint16				status;
} SEC_SET_BIOS_MUX_CTRL_STATUS_RESP_S;

/* 设置BIOS校验，在切换spi flash控制开关到host侧前需要做 */
/*
bios_type
0：表示X86 bios;
1：表示ARM 1620
*/
typedef struct tag_sec_verify_bios_req_s {
    SEC_CMD_HEADER_S 	cmd_header;
    guint16				bios_type;
} SEC_VERIFY_BIOS_REQ_S;

typedef struct tag_sec_verify_bios_resp_s {
    SEC_CMD_HEADER_S 	cmd_header;
    guint16 			resp_code;
} SEC_VERIFY_BIOS_RESP_S;

/* 获取BIOS 校验结果*/
typedef struct tag_sec_get_bios_verify_result_req_s {
    SEC_CMD_HEADER_S 	cmd_header;
} SEC_GET_BIOS_VERIFY_RESULT_REQ_S;

/*
result
0: Verify Success；
1: Verify Fail；
2: Busy
*/
typedef struct tag_sec_get_bios_verify_result_resp_s {
    SEC_CMD_HEADER_S 	cmd_header;
    guint16 			resp_code;
    guint16				result;
} SEC_GET_BIOS_VERIFY_RESULT_RESP_S;

/* KMC硬件根秘钥更新请求 */
typedef struct tag_sec_bmc_rootkey_update_req_s {
    SEC_CMD_HEADER_S    cmd_header;
    guint16             crc_16;
} __attribute__((packed)) SEC_BMC_ROOTKEY_UPDATE_REQ_S;

/* KMC硬件根秘钥更新响应 */
typedef struct tag_sec_common_msg_resp_s {
    SEC_CMD_HEADER_S    cmd_header;
    guint16             resp_code;
    guint16             crc_16;
} __attribute__((packed)) SEC_COMMON_MSG_RESP_S;

/* KMC主秘钥核间通信结构体(硬件保护根秘钥) */
typedef struct tag_bmc_master_key_info_s {
    guint32             key_len;
    guint8              iv[16];
    guint8              master_key[256];
} __attribute__((packed)) BMC_MASTER_KEY_INFO_S;

/* KMC主秘钥加解密请求(硬件保护根秘钥) */
typedef struct tag_sec_bmc_master_key_encdec_req_s {
    SEC_CMD_HEADER_S    cmd_header;
    guint16             reserved;
    BMC_MASTER_KEY_INFO_S   info;
    guint16             crc_16;
} __attribute__((packed)) SEC_BMC_MASTER_KEY_ENCDEC_REQ_S;

/* KMC主秘钥加解密响应(硬件保护根秘钥) */
typedef struct tag_sec_bmc_master_key_encdec_resp_s {
    SEC_CMD_HEADER_S    cmd_header;
    guint16             resp_code;
    BMC_MASTER_KEY_INFO_S   info;
    guint16             crc_16;
} __attribute__((packed)) SEC_BMC_MASTER_KEY_ENCDEC_RESP_S;
#pragma pack()

gint32 dal_get_bmc_tpcm_info(TPCMInfo *p_tpcm_info);
gint32 dal_get_bmc_tpcm_info_backup(TPCMInfo *p_tpcm_info);
gint32 dal_get_bmc_tpcm_pcr_info(guint8 pcr_index,TPCMInfo *p_tpcm_info);
gint32 dal_get_bmc_tpcm_random(TPCMInfo *p_tpcm_info);
gint32 dal_get_bmc_tpcm_sm3(TPCMInfo *p_tpcm_info, guint8 *data_in);
gint32 dal_get_bmc_tpcm_enable(guint8 *enable_flag);