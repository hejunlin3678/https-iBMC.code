
export const errorMessage = {
    'ServiceException': {
        'errorMessage': '服务响应超时，请重新登录'
    },
    'InvalidSession': {
        'errorMessage': '无效的会话，请重新登录'
    },
    'SessionTimeout': {
        'errorMessage': '会话已超时，请重新登录'
    },
    'NoValidSession': {
        'errorMessage': '会话已超时，请重新登录'
    },
    'SessionKickout': {
        'errorMessage': '您已被系统管理员强制登出'
    },
    'SessionRelogin': {
        'errorMessage': '您的账号已在其他地方登录'
    },
    'SessionChanged': {
        'errorMessage': '用户信息被修改，请重新登录'
    },
    'SessionLimitExceeded': {
        'errorMessage': '已达最大会话数'
    },
    'InsufficientPrivilege': {
        'errorMessage': '权限不足。'
    },
    'PropertyModificationNeedPrivilege': {
        'errorMessage': '权限不足。'
    },
    'AuthorizationFailed': {
        'attrMessage': '当前用户密码错误或账户已锁定。'
    },
    'AuthorizationUserNoAccess': {
        'errorMessage': '用户无权限'
    },
    'AuthorizationUserPasswordExpired': {
        'errorMessage': '密码已过期'
    },
    'AuthorizationUserRestricted': {
        'errorMessage': '用户受限'
    },
    'AuthorizationUserLocked': {
        'errorMessage': '当前账户已锁定'
    },
    'InvalidUserName': {
        'errorMessage': '操作失败',
        'attrMessage': '当前用户密码错误或账户已锁定。'
    },
    'ResourceAlreadyExists': {
        'errorMessage': '操作失败',
        'attrMessage': '用户已存在。'
    },
    'UserNameIsRestricted': {
        'errorMessage': '操作失败',
        'attrMessage': '用户名受限。'
    },
    'InvalidPasswordLength': {
        'errorMessage': '操作失败',
        'attrMessage': '无效密码。'
    },
    'InvalidPassword': {
        'errorMessage': '操作失败',
        'attrMessage': '无效密码。'
    },
    'PasswordComplexityCheckFail': {
        'errorMessage': '操作失败',
        'attrMessage': '不符合密码复杂度规则。'
    },
    'PasswordInWeakPWDDict': {
        'errorMessage': '操作失败',
        'attrMessage': '不能在弱口令字典中。'
    },
    'UserIsLoggingIn': {
        'errorMessage': '操作失败',
        'attrMessage': '用户已登录，不能修改。'
    },
    'PasswordNotSatisfyComplexity': {
        'errorMessage': '操作失败',
        'attrMessage': '新旧口令至少在两个字符位上不同。'
    },
    'PropertyValueExceedsMaxLength': {
        'errorMessage': '操作失败',
        'attrMessage': '无效密码。'
    },
    'DuringMinimumPasswordAge': {
        'errorMessage': '操作失败',
        'attrMessage': '该用户密码处于密码最短使用期保护中，不能修改。'
    },
    'EmergencyLoginUser': {
        'errorMessage': '操作失败',
        'attrMessage': '紧急登录用户，不能修改。'
    },
    'CannotDisableLastAdministrator': {
        'errorMessage': '操作失败',
        'attrMessage': '最后一个管理员账户无法禁用。'
    },
    'CannotDeleteLastAdministrator': {
        'errorMessage': '操作失败',
        'attrMessage': '最后一个管理员账户无法删除。'
    },
    'InvalidPasswordSameWithHistory': {
        'errorMessage': '操作失败',
        'attrMessage': '不能使用限制范围内的历史密码。'
    },
    'AccountForbidRemoved': {
        'errorMessage': '紧急登录用户或者SNMPtrapv3用户，不能删除。'
    },
    'PublicKeyNotExist': {
        'errorMessage': '公钥文件不存在。'
    },
    'PublicKeyImportError': {
        'errorMessage': '公钥格式错误'
    },
    'DumpExportingErr': {
        'errorMessage': '其他用户正在操作，请稍等'
    },
    'DifferentDriveSectorsSize': {
        'errorMessage': '所选物理盘扇区大小不相同。'
    },
    'InsufficientMemoryErr': {
        'errorMessage': '可用内存不足，收集失败。'
    },
    'UpgradingErr': {
        'errorMessage': '正在升级，请稍后再试。'
    },
    'DisableScreenShotInCurrentState': {
        'errorMessage': 'KVM独占模式下不支持手动截屏。'
    },
    'ActivePsNotNecessary': {
        'errorMessage': '不需要设置主用电源。'
    },
    'ActivePsNecessary': {
        'errorMessage': '请选择主用电源。'
    },
    'ActivePsNotPosition': {
        'errorMessage': '至少有一个主用电源不在位。'
    },
    'NoStandbyPsPosition': {
        'errorMessage': '没有一个备用电源在位。'
    },
    'ActivePsNotHealth': {
        'errorMessage': '主用或备用电源不健康。'
    },
    'CurrentPowerHigh': {
        'errorMessage': '当前功率超过所有主用电源额定功率值的75%。'
    },
    'ActivePsNotEnough': {
        'errorMessage': '主用电源的数量不够。'
    },
    'PsNotSupport': {
        'errorMessage': '电源不支持主备供电。'
    },
    'ActiveStandbyModeUnsupport': {
        'errorMessage': '不支持的主备模式。'
    },
    'DuplicateExportingErr': {
        'errorMessage': '正在下载，请稍后再试。'
    },
    'OperationInProcess': {
        'errorMessage': '已有日志收集任务正在进行，请稍后再试。'
    },
    'ResetOperationFailed': {
        'errorMessage': '正在升级BIOS，请稍后再试。'
    },
    'ResetOperationFailedVRD': {
        'errorMessage': '正在升级VRD，请稍后再试。'
    },
    'FileNotExist': {
        'errorMessage': '录像文件不存在。'
    },
    'RecordExceedsMaxNum': {
        'errorMessage': '工作记录已达最大条数。'
    },
    'InvalidRecordId': {
        'errorMessage': '工作记录不存在。'
    },
    'CertificateImportingErr': {
        'errorMessage': '正在上传，请稍后再试。'
    },
    'KeyImportError': {
        'errorMessage': '密钥格式错误。'
    },
    'FailedEnableTwoFactorCertification': {
        'errorMessage': '请至少为一个用户配置客户端证书和根证书。'
    },
    'CertificateQtyReachLimit': {
        'errorMessage': '证书达到上限。'
    },
    'RootCertificateAlreadyExists': {
        'errorMessage': '证书文件已存在。'
    },
    'RootCertificateNotExist': {
        'errorMessage': '证书不存在。'
    },
    'ClientCertLackProperties': {
        'errorMessage': '证书用途不匹配。'
    },
    'ClientCertificateAlreadyExists': {
        'errorMessage': '证书文件已存在。'
    },
    'ClientCertIssuerMismatch': {
        'errorMessage': '未找到根证书。'
    },
    'ClientCertQtyExceedLimit': {
        'errorMessage': '证书达到上限。'
    },
    'ClientCertificateNotExist': {
        'errorMessage': '证书不存在。'
    },
    'CertificateFormatError': {
        'errorMessage': '证书格式错误。'
    },
    'RootCertLackProperties': {
        'errorMessage': '证书用途不匹配。'
    },
    'RootCertificateMismatch': {
        'errorMessage': 'CRL与根证书不匹配。'
    },
    'RootCertificateNotImported': {
        'errorMessage': 'CRL与根证书不匹配。'
    },
    'CrlFileFormatError': {
        'errorMessage': '不是有效的CRL文件或非Base64编码的文件。'
    },
    'RootCertificateIdInvalid': {
        'errorMessage': '无效的根证书id。'
    },
    'MinPwdAgeAndPwdValidityRestrictEachOther': {
        'errorMessage': '操作失败',
        'attrMessage': '密码最短使用期必须比密码有效期小10天以上。'
    },
    'DisableKVMEncryptionFailed': {
        'errorMessage': '关闭KVM加密失败，需要先关闭VMM加密。'
    },
    'KvmInUse': {
        'errorMessage': 'KVM正在使用中,请稍后再试。'
    },
    'CommunityNameInWeakPWDDict': {
        'errorMessage': '不能在弱口令字典中。'
    },
    'CommunityNameNotContainSpace': {
        'errorMessage': '不符合密码复杂度规则。'
    },
    'SimpleCommunityName': {
        'errorMessage': '不符合密码复杂度规则。'
    },
    'SameCommunityName': {
        'errorMessage': '只读团体名和读写团体名相同。'
    },
    'EnableVMMEncryptionFailed': {
        'errorMessage': '开启VMM加密失败，需要先开启KVM加密。'
    },
    'VncInUse': {
        'errorMessage': 'VNC正在使用中，请稍后再试。'
    },
    'CSRGeneratingErr': {
        'errorMessage': '正在生成，请稍后再试。'
    },
    'CSRExportingErr': {
        'errorMessage': '正在生成，请稍后再试。'
    },
    'CertMismatch': {
        'errorMessage': '证书校验失败，导入证书失败。'
    },
    'V3UserNameNotUsed': {
        'errorMessage': '操作失败'
    },
    'NotSupportZhAndEnUninstalled': {
        'errorMessage': '不支持删除中英文语言包。'
    },
    'OnlyOneLanguageInstalled': {
        'errorMessage': '当前只支持一种语言，无法删除。'
    },
    'LanguageNotInstalled': {
        'errorMessage': '语言已被卸载，请刷新当前页面。'
    },
    'LicenseInstalling': {
        'errorMessage': '正在安装，请稍后再试。'
    },
    'CertImportFailed': {
        'errorMessage': '导入证书失败。'
    },
    'LicenseVerifyFailed': {
        'errorMessage': '许可证校验错误。'
    },
    'LicenseActivateFailed': {
        'errorMessage': '许可证激活失败。'
    },
    'LicenseExporting': {
        'errorMessage': '正在导出，请稍后再试。'
    },
    'LicenseNotInstall': {
        'errorMessage': '未安装许可证。'
    },
    'PortIdModificationFailed': {
        'errorMessage': '端口已被占用'
    },
    'PropertyUnknown': {
        'errorMessage': '操作失败'
    },
    'MalformedJSON': {
        'errorMessage': '操作失败'
    },
    'IncorrectSenderInfo': {
        'errorMessage': '用户名、密码或发件人地址不正确。'
    },
    'SmtpConnectionFailed': {
        'errorMessage': '无法连接到SMTP服务器或该服务器不支持匿名登录。'
    },
    'FeatureDisabledAndNotSupportOperation': {
        'errorMessage': '当前状态不支持该操作。'
    },
    'TrapMemberIDNotExist': {
        'errorMessage': 'Trap测试失败，因为已禁用trap服务或trap服务器已关闭或服务器地址为空。'
    },
    'SyslogTestFailed': {
        'errorMessage': '系统日志测试失败，因为已禁用系统日志服务或已禁用服务器或服务器地址为空。'
    },
    'SendingEmailFailed': {
        'errorMessage': '无法发送电子邮件或收件人的地址不正确。'
    },
    'TrapSendTestErr': {
        'errorMessage': 'Trap测试失败，因为已禁用trap服务或trap服务器已关闭或服务器地址为空。'
    },
    'CurrentStatusNotSupportOperation': {
        'errorMessage': '操作失败'
    },
    'RAIDControllerNotSupported': {
        'errorMessage': '操作失败'
    },
    'PropertyNotSupportedModificationByCurrentStatus': {
        'errorMessage': '操作失败'
    },
    'LocatedFailedByPDRebuilding': {
        'errorMessage': '操作失败'
    },
    'PropertyModificationNotSupportedByPDStatus': {
        'errorMessage': '操作失败'
    },
    'PropertyModificationFailedByRAIDlevelNotSupported': {
        'errorMessage': '操作失败'
    },
    'PropertyModificationFailedByIterfaceConflict': {
        'errorMessage': '操作失败'
    },
    'PropertyModificationFailedByMediaTypeConflict': {
        'errorMessage': '操作失败，介质类型冲突'
    },
    'PropertyConflict': {
        'errorMessage': '操作失败'
    },
    'ModifyfailedWithRAIDCardState': {
        'errorMessage': 'RAID卡管理未准备就绪'
    },
    'ModifyfailedWithCachecade': {
        'errorMessage': '操作失败'
    },
    'ModifyfailedWithRaidControllerNotSupport': {
        'errorMessage': '操作失败'
    },
    'ModifyfailedWithSSD': {
        'errorMessage': '操作失败'
    },
    'InvalidVolumeName': {
        'errorMessage': '操作失败'
    },
    'InternalError': {
        'errorMessage': '操作失败'
    },
    'CurrentStatusNotSupport': {
        'errorMessage': '当前状态下不支持此操作。'
    },
    'OperationNotSupported': {
        'errorMessage': '不支持此操作。'
    },
    'StripeSizeError': {
        'errorMessage': '条带大小不支持。'
    },
    'VolumeCapacityOutRange': {
        'errorMessage': '逻辑盘容量超出范围。'
    },
    'ArraySpaceInsufficient': {
        'errorMessage': 'Array空间不足。'
    },
    'ArraySpaceNoFree': {
        'errorMessage': 'Array没有可用空间。'
    },
    'NumberofDrivesForEachSpanInvalid': {
        'errorMessage': 'Span个数无效。'
    },
    'NumberOfSpansInvalid': {
        'errorMessage': '物理盘数不满足要求。'
    },
    'StorageMediaInconsistent': {
        'errorMessage': '物理盘介质类型不同。'
    },
    'DriveInterfacesInconsistent': {
        'errorMessage': '物理盘接口类型不同。'
    },
    'DriveStatusNotSupported': {
        'errorMessage': '当前物理盘的状态不支持正要执行的操作。'
    },
    'DriveListDuplicateValue': {
        'errorMessage': '物理盘列表重复。'
    },
    'RaidControllerLevelInvalid': {
        'errorMessage': 'Raid级别不支持。'
    },
    'RAIDControllerNotSupportedOperation': {
        'errorMessage': 'Raid卡不支持当前操作。'
    },
    'RAIDReachedVolumesNumberLimit': {
        'errorMessage': '逻辑盘个数已达上限。'
    },
    'SSDNotExist': {
        'errorMessage': '物理盘不是SSD盘。'
    },
    'PhysicalDiskIdCountError': {
        'errorMessage': '物理盘个数错误。'
    },
    'PhysicalDiskIdInvalid': {
        'errorMessage': '物理盘ID无效。'
    },
    'CryptoEraseNotSupported': {
        'errorMessage': '控制器或物理盘不支持该操作。'
    },
    'IncompleteForeignConfig': {
        'errorMessage': 'Foreign配置信息不完整。'
    },
    'NoForeignConfig': {
        'errorMessage': 'Foreign配置信息不存在。'
    },
    'ResourceMissingAtURI': {
        'errorMessage': '操作失败。'
    },
    'PropertyValueFormatError': {
        'errorMessage': '操作失败。'
    },
    'FirmwareUploadError': {
        'errorMessage': '固件上传失败，请稍后重试。'
    },
    'FirmwareUpgrading': {
        'errorMessage': '系统正在升级中。'
    },
    'FirmwareNotMatchSignFile': {
        'errorMessage': '升级文件与签名文件的名称不匹配。'
    },
    'UploadFileOutRange': {
        'errorMessage': '文件大小超出限制。'
    },
    'EmergencyLoginUserSettingFail': {
        'errorMessage': '紧急用户必须为有效的管理员账户。'
    },
    'ConflictWithLinuxDefaultUser': {
        'errorMessage': '操作失败',
        'attrMessage': '设置用户名失败。'
    },
    'SpecifiedFileExist': {
        'errorMessage': '已存在同名文件，请修改文件名后重新上传。'
    },
    'ActionFailedByBbuExist': {
        'errorMessage': '设备已启用备电功能，强制重启操作无效。'
    },
    'ResetOperationNotAllowed': {
        'errorMessage': '操作失败，当前状态不支持此操作，请稍后重试。'
    },
    'TrapV3loginUser': {
        'errorMessage': '操作失败',
        'attrMessage': 'Trap使能下，TrapV3用户无法修改用户名。'
    },
    'CertificateExpired': {
        'errorMessage': '证书已过期。'
    },
    'SameRWCommunityName': {
        'errorMessage': '设置的读写团体名不能与只读团体名相同。'
    },
    'SameROCommunityName': {
        'errorMessage': '设置的只读团体名不能与读写团体名相同。'
    },
    'RWCommunitySimilarWithHistory': {
        'errorMessage': '读写团体名新旧口令至少在两个字符位上不同。'
    },
    'ROCommunitySimilarWithHistory': {
        'errorMessage': '只读团体名新旧口令至少在两个字符位上不同。'
    },
    'TwoFactorCertificationFailedWithTLS': {
        'errorMessage': '操作失败，请确认已在安全配置中开启TLS 1.2。'
    },
    'UserLocked': {
        'errorMessage': '当前账户已锁定'
    },
    'IPv4AddressConflict': {
        'errorMessage': 'IPv4地址网段与其他网络冲突。'
    },
    'SystemLockdownForbid': {
        'errorMessage': '系统锁定已开启，修改密码失败，请联系管理员处理。'
    },
    'TLSFailedWithTwoFactorCertification': {
        'errorMessage': '双因素认证已开启，当前状态不支持关闭TLS 1.2。'
    },
    'CrlIssuingDateInvalid': {
        'errorMessage': '导入失败，已存在较新发证日期的CRL。'
    },
    'LoginUserLocked': {
        'errorMessage': '账户已被锁定，请联系管理员。'
    },
    'LDInitInProgress': {
        'errorMessage': '操作失败，逻辑盘正在初始化。'
    },
    'NotSupportZhAndEnDisabled': {
        'errorMessage': '不支持关闭中英文语言。'
    },
    'OnlyOneLanguageEnabled': {
        'errorMessage': '只开启一种语言的情况下，不支持关闭该语言。'
    },
    'LanguageNotEnabled': {
        'errorMessage': '语言已被关闭，请刷新当前页面。'
    },
    'LanguageNotSupport': {
        'errorMessage': '该语言不支持当前操作。'
    },
    'CANotSupportCrlSignature': {
        'errorMessage': '根证书不支持CRL签名。'
    },
    'LanguageNotDisabled': {
        'errorMessage': '语言已被开启，请刷新当前页面。'
    },
    'KeyFileTypeNotSupported': {
        'errorMessage': '不支持该加密算法类型的密钥文件导入。'
    },
    'BodyExceedsMaxLength': {
        'errorMessage': '资产信息内容过长，建议控制在300字符以内。'
    },
    'EncryptedCertImportFailed': {
        'errorMessage': '导入加密证书失败。'
    },
    'EncryptionAlgorithmIsWeak': {
        'errorMessage': '不支持弱签名算法(MD5)证书，请重新生成证书。'
    },
    'EncryptionAlgorithmSHA1IsWeak': {
        'errorMessage': '不支持弱签名算法(SHA1)证书，请重新生成证书。'
    },
    'ConfigurationExist': {
        'errorMessage': '操作失败，存在逻辑盘或增强型物理盘。'
    },
    'NoRestorePoint': {
        'errorMessage': '还原点不存在。'
    },
    'CertificateAlreadyExists': {
        'errorMessage': '证书文件重复。'
    },
    'FileNameError': {
        'errorMessage': '文件名错误。'
    },
    'NonSSDExist': {
        'errorMessage': '操作失败，成员盘中存在非SSD硬盘。'
    },
    'PropertyModificationNotSupportedByBBUStatus': {
        'errorMessage': '操作失败，请检查BBU是否在位并且已充电完成。'
    },
    'VolumeShrinkNotAllowed': {
        'errorMessage': '操作失败，不允许收缩逻辑盘的容量。'
    },
    'AssociatedVolumeCapacityOutRange': {
        'errorMessage': '操作失败，关联逻辑盘的容量超过范围。'
    },
    'OperationFailed': {
        'errorMessage': '当前状态不支持该操作，请稍后再试。'
    },
    'ModifyfailedWithRCPNotInRange': {
        'errorMessage': '操作失败，请确保读缓存百分比在有效范围。'
    }
};
