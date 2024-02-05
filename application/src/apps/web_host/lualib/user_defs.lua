return {
  -- ************内部错误码定义************* */
  UUSER_USERNAME_INVALID = -121,
  UUSER_NO_PERMISSION = -180,
  UUSER_USERNAME_TOO_LONG = -181,
  UUSER_USERNAME_EMPTY = -181,
  UUSER_USERPASS_TOO_LONG = -182,
  UUSER_USERPASS_EMPTY = -183,
  UUSER_LINUX_DEFAULT_USER = -184,
  UUSER_PASS_COMPLEXITY_FAIL = -186, -- 口令复杂度检测

  -- **********IPMI接口返回码*************
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
  USER_DELETE_EXCLUDE_USER_UNSURPPORT = 0xD9 -- 用户被设置为逃生用户，不能删除
}
