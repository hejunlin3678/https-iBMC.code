#define vendorSig ENABLE //ENABLE时需要保证厂家签名路径有效
#define customerSig ENABLE //ENABLE时需要保证客户签名路径有效
#define GPP_HEADER_OK           0
#define GPP_HEADER_ERR          (-1)
#define MAX_PATH_LEN            120
#define MAX_NAME_LEN            128
#define MAX_LINE_LEN            256
#define TOP_HEADER_SIZE         512
#define MAX_Component_Num       10
#define GPP_SIZE                 "GPP_MAX_SIZE"
#define FW_NUM                   "FW_NUM"
#define VendorRootPubkCertPath   "VendorRootPubkCertPath"
#define VendorCMSSignaturePath   "VendorCMSSignaturePath"
#define VendorCRLPath            "VendorCRLPath"
#define CustomerRootPubkCertPath "CustomerRootPubkCertPath"
#define CustomerCMSSignaturePath "CustomerCMSSignaturePath"
#define CustomerCRLPath          "CustomerCRLPath"
#define ROOTFSPath               "ROOTFSPath" 

#define M3_UBOOT_COMPONENT_TYPE 0
#define ROOTFS_HEAD_COMPONENT_TYPE 1
#define ROOTFS_IMG_COMPONENT_TYPE 2
#define CMS_UBOOT_COMPONENT_TYPE 4     // M3带CMS签名的UBOOT文件（不包含L0/L1）

typedef unsigned int   uint32;
typedef unsigned short uint16;
typedef char uint8;

char vendorRootPubkCertPath[MAX_NAME_LEN] = "NULL"; //Type = 16和Offset = 2KB; (0-2KB预留给subImageHeader） Length = 文件实际大小
char vendorCMSSignaturePath[MAX_NAME_LEN] = "NULL"; //Type = 17和Offset = 2KB + ((vendorRootPubkCert_Length + 2KB - 1)/2KB)*2KB; Length = 文件实际大小
char vendorCRLPath[MAX_NAME_LEN] = "NULL"; //Type = 18和Offset = vendorCMSSignature_Offset + ((vendorCMSSignature_Length + 2KB - 1)/2KB)*2KB; Length = 文件实际大小
char customerRootPubkCertPath[MAX_NAME_LEN] = "NULL"; //Type = 24和Offset = vendorCRL_Offset + ((vendorCRL_Length + 2KB - 1)/2KB)*2KB; Length = 文件实际大小
char customerCMSSignaturePath[MAX_NAME_LEN] = "NULL"; //Type = 25和Offset = customerRootPubkCert_Offset + ((customerRootPubkCert_Length + 2KB - 1)/2KB)*2KB; Length = 文件实际大小
char customerCRLPath[MAX_NAME_LEN] = "NULL"; //Type = 26和Offset = customerCMSSignature_Offset + ((customerCMSSignature_Length + 2KB - 1)/2KB)*2KB; Length = 文件实际大小
char rootFSPath[MAX_NAME_LEN] = "NULL"; //Type = 4和Offset = 0; Length = 文件实际大小

typedef struct {
    char* VendorSig;
    char* CustomerSig;
    char* vendorRootPubkCertPath;
    char* vendorCMSSignaturePath;
    char* vendorCRLPath;
    char* customerRootPubkCertPath;
    char* customerCMSSignaturePath;
    char* customerCRLPath;
    char* rootFSPath;
}Subheader_s;

typedef struct {
    uint32 sectionType;
    uint32 sectionOffset;
    uint32 sectionLength;
}section_s;

typedef struct {
    uint32 preamble; //0x55aa55aa
    uint32 fileLen; // = Customer CRL Section Offset + Section Length;
    uint16 firmwareType; // rootfs file type is 3
    uint16 sectionNumber; // The Max is 7: SubHeader, vendorRootPubkCert, vendorCMSSignature, vendorCRL, customerRootPubkCert, customerCMSSignature, customerCRL
    uint8 reserved[64];
    section_s section16;
    section_s section17;
    section_s section18;
    section_s section24;
    section_s section25;
    section_s section26;
    section_s section4;
    uint32 headMagic; //0x33cc33cc
}subImageHeader_s;

typedef struct {
    uint32 component_type;
    uint32 component_offset;
    uint32 component_length;
    uint32 reserved;
} COMPONET_S;

typedef struct { // 512byte
    uint8 component_num;
    uint8 reserved[15];
    COMPONET_S component[31];
} TOPIMAGEHEADER_S;