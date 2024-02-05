export const errorMessage = {
    'ServiceException': {
        'errorMessage': 'The service timed out. Please log in again.'
    },
    'InvalidSession': {
        'errorMessage': 'Invalid session. Please log in again.'
    },
    'SessionTimeout': {
        'errorMessage': 'Your login session timed out. Please log in again.'
    },
    'NoValidSession': {
        'errorMessage': 'Your login session timed out. Please log in again.'
    },
    'SessionKickout': {
        'errorMessage': 'You have been logged out by the system administrator.'
    },
    'SessionRelogin': {
        'errorMessage': 'Your account has logged in from another device.'
    },
    'SessionChanged': {
        'errorMessage': 'The user information has been changed. Please log in again.'
    },
    'SessionLimitExceeded': {
        'errorMessage': 'The maximum number of sessions has been reached.'
    },
    'InsufficientPrivilege': {
        'errorMessage': 'You are not authorized to perform this operation. '
    },
    'PropertyModificationNeedPrivilege': {
        'errorMessage': 'You are not authorized to perform this operation. '
    },
    'AuthorizationFailed': {
        'attrMessage': 'Incorrect password or locked user account.'
    },
    'AuthorizationUserNoAccess': {
        'attrMessage': 'No permission.'
    },
    'AuthorizationUserPasswordExpired': {
        'attrMessage': 'Password has expired.'
    },
    'AuthorizationUserRestricted': {
        'attrMessage': 'Login restricted.'
    },
    'AuthorizationUserLocked': {
        'attrMessage': 'The account has been locked.'
    },
    'InvalidUserName': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'Incorrect password or locked user account.'
    },
    'ResourceAlreadyExists': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'The user already exists.'
    },
    'UserNameIsRestricted': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'The user name is restricted.'
    },
    'InvalidPasswordLength': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'Invalid Password.'
    },
    'InvalidPassword': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'Invalid Password.'
    },
    'PasswordComplexityCheckFail': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'The password does not meet password complexity requirements.'
    },
    'PasswordInWeakPWDDict': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'Cannot be included in the weak password dictionary.'
    },
    'UserIsLoggingIn': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'The user has logged in and cannot be modified.'
    },
    'PasswordNotSatisfyComplexity': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'Have at least two new characters when compared with the previous password.'
    },
    'PropertyValueExceedsMaxLength': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'Invalid Password.'
    },
    'DuringMinimumPasswordAge': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'The password is in the minimum password age and cannot be changed.'
    },
    'EmergencyLoginUser': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'The emergency login user cannot be changed.'
    },
    'CannotDisableLastAdministrator': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'The last administrator cannot be disabled.'
    },
    'CannotDeleteLastAdministrator': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'The last administrator cannot be deleted.'
    },
    'InvalidPasswordSameWithHistory': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'The historical password cannot be used. '
    },
    'AccountForbidRemoved': {
        'errorMessage': 'The user is an emergency login user or SNMPv3 trap user and cannot be deleted.'
    },
    'PublicKeyNotExist': {
        'errorMessage': 'The public key file does not exist.'
    },
    'PublicKeyImportError': {
        'errorMessage': 'Incorrect public key format'
    },
    'DumpExportingErr': {
        'errorMessage': 'Task in progress... Please wait.'
    },
    'DifferentDriveSectorsSize': {
        'errorMessage': 'The sector sizes of the selected physical drives are different. '
    },
    'InsufficientMemoryErr': {
        'errorMessage': 'Failed to collect data due to insufficient memory.'
    },
    'UpgradingErr': {
        'errorMessage': 'Upgrading … Please try again later.'
    },
    'DisableScreenShotInCurrentState': {
        'errorMessage': 'Manual screenshot capturing is not supported when the KVM is in private mode.'
    },
    'ActivePsNotNecessary': {
        'errorMessage': 'You do not need to set the active PSU.'
    },
    'ActivePsNecessary': {
        'errorMessage': 'Please select the active PSU.'
    },
    'ActivePsNotPosition': {
        'errorMessage': 'At least one active PSU is not detected.'
    },
    'NoStandbyPsPosition': {
        'errorMessage': 'No standby PSU is detected.'
    },
    'ActivePsNotHealth': {
        'errorMessage': 'The active or standby PSU is unhealthy.'
    },
    'CurrentPowerHigh': {
        'errorMessage': 'The current power exceeds 75% of the power ratings of all active PSUs.'
    },
    'ActivePsNotEnough': {
        'errorMessage': 'Active PSUs are insufficient.'
    },
    'PsNotSupport': {
        'errorMessage': 'The PSUs do not support active/standby mode.'
    },
    'ActiveStandbyModeUnsupport': {
        'errorMessage': 'The Active/Standby mode is not supported. '
    },
    'DuplicateExportingErr': {
        'errorMessage': 'Downloading … Please try again later.'
    },
    'OperationInProcess': {
        'errorMessage': 'A log collection task is running. Try again later.'
    },
    'ResetOperationFailed': {
        'errorMessage': 'Upgrading the BIOS …'
    },
    'ResetOperationFailedVRD': {
        'errorMessage': 'Upgrading the VRD …'
    },
    'FileNotExist': {
        'errorMessage': 'The video file does not exist.'
    },
    'RecordExceedsMaxNum': {
        'errorMessage': 'The number of work records has reached the maximum.'
    },
    'InvalidRecordId': {
        'errorMessage': 'The work record does not exist.'
    },
    'CertificateImportingErr': {
        'errorMessage': 'Uploading certificate … Please try again later.'
    },
    'KeyImportError': {
        'errorMessage': 'Incorrect key format.'
    },
    'FailedEnableTwoFactorCertification': {
        'errorMessage': 'Configure client and root certificates for at least one user.'
    },
    'CertificateQtyReachLimit': {
        'errorMessage': 'The number of certificates has reached the maximum.'
    },
    'RootCertificateAlreadyExists': {
        'errorMessage': 'The file already exists.'
    },
    'RootCertificateNotExist': {
        'errorMessage': 'The certificate does not exist.'
    },
    'ClientCertLackProperties': {
        'errorMessage': 'Failed to import the client certificate used for two-factor authentication because the certificate lacks certain properties.'
    },
    'ClientCertificateAlreadyExists': {
        'errorMessage': 'The file already exists.'
    },
    'ClientCertIssuerMismatch': {
        'errorMessage': 'Failed to find the root certificate.'
    },
    'ClientCertQtyExceedLimit': {
        'errorMessage': 'The number of certificates has reached the maximum.'
    },
    'ClientCertificateNotExist': {
        'errorMessage': 'The certificate does not exist.'
    },
    'CertificateFormatError': {
        'errorMessage': 'Incorrect certificate format.'
    },
    'RootCertLackProperties': {
        'errorMessage': 'The certificate is not a root certificate.'
    },
    'RootCertificateMismatch': {
        'errorMessage': 'The CRL does not match the root certificate.'
    },
    'RootCertificateNotImported': {
        'errorMessage': 'The CRL does not match the root certificate.'
    },
    'CrlFileFormatError': {
        'errorMessage': 'Invalid CRL file or non-Base64-encoded file.'
    },
    'RootCertificateIdInvalid': {
        'errorMessage': 'Invalid root certificate ID.'
    },
    'MinPwdAgeAndPwdValidityRestrictEachOther': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'The minimum password age must be 10 days shorter than the password validity period.'
    },
    'DisableKVMEncryptionFailed': {
        'errorMessage': 'Disable VMM encryption before you disable KVM encryption.'
    },
    'KvmInUse': {
        'errorMessage': 'The KVM is in use. Please try again later.'
    },
    'CommunityNameInWeakPWDDict': {
        'errorMessage': 'Cannot be included in the weak password dictionary.'
    },
    'CommunityNameNotContainSpace': {
        'errorMessage': 'The password does not meet password complexity requirements.'
    },
    'SimpleCommunityName': {
        'errorMessage': 'The password does not meet password complexity requirements.'
    },
    'SameCommunityName': {
        'errorMessage': 'The Read-Only Community must not be the same as the Read-Write Community.'
    },
    'EnableVMMEncryptionFailed': {
        'errorMessage': 'Enable KVM encryption before you enable VMM encryption.'
    },
    'VncInUse': {
        'errorMessage': 'The VNC is in use. Please try again later.'
    },
    'CSRGeneratingErr': {
        'errorMessage': 'Generating CSR … Please try again later.'
    },
    'CSRExportingErr': {
        'errorMessage': 'Generating … Please try later.'
    },
    'CertMismatch': {
        'errorMessage': 'Failed to import the certificate because the verification failed.'
    },
    'V3UserNameNotUsed': {
        'errorMessage': 'Operation failed.'
    },
    'NotSupportZhAndEnUninstalled': {
        'errorMessage': 'The Chinese and English language packs cannot be deleted.'
    },
    'OnlyOneLanguageInstalled': {
        'errorMessage': 'This is the only language used by the system and cannot be deleted.'
    },
    'LanguageNotInstalled': {
        'errorMessage': 'The language pack is already uninstalled. Refresh the page for the latest data.'
    },
    'LicenseInstalling': {
        'errorMessage': 'Installing license … Please try again later.'
    },
    'CertImportFailed': {
        'errorMessage': 'Failed to import the certificate.'
    },
    'LicenseVerifyFailed': {
        'errorMessage': 'License verification failed.'
    },
    'LicenseActivateFailed': {
        'errorMessage': 'Failed to activate the license.'
    },
    'LicenseExporting': {
        'errorMessage': 'Exporting license … Please try again later.'
    },
    'LicenseNotInstall': {
        'errorMessage': 'No license is installed.'
    },
    'PortIdModificationFailed': {
        'errorMessage': 'The port is in use.'
    },
    'PropertyUnknown': {
        'errorMessage': 'Operation failed.'
    },
    'MalformedJSON': {
        'errorMessage': 'Operation failed.'
    },
    'IncorrectSenderInfo': {
        'errorMessage': 'Incorrect user name, password, or sender\'s address.'
    },
    'SmtpConnectionFailed': {
        'errorMessage': 'Failed to connect to the SMTP server, or the SMTP server does not support anonymous login.'
    },
    'FeatureDisabledAndNotSupportOperation': {
        'errorMessage': 'The current state does not support this operation.'
    },
    'TrapMemberIDNotExist': {
        'errorMessage': 'The trap test failed because the trap service or trap server is disabled or the trap server address is empty.'
    },
    'SyslogTestFailed': {
        'errorMessage': 'The system log test failed because the log service or log server is disabled or the log server address is empty.'
    },
    'SendingEmailFailed': {
        'errorMessage': 'Failed to send e-mail, or the recipient\'s address is incorrect.'
    },
    'TrapSendTestErr': {
        'errorMessage': 'The trap test failed because the trap service or trap server is disabled or the trap server address is empty.'
    },
    'InternalError': {
        'errorMessage': 'Operation failed.'
    },
    'CurrentStatusNotSupportOperation': {
        'errorMessage': 'Operation failed.'
    },
    'RAIDControllerNotSupported': {
        'errorMessage': 'Operation failed.'
    },
    'PropertyNotSupportedModificationByCurrentStatus': {
        'errorMessage': 'Operation failed.'
    },
    'LocatedFailedByPDRebuilding': {
        'errorMessage': 'Operation failed.'
    },
    'PropertyModificationNotSupportedByPDStatus': {
        'errorMessage': 'Operation failed.'
    },
    'PropertyModificationFailedByRAIDlevelNotSupported': {
        'errorMessage': 'Operation failed.'
    },
    'PropertyModificationFailedByIterfaceConflict': {
        'errorMessage': 'Operation failed.'
    },
    'PropertyModificationFailedByMediaTypeConflict': {
        'errorMessage': 'Operation failed. Type of the media to be added conflicts with the existing media type.'
    },
    'PropertyConflict': {
        'errorMessage': 'Operation failed.'
    },
    'ModifyfailedWithRAIDCardState': {
        'errorMessage': 'RAID controller card management is not ready.'
    },
    'ModifyfailedWithCachecade': {
        'errorMessage': 'Operation failed.'
    },
    'ModifyfailedWithRaidControllerNotSupport': {
        'errorMessage': 'Operation failed.'
    },
    'ModifyfailedWithSSD': {
        'errorMessage': 'Operation failed.'
    },
    'InvalidVolumeName': {
        'errorMessage': 'Operation failed.'
    },
    'CurrentStatusNotSupport': {
        'errorMessage': 'The current status does not support this operation.'
    },
    'OperationNotSupported': {
        'errorMessage': 'This operation is not supported.'
    },
    'StripeSizeError': {
        'errorMessage': 'The stripe size is not supported.'
    },
    'VolumeCapacityOutRange': {
        'errorMessage': 'The logical drive capacity is out of range.'
    },
    'ArraySpaceInsufficient': {
        'errorMessage': 'Insufficient array space.'
    },
    'ArraySpaceNoFree': {
        'errorMessage': 'The array has no available space.'
    },
    'NumberofDrivesForEachSpanInvalid': {
        'errorMessage': 'The number of spans is invalid.'
    },
    'NumberOfSpansInvalid': {
        'errorMessage': 'The number of physical drives does not meet service requirements.'
    },
    'StorageMediaInconsistent': {
        'errorMessage': 'The physical drives have different types of media.'
    },
    'DriveInterfacesInconsistent': {
        'errorMessage': 'The physical drives have different types of interfaces.'
    },
    'DriveStatusNotSupported': {
        'errorMessage': 'The current status of the physical drive does not support this operation.'
    },
    'DriveListDuplicateValue': {
        'errorMessage': 'Duplicate physical drive list.'
    },
    'RaidControllerLevelInvalid': {
        'errorMessage': 'The RAID level is not supported.'
    },
    'RAIDControllerNotSupportedOperation': {
        'errorMessage': 'The RAID controller card does not support this operation.'
    },
    'RAIDReachedVolumesNumberLimit': {
        'errorMessage': 'The number of logical drives has reached the maximum.'
    },
    'SSDNotExist': {
        'errorMessage': 'The physical drive is not an SSD.'
    },
    'PhysicalDiskIdCountError': {
        'errorMessage': 'The number of physical drives is incorrect.'
    },
    'PhysicalDiskIdInvalid': {
        'errorMessage': 'The physical drive ID is invalid.'
    },
    'CryptoEraseNotSupported': {
        'errorMessage': 'The controller or physical drive does not support this operation.'
    },
    'IncompleteForeignConfig': {
        'errorMessage': 'The foreign configuration is incomplete.'
    },
    'NoForeignConfig': {
        'errorMessage': 'The foreign configuration does not exist.'
    },
    'ResourceMissingAtURI': {
        'errorMessage': 'Operation failed.'
    },
    'PropertyValueFormatError': {
        'errorMessage': 'Operation failed.'
    },
    'FirmwareUploadError': {
        'errorMessage': 'Failed to upload the firmware. Please try again later.'
    },
    'FirmwareUpgrading': {
        'errorMessage': 'Upgrading ...'
    },
    'FirmwareNotMatchSignFile': {
        'errorMessage': 'The upgrade file name does not match that of the signature file.'
    },
    'UploadFileOutRange': {
        'errorMessage': 'The file size exceeds the limit.'
    },
    'EmergencyLoginUserSettingFail': {
        'errorMessage': 'The emergency login user must be a valid administrator.'
    },
    'ConflictWithLinuxDefaultUser': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'Failed to set the user name.'
    },
    'SpecifiedFileExist': {
        'errorMessage': 'A file with the same name already exists. Change the file name and upload the file again.'
    },
    'ActionFailedByBbuExist': {
        'errorMessage': 'The forced restart is invalid when the backup power function is enabled.'
    },
    'ResetOperationNotAllowed': {
        'errorMessage': 'Operation failed. This operation is not supported in the current status. Try again later.'
    },
    'TrapV3loginUser': {
        'errorMessage': 'Operation failed.',
        'attrMessage': 'When trap is enabled, the SNMP v3 trap user names cannot be changed.'
    },
    'CertificateExpired': {
        'errorMessage': 'The certificate has expired.'
    },
    'SameRWCommunityName': {
        'errorMessage': 'The read/write community name cannot be the same as the read-only community name.'
    },
    'SameROCommunityName': {
        'errorMessage': 'The read-only community name cannot be the same as the read/write community name.'
    },
    'RWCommunitySimilarWithHistory': {
        'errorMessage': 'The new and old passwords of the read/write community name must have at least two different characters.'
    },
    'ROCommunitySimilarWithHistory': {
        'errorMessage': 'The new and old passwords of the read-only community name must have at least two different characters.'
    },
    'TwoFactorCertificationFailedWithTLS': {
        'errorMessage': 'Operation failed. Please check that TLS 1.2 is enabled on Security Management.'
    },
    'UserLocked': {
        'errorMessage': 'The account has been locked.'
    },
    'IPv4AddressConflict': {
        'errorMessage': 'The IPv4 address segment conflicts with another network.'
    },
    'SystemLockdownForbid': {
        'errorMessage': 'Failed to change the password because the system lock is enabled. Please contact the administrator.'
    },
    'TLSFailedWithTwoFactorCertification': {
        'errorMessage': 'Two-factor authentication is enabled, and TLS 1.2 cannot be disabled.'
    },
    'CrlIssuingDateInvalid': {
        'errorMessage': 'Failed to import the CRL. A CRL of a later date already exists.'
    },
    'LoginUserLocked': {
        'errorMessage': 'The account has been locked. Please contact the administrator.'
    },
    'LDInitInProgress': {
        'errorMessage': 'Operation failed because the logical drive is being initialized.'
    },
    'NotSupportZhAndEnDisabled': {
        'errorMessage': 'The Chinese and English languages cannot be disabled.'
    },
    'OnlyOneLanguageEnabled': {
        'errorMessage': 'This language cannot be disabled, because it is the only enabled language.'
    },
    'LanguageNotEnabled': {
        'errorMessage': 'The language is already disabled. Refresh the page for the latest data.'
    },
    'LanguageNotSupport': {
        'errorMessage': 'This language does not support this operation.'
    },
    'CANotSupportCrlSignature': {
        'errorMessage': 'Root certificates do not support CRL signatures.'
    },
    'LanguageNotDisabled': {
        'errorMessage': 'The language is already enabled. Refresh the page for the latest data.'
    },
    'KeyFileTypeNotSupported': {
        'errorMessage': 'This key file type is not supported. '
    },
    'BodyExceedsMaxLength': {
        'errorMessage': 'The asset information length reaches the upper limit. The recommended length is within 300 characters.'
    },
    'EncryptedCertImportFailed': {
        'errorMessage': 'Failed to import the encryption certificate.'
    },
    'EncryptionAlgorithmIsWeak': {
        'errorMessage': 'The certificate with weak signature algorithm (MD5) is not supported. Generate another certificate.'
    },
    'EncryptionAlgorithmSHA1IsWeak': {
        'errorMessage': 'The certificate with weak signature algorithm (SHA1) is not supported. Generate another certificate.'
    },
    'ConfigurationExist': {
        'errorMessage': 'Operation failed because logical drives or enhanced physical drives exist.'
    },
    'NoRestorePoint': {
        'errorMessage': 'The restore point does not exist.'
    },
    'CertificateAlreadyExists': {
        'errorMessage': 'The certificate file already exists.'
    },
    'FileNameError': {
        'errorMessage': 'Incorrect file name.'
    },
    'NonSSDExist': {
        'errorMessage': 'Operation failed. The RAID group contains non-SSDs.'
    },
    'PropertyModificationNotSupportedByBBUStatus': {
        'errorMessage': 'Operation failed. Check whether the BBU is in position and fully charged.'
    },
    'VolumeShrinkNotAllowed': {
        'errorMessage': 'Operation failed. The logical drive capacity cannot be reduced.'
    },
    'AssociatedVolumeCapacityOutRange': {
        'errorMessage': 'Operation failed. The associated logical drive capacity is out of the range.'
    },
    'OperationFailed': {
        'errorMessage': 'This operation is not supported in the current state. Try again later.'
    },
    'ModifyfailedWithRCPNotInRange': {
        'errorMessage': 'Operation failed. Ensure that the read cache percentage is within the valid range.'
    }
};
