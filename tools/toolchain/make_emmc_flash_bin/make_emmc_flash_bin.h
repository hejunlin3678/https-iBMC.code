#define LINE_MAX 1000
#define ECSD_MAX_LEN 1000
#define MMC_PERLINE_MAX_LEN 300
#define MMC_BLOCK_SIZE 512
#define SKIP_BLANK_LEN 1000
// 各个分区的配置文件打印格式，其中20190726表示时间
#define BOOT_info "	#	BOOT%d:	@%d	,%d	= D:\\GPP_bin\\20190726\\Hi1711_boot_4096.bin/00h|%X\n"
#define GPP_info "	#	GPP%d:	@%d	,%d	= D:\\GPP_bin\\20190726\\GPP_rootfs.bin/00h|%X\n"
#define GPP_4_INFO "	#	GPP%d:	@%d	,%d	= D:\\GPP_4_bin\\20190726\\GPP_4.img/00h|%X\n"
#define USER_info "	#	USER:	@%d	,%d	= D:\\GPP_bin\\20190726\\ipmc.bin/00h|%lX\n"
#define UBOOT_NAME "Hi1711_boot_4096.bin"
#define GPP_ROOTFS_NAME "GPP_rootfs_BOARDNAME.bin"
#define USER_BIN_NAME "ipmc.bin"
#define GPP_4_NAME "GPP_4.img"
#define XML_MMC_INFO                                                                                            \
    "        <Entry Name = \"%04d\" FileBlockPos = \"0x%08X\" ChipBlockPos = \"0x%08X\" BlockNum = \"0x%08X\" " \
    "Partition = \"%s\"/>\n"

typedef signed long VOS_INT32;
typedef enum emmc_partition_order {
    Boot0 = 1,
    Boot1,
    GPP1,
    GPP2,
    GPP3,
    GPP4,
    USER
} partition_order;

typedef struct tag_emmc_partition_s {
    partition_order bmc_partition_order;
    char *str_bmc_partition;
} EMM_PATITION_S;

typedef struct tag_emc_partition_info {
    partition_order bmc_partition_order;
    int order_num;
    char *emc_info;
}EMM_PATITION_INFO;

