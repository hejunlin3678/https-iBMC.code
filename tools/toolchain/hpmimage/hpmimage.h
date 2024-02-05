#ifndef HPMIMAGE_H
#define HPMIMAGE_H

#define IMAGE_NAME                     "image_name"
#define FORMAT_VERSION                 "format_version"               
#define DEVICE_ID                      "device_id"                    
#define MANUFACTURE_ID                 "manufacture_id"
#define PRODUCT_ID                     "product_id"                   
#define IMAGE_CAPABILITIES             "image_capabilities"           
#define COMPONENTS                     "components"                   
#define EARLIEST_COMPATIBLE_REVISION   "earliest_compatible_revision" 
#define FIRMWARE_REVISION              "firmware_revision"            
#define AUXILIARY_FIRMWARE_REVISION    "auxiliary_firmware_revision" 
#define DESCRIPTOR_TYPE_ID             "descriptor_type_id"   
#define END_OF_LIST                    "end_of_list"  
#define OEM_LENGTH                     "oem_length"
#define OEM_DATA_LENGTH                "oem_data_length" 
#define OEM_DATA                       "oem_data"
#define DESCRIPTOR_CHECK               "descriptor_check"    
#define BACKUP_ACTION                  "backup_action"        
#define BACKUP_COMPONENTS              "backup_components"    
#define PREPARE_ACTION                 "prepare_action"       
#define PREPARE_COMPONENTS             "prepare_components"       
#define UPLOAD_ACTION                  "upload_action"              
#define UPLOAD_COMPONENTS              "upload_components"          
#define FIRMWARE_VERSION               "firmware_version"           
#define FIRMWARE_DESCRIPTION           "firmware_description"       
#define FILE_NUMBER                    "file_number"  
#define FILE_PATH                      "file_path"
#define FILE_NAME                      "file_name"  
#define SELFTEST_TIMEOUT               "selftest_timeout"
#define ROLLBACK_TIMEOUT               "rollback_timeout"
#define INACCESSIBILITY_TIMEOUT        "inaccessibility_timeout"


#define SIGNATURE                      "PICMGFWU"
#define MAX_PATH_LEN                   120
#define MAX_NAME_LEN                   120
#define HPM_OK                         1
#define HPM_ERR                        0
#define HPM_ERROR                      -1
#define HEADER_LEN                     37 

#define OEM_DATA_ITEM                  5
#define UPGRADE_ACTION_ITEM            8
#define MAX_BANK_NUMBER                8
#define RESERVE_SPACE                  256
#define UPLOAD_FIRMWARE_IMAGE          2
#define MAX_FIRMWARE_DES               21
typedef struct g_image_header
{
	unsigned char signature[8];        /*PICMGFWU*/
	unsigned char format_version;  
	unsigned char device_id;
	unsigned char manufacture_id[3];       /*小字序*/
	unsigned char product_id[2];           /*小字序*/
	unsigned char image_time[4];           /*镜像文件创建事件*/
	unsigned char image_capabilities;
	unsigned char components[4];
	unsigned char selftest_timeout;         /*以5秒为单位*/
	unsigned char rollback_timeout;         /*以5秒为单位*/
	unsigned char inaccessibility_timeout;  /*以5秒为单位*/
	unsigned char earliest_revision[2];
	unsigned char firmware_revision[2];
	unsigned char auxiliary_revision[4];
	unsigned char oem_data_length[2];       /*小字序*/
	unsigned char *oem_descriptor;
	unsigned char header_checksum;	
}image_header, *pimage_header;

typedef struct g_oem_data_record
{
	unsigned char descriptior_type;
	unsigned char end_of_list;
	unsigned char length[2];                /*小字序*/
	unsigned char manufacture_id[3];        /*小字序*/
	unsigned char check_sum;
	unsigned char *data;
}oem_data_record, *poem_data_record;

typedef struct g_bank_information
{
	unsigned char offset[4];
	unsigned char length[4];
	unsigned char file_path[24];
	unsigned char file_name[MAX_NAME_LEN];
}bank_info, *pbank_info;

typedef struct g_upgrade_action_records
{
	unsigned char action_type;
	unsigned char components[4];
	unsigned char head_checksum;
	unsigned char firmware_vision[2];
	unsigned char auxiliary_vision[4];
	unsigned char firmware_des[MAX_FIRMWARE_DES];
	unsigned char firmware_length[4];
	unsigned char reserved[RESERVE_SPACE];
	bank_info bank_information[MAX_BANK_NUMBER];
	unsigned char bank_number;                     /*此部件包含的 bank个数*/
}upgrade_action_record, *pupgrade_action_record;

#endif
