/*
* Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
* Description: header file that contains constant.
* Author: AdaptiveLM team
* Create: 2015-07-14
*/
#ifndef ALM_INTF_CONSTANT_H
#define ALM_INTF_CONSTANT_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#if defined(ALM_PACK_4)
#pragma pack(4)
#elif defined(ALM_PACK_8)
#pragma pack(8)
#endif

/* AdaptiveLM��ǰ�汾��� */
#define ALM_CONST_LIC_CBB_C_VER "AdaptiveLMV100R007C00SPC001"
#define ALM_CONST_LIC_CBB_B_VER "AdaptiveLMV100R007C00SPC001B030"
#define ALM_CONST_LIC_CBB_D_VER "AdaptiveLMV100R007C00SPC001B030T63266"

/* **************************** ���ó��� ********************************* */
/* FIXTIME��ѯ����״̬�� */
#define ALM_CONST_FIXTIME_GROUPNUM 4

/* һ�㶨�������õĳ��� */
#define ALM_CONST_NAME_MAX_LEN 63

/* ����ת��Ϊ�ַ����ĳ���xxxx-xx-xx xx:xx:xx */
#define ALM_CONST_DATE_LEN 19

/* ��ϣ����ĳ��� */
#define ALM_CONST_HASH_LEN 72

/* ************************ License�ļ�������� ************************** */
/* ʧЧ�����󳤶� */
#define ALM_CONST_RVK_TIKET_LEN 128

/* ��Ʒ����󳤶ȣ�����License�ļ��в�Ʒ����ע���Ʒʱָ���Ĳ�Ʒ�� */
#define ALM_CONST_PRD_NAME_LEN_MAX 54

/* ��Ʒ�汾��󳤶ȣ�����License�ļ��в�Ʒ�汾��ע���Ʒʱָ���Ĳ�Ʒ�汾 */
#define ALM_CONST_PRD_VER_MAX 30

/* License���õ����Ƴ��� */
#define ALM_CONST_LIC_NAME_MAX 64

/* License���õ�������Ϣ�ĳ��� */
#define ALM_CONST_LIC_DESC_MAX 256

/* License���õ����ٵĳ��� */
#define ALM_CONST_LIC_UNIT_MAX 64

/* License�ļ�������� */
#define ALM_CONST_LIC_LSN 17
#define ALM_CONST_LIC_TYPE_MAX 16

#define ALM_CONST_LIC_FILE_LEN_MAX (2 * 1024 * 1024)
#define ALM_CONST_LIC_ALL_ESN_LEN_MAX 2048
#define ALM_CONST_LIC_ESN_FEAT_FORMAT_LEN_MAX (ALM_CONST_LIC_ALL_ESN_LEN_MAX + 2)
#define ALM_CONST_LIC_SINGLE_ESN_LEN_MAX 64

#define ALM_CONST_LIC_FILE_VER_LEN_MAX 31
#define ALM_CONST_LIC_FILE_TYPE_LEN_MAX 31
#define ALM_CONST_LIC_COPY_RIGHT_MAX 128
#define ALM_CONST_LIC_CREATER_MAX 128
#define ALM_CONST_LIC_ISSUER_MAX 128
#define ALM_CONST_LIC_COUNTRY_MAX 128
#define ALM_CONST_LIC_CUSTOMER_MAX 512
#define ALM_CONST_LIC_OFFICE_MAX 128
#define ALM_CONST_LIC_DOMAIN_MAX 64
#define ALM_CONST_LIC_DATAVERSION_MAX 32

#define ALM_CONST_LIC_OFFERING_NAME_MAX 64
#define ALM_CONST_LIC_OFFERING_VER_MAX 64
#define ALM_CONST_LIC_OFFERING_POOLDEVSERIES_MAX 64

#define ALM_CONST_LIC_NODE_NAME_MAX ALM_CONST_LIC_NAME_MAX

/* Node������󳤶��޸�Ϊ192 */
#define ALM_CONST_LIC_NODEINFO_NODE_MAX 192

#define ALM_CONST_LIC_NODE_DES_MAX 128
#define ALM_CONST_LIC_NODE_LOCKMODEL_MAX 16

#define ALM_CONST_LIC_SOFT_ID_MAX 64

#define ALM_CONST_LIC_BBOMGROUP_NAME_MAX ALM_CONST_NAME_MAX_LEN

#define ALM_CONST_LIC_ATRRIBULT_MAX 128
#define ALM_CONST_LIC_BBOM_NAME_MAX 63
#define ALM_CONST_LIC_GROUP_NAME_MAX ALM_CONST_LIC_NAME_MAX
#define ALM_CONST_LIC_CLASSIFICATIONCODE_MAX 64
#define ALM_CONST_LIC_SBOM_NAME_MAX ALM_CONST_LIC_NAME_MAX
#define ALM_CONST_LIC_SBOM_DESC_CHS_UNIT_MAX ALM_CONST_LIC_UNIT_MAX
#define ALM_CONST_LIC_SBOM_DESC_ENG_UNIT_MAX ALM_CONST_LIC_UNIT_MAX
#define ALM_CONST_LIC_SBOM_DESC_CHS_MAX ALM_CONST_LIC_DESC_MAX
#define ALM_CONST_LIC_SBOM_DESC_ENG_MAX ALM_CONST_LIC_DESC_MAX
#define ALM_CONST_LIC_PACK_NAME_MAX ALM_CONST_LIC_NAME_MAX
#define ALM_CONST_LIC_PACK_DESC_CHS_UNIT_MAX ALM_CONST_LIC_UNIT_MAX
#define ALM_CONST_LIC_PACK_DESC_ENG_UNIT_MAX ALM_CONST_LIC_UNIT_MAX
#define ALM_CONST_LIC_PACK_DESC_CHS_MAX ALM_CONST_LIC_DESC_MAX
#define ALM_CONST_LIC_PACK_DESC_ENG_MAX ALM_CONST_LIC_DESC_MAX

/* License�н�ֹ�����ַ�����Ϊ��ֵ����ʾ���� */
#define ALM_CONST_LIC_DEADLINE_PERM "PERMANENT"

/* ******************************** �������� ********************************* */
/* ��Ʒ����Ļ�ȡ����ESN�ص��õ���ESN�������󳤶� */
#define ALM_CONST_ENV_ESN_MAX_LEN 2048

/* ��Կ��󳤶ȣ���Կ�� + ����ǩ����ð�ŷָ����㷨 + ��Ʒ�� + ��Ʒ�汾 */
#define ALM_CONST_KEY_MAX_LEN (3752 * 2 + 25 + ALM_CONST_PRD_NAME_LEN_MAX + ALM_CONST_PRD_VER_MAX)

/* ���δ�ӡ��־��󳤶ȣ������ᱻ�ض� */
#define ALM_CONST_LOG_MAX_LEN 511

/* ����Ĭ�������������� */
#define ALM_CONST_EMERGENCY_DFLT_COUNT 3

/* ���������������������ֵ */
#define ALM_CONST_EMERGENCY_MAX_COUNT 10

/* �����������������е����� */
#define ALM_CONST_EMERGENCY_DAYS 7

/* STICK��������������������ֵ */
#define ALM_CONST_STICK_MAX_COUNT 2

/* STICKĬ�������������� */
#define ALM_CONST_STICK_DFLT_COUNT 2

/* STICK������Ĭ���������е����� */
#define ALM_CONST_STICK_DFLT_DAYS 30

/* STICK�������������е�������������ֵ */
#define ALM_CONST_STICK_MAX_DAYS 180

/* �־û��ļ��������Ĳ�Ʒ�� */
#define ALM_CONST_PS_PRD_MAX_NUM 10

/* �־û�������Ʒ�����������ٸ�License��¼ */
#define ALM_CONST_PS_PRD_LIC_MAX_NUM 20

/* �־û�������Ʒ��������ٸ�ʧЧlicense��¼ */
#define ALM_CONST_PS_PRD_RVK_LIC_MAX_NUM 20

/* Ĭ��״̬ˢ�µ�����[AM 2:00, AM 3:00) */
#define ALM_CONST_STATE_REFRESH_DFLT_HOUR 2

/* Ĭ��״̬ˢ�¼������λΪ���� */
#define ALM_CONST_STATE_REFRESH_DFLT_INTERVAL 30

/* ******************************** ������ ********************************* */
/* ��������ʼֵ */
#define ALM_ERR_BASE 0x79000000

/* Enum for return codes */
typedef enum {
    /* ���سɹ� */
    ALM_OK = 0,

    /* δ֪���� */
    ALM_ERR_UNKNOW = (ALM_ERR_BASE | 0x01),

    /* ��ȫ�������ش��� */
    ALM_ERR_SECURE_ERROR = (ALM_ERR_BASE | 0x02),

    /* ******************  �����Ϸ�����ش����� ******************* */
    /* ����ָ����AdaptiveLm�汾��LIB��汾��һ�� */
    /* �п���ʹ�õ�AdaptiveLmͷ�ļ���AdaptiveLm��汾��һ�� */
    ALM_ERR_HEAD_AND_LIB_MIS = (ALM_ERR_BASE | 0x10),

    /* ��ָ����� */
    ALM_ERR_NULL_PTR = (ALM_ERR_BASE | 0x11),

    /* ��Ч�Ĳ��� */
    ALM_ERR_INVALID_PARA = (ALM_ERR_BASE | 0x12),

    /* ������� */
    ALM_ERR_DATA_OVERFLOW = (ALM_ERR_BASE | 0x13),

    /* �ַ����ĳ��ȳ������ֵ */
    ALM_ERR_STR_OVERFLOW = (ALM_ERR_BASE | 0x14),

    /* ����ѯ�ṹ�廹û�г�ʼ�� */
    ALM_ERR_QUERY_DATA_NOT_INIT = (ALM_ERR_BASE | 0x15),

    /* *******************  ���亯����ش����� ******************** */
    /* �ڴ治�� */
    ALM_ERR_MEM_NOT_ENOUGH = (ALM_ERR_BASE | 0x20),

    /* û��ע������ػص����� */
    ALM_ERR_MUTEX_FUNCS_NOT_REG = (ALM_ERR_BASE | 0x21),

    /* û��ע�ᶨʱ����ػص����� */
    ALM_ERR_TIMER_FUNCS_NOT_REG = (ALM_ERR_BASE | 0x22),

    /* û��ע���ȡʱ��ص����� */
    ALM_ERR_GET_TIME_FUNC_NOT_REG = (ALM_ERR_BASE | 0x23),

    /* û��ע���ȡESN�ص����� */
    ALM_ERR_GET_ESN_FUNC_NOT_REG = (ALM_ERR_BASE | 0x24),

    /* û��ע���д�ص����� */
    ALM_ERR_RW_FUNC_NOT_REG = (ALM_ERR_BASE | 0x25),

    /* û��ע���ڴ�����ͷŻص����� */
    ALM_ERR_MEM_FUNCS_NOT_REG = (ALM_ERR_BASE | 0x26),

    /* ���ô������ص�����ʧ�� */
    ALM_ERR_OS_MUTEX_CREATE_FAILED = (ALM_ERR_BASE | 0x27),

    /* �����ͷ����ص�����ʧ�� */
    ALM_ERR_OS_MUTEX_RELEASE_FAILED = (ALM_ERR_BASE | 0x28),

    /* ����ɾ�����ص�����ʧ�� */
    ALM_ERR_OS_MUTEX_DEL_FAILED = (ALM_ERR_BASE | 0x29),

    /* ���û�ȡ���ص�����ʧ�� */
    ALM_ERR_OS_MUTEX_AQUIRE_FAILED = (ALM_ERR_BASE | 0x2A),

    /* ���û�ȡʱ�亯��ʧ�� */
    ALM_ERR_OS_GET_TIME_FAILED = (ALM_ERR_BASE | 0x2B),

    /* ����������ʱ���ص�ʧ�� */
    ALM_ERR_OS_TIMER_START_FAILED = (ALM_ERR_BASE | 0x2C),

    /* ����ֹͣ��ʱ���ص�ʧ�� */
    ALM_ERR_OS_TIMER_STOP_FAILED = (ALM_ERR_BASE | 0x2D),

    /* �����ڴ�ʧ�� */
    ALM_ERR_OS_MALLOC_FAILED = (ALM_ERR_BASE | 0x2E),

    /* ����д�����ļ��ص�ʧ�� */
    ALM_ERR_ADAPTER_PS_WRITE_FAILED = (ALM_ERR_BASE | 0x2F),

    /* ���ö������ļ��ص�ʧ�� */
    ALM_ERR_ADAPTER_PS_READ_FAILED = (ALM_ERR_BASE | 0x30),

    /* ����notify �ص�ʧ�� */
    ALM_ERR_ADAPTER_NOTIFY_FAILED = (ALM_ERR_BASE | 0x31),

    /* ����alarm �ص�ʧ�� */
    ALM_ERR_ADAPTER_ALARM_FAILED = (ALM_ERR_BASE | 0x32),

    /* ����ps c01 read �ص�ʧ�� */
    ALM_ERR_ADAPTER_PS_C01_READ_FAILED = (ALM_ERR_BASE | 0x33),

    /* ���� c01 get prds �ص�ʧ�� */
    ALM_ERR_ADAPTER_PS_C01_GET_PRDS_FAILED = (ALM_ERR_BASE | 0x34),

    /* ����ps c01 Write �ص�ʧ�� */
    ALM_ERR_ADAPTER_PS_C01_WRITE_FAILED = (ALM_ERR_BASE | 0x35),

    /* ע���д�Ͱ汾�����ļ���ػص����� */
    ALM_ERR_OP_OLD_VER_PS = (ALM_ERR_BASE | 0x36),

    /* ���û�ȡKeyFile�Ļص�ʧ�� */
    ALM_ERR_ADAPTER_PUBKEY_READ_FAILED = (ALM_ERR_BASE | 0x37),

    /* *******************  ��Ʒ������ش����� ******************** */
    /* ��û������ */
    ALM_ERR_NOT_START = (ALM_ERR_BASE | 0x40),

    /* �Ѿ�������� */
    ALM_ERR_HAVE_START = (ALM_ERR_BASE | 0x41),

    /* ������ָ����Ʒ */
    ALM_ERR_PRD_NOT_EXIST = (ALM_ERR_BASE | 0x42),

    /* ��Ʒ�Ѿ����� */
    ALM_ERR_PRD_EXIST = (ALM_ERR_BASE | 0x43),

    /* ��ǰ��Ʒ���Ѿ������־û����õ������Ŀ */
    ALM_ERR_PRD_OVER_PS_MAX_NUM = (ALM_ERR_BASE | 0x44),

    /* ��Ч�Ĳ�ƷBBOM ֵ */
    ALM_ERR_PRD_BBOM_VALUE_INVALID = (ALM_ERR_BASE | 0x45),

    /* û��ָ��BBOM */
    ALM_ERR_PRD_BBOM_IS_NULL = (ALM_ERR_BASE | 0x46),

    /* BBOM������ */
    ALM_ERR_PRD_BBOM_NAME_EXCEED_MAX_LEN = (ALM_ERR_BASE | 0x47),

    /* BBOM����ظ� */
    ALM_ERR_PRD_BBOM_SAME_ID = (ALM_ERR_BASE | 0x48),

    /* BBOM���ظ� */
    ALM_ERR_PRD_BBOM_SAME_NAME = (ALM_ERR_BASE | 0x49),

    /* BBOM������Ч */
    ALM_ERR_PRD_BBOM_TYPE_INVALID = (ALM_ERR_BASE | 0x4A),

    /* ��Ч����Կ */
    ALM_ERR_INVALID_KEY = (ALM_ERR_BASE | 0x4B),

    /* ��Ʒ��������󳤶� */
    ALM_ERR_PRD_NAME_EXCEED_MAX_LEN = (ALM_ERR_BASE | 0x4C),

    /* ��Ʒ�汾�ų�����󳤶� */
    ALM_ERR_PRD_VER_EXCEED_MAX_LEN = (ALM_ERR_BASE | 0x4D),

    /* ��Ʒ��ǰ����disable״̬ */
    ALM_ERR_PRD_DISABLED = (ALM_ERR_BASE | 0x4E),

    /* ��Ʒ��ǰ����Not Disable ״̬ */
    ALM_ERR_PRD_NOT_DISABLED = (ALM_ERR_BASE | 0x4F),

    /* *******************  License������ش����� ******************** */
    /* License �ļ���ʽ���� */
    ALM_ERR_LIC_FORMAT = (ALM_ERR_BASE | 0x60),

    /* License �ļ�����Ϊ0���ļ��ַ���Ϊ�� */
    ALM_ERR_LIC_IS_NULL = (ALM_ERR_BASE | 0x61),

    /* У��License�ļ�ǩ��ʧ�� */
    ALM_ERR_LIC_CHECK_SIGN = (ALM_ERR_BASE | 0x62),

    /* �Ѿ�������License�ļ� */
    ALM_ERR_LIC_EXIST = (ALM_ERR_BASE | 0x63),

    /* License�Ѿ����ڻ�ESN����Ʒ�����汾��ʧЧ��ԭ���� */
    ALM_ERR_LIC_INVALID = (ALM_ERR_BASE | 0x64),

    /* һ������ͬһ��Ʒ�ϼ�����License */
    ALM_ERR_LIC_ACTIVED_MORE_THAN_ONE_AT_A_TIME = (ALM_ERR_BASE | 0x65),

    /* ������ָ����License�ļ� */
    ALM_ERR_LIC_NOT_EXIST = (ALM_ERR_BASE | 0x66),

    /* License�ļ��в�����ָ����Ʒ��ע���BBOM */
    ALM_ERR_LIC_NO_BBOM_REGISTED_TO_PRD = (ALM_ERR_BASE | 0x67),

    /* ESN���ȳ��� */
    ALM_ERR_LIC_ESN_EXCEED_MAX_LEN = (ALM_ERR_BASE | 0x68),

    /* ��Ʒ��֧����ʱ��ʧЧ���� */
    ALM_ERR_FIX_TIME_NOT_SUPPORT = (ALM_ERR_BASE | 0x69),

    /* ����ʱ��ʧЧ���ܵ�
     * license���������õĴ�����ʱ�������license
     */
    ALM_ERR_FIX_TIME_LK_NOT_MATCH = (ALM_ERR_BASE | 0x6A),

    /* License�ļ����ȳ��� */
    ALM_ERR_LIC_EXCEED_MAX_LEN = (ALM_ERR_BASE | 0x6B),

    /* License�ļ������ڶ�ӦSpartName ���� BpartName */
    ALM_ERR_LIC_S2B_BPART_SPART_NOT_EXIST = (ALM_ERR_BASE | 0x6C),

    /* License SBOM ��spart valueȫΪ0(S2B ����Ч��Ȩ����Ϊ0) */
    ALM_ERR_LIC_S2B_SBOM_VALUE_INVALID = (ALM_ERR_BASE | 0x6D),

    /* FIX TIME ʧЧ�����Ƿ� */
    ALM_ERR_FIX_TIME_GROUP_NAME_INVALID = (ALM_ERR_BASE | 0x6E),

    /* Service�β�֧�� FIX TIME ���� */
    ALM_ERR_FIX_TIME_SERVICE_NOT_SUPPORT = (ALM_ERR_BASE | 0X70),

    /* *******************  ���Թ�����ش����� ******************** */
    /* �Ѿ�������stick */
    ALM_ERR_LIC_STICK_EXIST = (ALM_ERR_BASE | 0x80),

    /* û������stick */
    ALM_ERR_LIC_STICK_NOT_EXIST = (ALM_ERR_BASE | 0x81),

    /* ��û������stick */
    ALM_ERR_LIC_STICK_NOT_RUNNING = (ALM_ERR_BASE | 0x82),

    /* stick�������� */
    ALM_ERR_LIC_STICK_IS_RUNNING = (ALM_ERR_BASE | 0x83),

    /* �Ѿ������˽��� */
    ALM_ERR_LIC_EMERGENCY_EXIST = (ALM_ERR_BASE | 0x84),

    /* û�����ý���license */
    ALM_ERR_LIC_EMERGENCY_NOT_EXIST = (ALM_ERR_BASE | 0x85),

    /* ��û���������� */
    ALM_ERR_LIC_EMERGENCY_NOT_RUNNING = (ALM_ERR_BASE | 0x86),

    /* ������������ */
    ALM_ERR_LIC_EMERGENCY_IS_RUNNING = (ALM_ERR_BASE | 0x87),

    /* ������stick�����������ѱ��Ĺ� */
    ALM_ERR_LIC_NO_USE_COUNT = (ALM_ERR_BASE | 0x88),

    /* ��ͨ��ʱ�䰲ȫ��� */
    ALM_ERR_SAFETIME_CHK_NOT_PASS = (ALM_ERR_BASE | 0x89),

    /* �����־û�����ʧ�� */
    ALM_ERR_PS_INVALID = (ALM_ERR_BASE | 0x8A),

    /* ��Ʒ�ڵ�ǰ״̬�²�����ʧЧ */
    ALM_ERR_REVOKE_NOT_ALLOWED = (ALM_ERR_BASE | 0x8B),

    /* ��Ʒ�ڽ���״̬�²�����ʧЧ */
    ALM_ERR_EM_REVOKE_NOT_ALLOWED = (ALM_ERR_BASE | 0x8C),

    /* û������BBOM֧�ֵ�ǰ����״̬ */
    ALM_ERR_LIC_EMERGENCY_NO_ITEM_IN_EME = (ALM_ERR_BASE | 0x8D),

    /* **************************״̬���****************************** */
    /* ���������У��������ù��� */
    ALM_ERR_STATE_NOT_MATCH_VERIFY_RULE = (ALM_ERR_BASE | 0x90),

    /* ��֧�ֵ��㷨���� */
    ALM_ERR_ALG_NOT_SUPPORT = (ALM_ERR_BASE | 0x91),

    /* Lic�汾�Ͳ�Ʒ�汾ƥ��ʱ, Lic��֤ͨ�����㷨�����㷨, �û����ò�������������LIC */
    ALM_ERR_NOT_ALLOW_VERMAT_WEAKALG_LIC = (ALM_ERR_BASE | 0x92),

    /* Unknown */
    ALM_ERROR_BUTT = (ALM_ERR_BASE | 0xFF)
} ALM_RET_ENUM;

#if (defined(ALM_PACK_4) || defined(ALM_PACK_8))
#pragma pack()
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#endif /* ALM_INTF_CONSTANT_H */
