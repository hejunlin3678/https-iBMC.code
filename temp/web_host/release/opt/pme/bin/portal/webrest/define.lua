return {
    -- Redfish会话更新标识
    REFRESH_ENABLE = 1,
    REFRESH_DISABLE = 2,
    -- Redfish会话通道类型
    REDFISH_INNER_HMM = 0,
    REDFISH_INNER_BMA = 1,
    REDFISH_INNER_MAX = 2,
    REDFISH_WEBUI = 0x10,
    REDFISH_UNKNOWN = 0xFF,
    -- 登录类型
    LOG_TYPE_LOCAL = 0,
    LOG_TYPE_LDAP_AUTO_MATCH = 1, -- 自动匹配认证
    LOG_TYPE_LDAP_SERVER1 = 2,
    LOG_TYPE_LDAP_SERVER2 = 3,
    LOG_TYPE_LDAP_SERVER3 = 4,
    LOG_TYPE_LDAP_SERVER4 = 5,
    LOG_TYPE_LDAP_SERVER5 = 6,
    LOG_TYPE_LDAP_SERVER6 = 7,
    LOG_TYPE_LDAP_KRB_SSO = 12,
    LOG_TYPE_KRB_SERVER = 13,
    LOG_TYPE_SSO_ = 14,
    LOG_TYPE_SSO_LDAP = 15,
    LOG_TYPE_AUTO_MATCH = 0xfe, -- 校验范围包括本地用户和LDAP用户
    -- 用户名类型
    USER_NAME_CONTAIN_DOMAIN = 0,
    USER_NAME_NOT_CONTAIN_DOMAIN = 1,
    USER_NAME_NEED_POST_BACK = 2,
    -- 内部错误码定义
    UUSER_USERNAME_INVALID = -121,
    UUSER_NO_PERMISSION = -180,
    UUSER_USERNAME_TOO_LONG = -181,
    UUSER_USERNAME_EMPTY = -181,
    UUSER_USERPASS_TOO_LONG = -182,
    UUSER_USERPASS_EMPTY = -183,
    UUSER_LINUX_DEFAULT_USER = -184,
    UUSER_PASS_COMPLEXITY_FAIL = -186, -- 口令复杂度检测
    -- IPMI接口返回码
    USER_NAME_EXIST = 0x80,
    USER_DONT_EXIST = 0x81,
    INVALID_PASSWORD = 0x82, -- 用户密码错误
    USER_IS_LOCKED = 0x83, -- 用户被锁定
    USER_PASS_COMPLEXITY_FAIL = 0x84, -- 口令复杂度检测
    USER_UNLOCK_FAIL = 0x85,
    PERMIT_RULE_INVALID = 0x86,
    USER_MGNT_OPERATION_DISABLED = 0x87, -- 用户管理操作未使能
    USER_NO_ACCESS = 0x88, -- 用户无权访问
    USER_PASSWORD_EXPIRED = 0x89, -- 口令过期
    USER_LOGIN_LIMITED = 0x90, -- 登录受限
    USER_TIME_LIMIT_UNREASONABLE = 0x91, -- 密码有效期设置短于密码最短使用期限
    USER_PUBLIC_KEY_FORMAT_ERROR = 0x92, -- 公钥格式错误，支持RFC4716，Openssh格式
    USER_CANNT_SET_SAME_PASSWORD = 0x93, -- 开启了历史密码检测，还设置历史密码
    USER_SET_PASSWORD_EMPTY = 0X94,
    USER_SET_PASSWORD_TOO_WEAK = 0X95, -- 密码在弱口令字典
    USER_NODE_BUSY = 0xc0,
    USER_COMMAND_NORMALLY = 0x00,
    USER_REQ_LENGTH_INVALID = 0xC7,
    USER_PARAMETER_OUT_OF_RANGE = 0xC9,
    COMP_CODE_INVALID_FIELD = 0xCC,
    USER_INVALID_DATA_FIELD = 0XCC,
    USER_DATA_LENGTH_INVALID = 0xC7,
    USER_UNSUPPORT = 0xD5,
    USER_DELETE_TRAPV3_USER_UNSUPPORT = 0xD7, -- 用户被设置为SNMP Trap V3的用户，不能删除
    USER_DELETE_EXCLUDE_USER_UNSURPPORT = 0xD9, -- 用户被设置为逃生用户，不能删除
    AUTH_RET_MAP = {
        [0xD5] = 'AuthorizationFailed', -- USER_UNSUPPORT
        [0x82] = 'AuthorizationFailed', -- INVALID_PASSWORD
        [0x81] = 'AuthorizationFailed', -- USER_DONT_EXIST
        [-181] = 'AuthorizationFailed', -- UUSER_USERNAME_TOO_LONG
        [-182] = 'AuthorizationFailed', -- UUSER_USERPASS_TOO_LONG
        [0x88] = 'NoAccess', -- USER_NO_ACCESS
        [0x89] = 'UserPasswordExpired', -- USER_PASSWORD_EXPIRED
        [0x90] = 'UserLoginRestricted', -- USER_LOGIN_LIMITED
        [0x83] = 'UserLocked', -- USER_IS_LOCKED
        [0xDA] = 'AuthorizationFailed' -- USER_LDAP_LOGIN_FAIL
    },
    -- 文件下载类型
    DOWNLOAD_CONFIG = 0x01,
    DOWNLOAD_LICENSE = 0x02,
    DOWNLOAD_DUMP = 0x03,
    DOWNLOAD_SEL = 0x04,
    DOWNLOAD_OPERATE_LOG = 0x05,
    DOWNLOAD_RUN_LOG = 0x06,
    DOWNLOAD_SECURITY_LOG = 0x07,
    DOWNLOAD_BLACK_BOX = 0x08,
    DOWNLOAD_SYSTEM_COM = 0x09,
    DOWNLOAD_CSR = 0x0A,
    DOWNLOAD_POWER_HISTORY = 0x0B,
    DOWNLOAD_VIDEO = 0x0C,
    DOWNLOAD_PICTURE = 0x0D,
    DOWNLOAD_NPU_LOG = 0x0E,
    DOWNLOAD_SDI_BLACK_BOX = 0x0F,
    -- SHA类型
    G_CHECKSUM_SHA256 = 2
}
