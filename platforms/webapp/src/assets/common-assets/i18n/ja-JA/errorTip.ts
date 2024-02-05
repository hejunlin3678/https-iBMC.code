export const errorMessage = {
    'ServiceException': {
        'errorMessage': 'サービスの応答がタイムアウトになりました。もう一度ログインしてください。'
    },
    'SessionTimeout': {
        'errorMessage': 'ログインセッションがタイムアウトしました。再度ログインしてください。'
    },
    'InvalidSession': {
        'errorMessage': '無効なセッションです。再度ログインしてください。'
    },
    'NoValidSession': {
        'errorMessage': 'ログインセッションがタイムアウトしました。再度ログインしてください。'
    },
    'SessionKickout': {
        'errorMessage': 'システム管理者に強制的にログアウトされました。'
    },
    'SessionRelogin': {
        'errorMessage': '現在のアカウントで別の端末からログインしています。'
    },
    'SessionChanged': {
        'errorMessage': 'ユーザー情報が変更されました。再度ログインしてください。'
    },
    'InsufficientPrivilege': {
        'errorMessage': 'この操作を実行する権限がありません。'
    },
    'PropertyModificationNeedPrivilege': {
        'errorMessage': 'この操作を実行する権限がありません。'
    },
    'AuthorizationFailed': {
        'attrMessage': 'パスワードが間違っているか、またはユーザーアカウントがロックされています。'
    },
    'AuthorizationUserNoAccess': {
        'attrMessage': '該当ユーザーはアクセス権がありません。'
    },
    'AuthorizationUserPasswordExpired': {
        'attrMessage': 'パスワードが有効期限切れです。'
    },
    'AuthorizationUserRestricted': {
        'attrMessage': 'ログインが制限されました。'
    },
    'AuthorizationUserLocked': {
        'attrMessage': 'アカウントがロックされました。'
    },
    'InvalidUserName': {
        'errorMessage': '操作に失敗',
        'attrMessage': 'パスワードが間違っているか、またはユーザーアカウントがロックされています。'
    },
    'ResourceAlreadyExists': {
        'errorMessage': '操作に失敗',
        'attrMessage': 'このユーザー名は既に使用されています。'
    },
    'UserNameIsRestricted': {
        'errorMessage': '操作に失敗',
        'attrMessage': 'ユーザー名が制限されています。'
    },
    'InvalidPasswordLength': {
        'errorMessage': '操作に失敗',
        'attrMessage': '無効なパスワード。'
    },
    'InvalidPassword': {
        'errorMessage': '操作に失敗',
        'attrMessage': '無効なパスワード。'
    },
    'PasswordComplexityCheckFail': {
        'errorMessage': '操作に失敗',
        'attrMessage': 'パスワードが複雑さの要件を満たしていません。'
    },
    'PasswordInWeakPWDDict': {
        'errorMessage': '操作に失敗',
        'attrMessage': '弱いパスワード辞書に含まれることはできません。'
    },
    'UserIsLoggingIn': {
        'errorMessage': '操作に失敗',
        'attrMessage': 'このユーザーはSSH経由でシステムにログインしています。削除できません。'
    },
    'PasswordNotSatisfyComplexity': {
        'errorMessage': '操作に失敗',
        'attrMessage': '新しいパスワードにおいて、2つの文字が古いパスワードと異なる場所で使用されていること。'
    },
    'PropertyValueExceedsMaxLength': {
        'errorMessage': '操作に失敗',
        'attrMessage': 'パスワードの設定に失敗しました。'
    },
    'DuringMinimumPasswordAge': {
        'errorMessage': '操作に失敗',
        'attrMessage': 'パスワードの最小経過期間を経過していないため、パスワードを変更できません。'
    },
    'EmergencyLoginUser': {
        'errorMessage': '操作に失敗',
        'attrMessage': '緊急ログインユーザーは変更できません。'
    },
    'CannotDisableLastAdministrator': {
        'errorMessage': '操作に失敗',
        'attrMessage': '最後の管理者は無効化できません。'
    },
    'CannotDeleteLastAdministrator': {
        'errorMessage': '操作に失敗',
        'attrMessage': '最後の管理者は削除できません。'
    },
    'InvalidPasswordSameWithHistory': {
        'errorMessage': '操作に失敗',
        'attrMessage': '制限される履歴パスワードは使用できません。'
    },
    'AccountForbidRemoved': {
        'errorMessage': 'このユーザーは緊急ログインユーザー、またはSNMPv3 trapユーザーです。削除できません。'
    },
    'PublicKeyNotExist': {
        'errorMessage': '公開鍵ファイルが存在しません。'
    },
    'PublicKeyImportError': {
        'errorMessage': '公開鍵の形式が間違っています'
    },
    'DumpExportingErr': {
        'errorMessage': 'タスク処理中です。お待ちください。'
    },
    'DifferentDriveSectorsSize': {
        'errorMessage': '選択した物理ディスクのセクターサイズが一致しません。'
    },
    'InsufficientMemoryErr': {
        'errorMessage': '更新のための空き容量が不足しているため、収集に失敗しました。'
    },
    'UpgradingErr': {
        'errorMessage': 'アップグレード中です。後で再試行してください。'
    },
    'DisableScreenShotInCurrentState': {
        'errorMessage': 'KVMがプライベートモードの場合、手動によるスクリーンショット取得はできません。'
    },
    'ActivePsNotNecessary': {
        'errorMessage': 'アクティブPSUを設定する必要はありません。'
    },
    'ActivePsNecessary': {
        'errorMessage': 'アクティブPSUを選択してください。'
    },
    'ActivePsNotPosition': {
        'errorMessage': 'アクティブなPSUが1つもありません。'
    },
    'NoStandbyPsPosition': {
        'errorMessage': 'スタンバイPSUがありません。'
    },
    'ActivePsNotHealth': {
        'errorMessage': 'アクティブ/スタンバイのPSUの状態に異常があります。'
    },
    'CurrentPowerHigh': {
        'errorMessage': '現在の電力がすべてのアクティブPSUの電力定格の75%を超えています。'
    },
    'ActivePsNotEnough': {
        'errorMessage': 'アクティブPSUが不足しています。'
    },
    'PsNotSupport': {
        'errorMessage': 'PSUが1つもサポートされていません。'
    },
    'ActiveStandbyModeUnsupport': {
        'errorMessage': '設定がサポートされていません。'
    },
    'DuplicateExportingErr': {
        'errorMessage': 'ダウンロード中です。後で再試行してください。'
    },
    'OperationInProcess': {
        'errorMessage': 'ログ収集タスクが実行されています。後で再試行してください。'
    },
    'ResetOperationFailed': {
        'errorMessage': 'BIOSはアップグレードしているので、少々お待ちください。'
    },
    'ResetOperationFailedVRD': {
        'errorMessage': 'VRDはアップグレードしているので、少々お待ちください。'
    },
    'FileNotExist': {
        'errorMessage': 'ビデオファイルが存在しません。'
    },
    'DownFileNotExist': {
        'errorMessage': 'ダウンロードするファイルは存在しません。'
    },
    'RecordExceedsMaxNum': {
        'errorMessage': '作業履歴の数が上限に達しました。'
    },
    'InvalidRecordId': {
        'errorMessage': '実行履歴が見つかりません。'
    },
    'CertificateImportingErr': {
        'errorMessage': '証明書をアップロードしています。後で再試行してください。'
    },
    'KeyImportError': {
        'errorMessage': '秘密鍵の形式が正しくありません。'
    },
    'FailedEnableTwoFactorCertification': {
        'errorMessage': '少なくとも1つのユーザーに対してクライアントおよびルート証明書を設定してください。'
    },
    'CertificateQtyReachLimit': {
        'errorMessage': '証明書の数が上限に達しました。'
    },
    'RootCertificateAlreadyExists': {
        'errorMessage': 'このファイルは既に存在します。'
    },
    'RootCertificateNotExist': {
        'errorMessage': '証明書が存在しません。'
    },
    'ClientCertLackProperties': {
        'errorMessage': 'インポートした証明書が正しくありません。'
    },
    'ClientCertificateAlreadyExists': {
        'errorMessage': 'ファイルは既に存在します。'
    },
    'ClientCertIssuerMismatch': {
        'errorMessage': 'ルート証明書の検索に失敗しました。'
    },
    'ClientCertQtyExceedLimit': {
        'errorMessage': '証明書の数が上限に達しました。'
    },
    'ClientCertificateNotExist': {
        'errorMessage': '証明書が存在しません。'
    },
    'CertificateFormatError': {
        'errorMessage': '証明書の形式が正しくありません。'
    },
    'RootCertLackProperties': {
        'errorMessage': '証明書はルート証明書ではありません。'
    },
    'RootCertificateMismatch': {
        'errorMessage': 'CRLがルート証明書と一致しません。'
    },
    'RootCertificateNotImported': {
        'errorMessage': 'CRLがルート証明書と一致しません。'
    },
    'CrlFileFormatError': {
        'errorMessage': '無効なCRLファイルまたはBase64でエンコードされていないファイルです。'
    },
    'RootCertificateIdInvalid': {
        'errorMessage': '無効なルート証明書IDです。'
    },
    'MinPwdAgeAndPwdValidityRestrictEachOther': {
        'errorMessage': '操作に失敗',
        'attrMessage': 'パスワードの最小使用期間は、パスワードの有効期間より10日以上短い必要があります。'
    },
    'DisableKVMEncryptionFailed': {
        'errorMessage': 'VMM暗号化を無効にした後にのみ、KVM暗号化を無効にできます。'
    },
    'KvmInUse': {
        'errorMessage': 'KVMは使用中です。後でもう一度試してください。'
    },
    'CommunityNameInWeakPWDDict': {
        'errorMessage': '弱いパスワード辞書に含まれることはできません。'
    },
    'CommunityNameNotContainSpace': {
        'errorMessage': 'パスワードが複雑さの要件を満たしていません。'
    },
    'SimpleCommunityName': {
        'errorMessage': 'パスワードが複雑さの要件を満たしていません。'
    },
    'EnableVMMEncryptionFailed': {
        'errorMessage': 'KVM暗号化を有効にした後にのみ、VMM暗号化を有効にできます。'
    },
    'VncInUse': {
        'errorMessage': 'VNCが使用されています。後で再試行してください。'
    },
    'CSRGeneratingErr': {
        'errorMessage': 'CSR生成中です。後で再試行してください。'
    },
    'CSRExportingErr': {
        'errorMessage': 'CSR生成中です。後で再試行してください。'
    },
    'CertMismatch': {
        'errorMessage': '検証が失敗したため、証明書のインポートに失敗しました。'
    },
    'V3UserNameNotUsed': {
        'errorMessage': '操作に失敗'
    },
    'InternalError': {
        'errorMessage': '操作に失敗'
    },
    'NotSupportZhAndEnUninstalled': {
        'errorMessage': '中国語および英語の言語パックは削除できません。'
    },
    'OnlyOneLanguageInstalled': {
        'errorMessage': '当該言語はシステムでサポートされている唯一の言語であるため、削除できません。'
    },
    'LanguageNotInstalled': {
        'errorMessage': '選択した言語パックが既にアンインストールされます。この画面をリフラッシュしてください。'
    },
    'LicenseInstalling': {
        'errorMessage': 'ライセンスをインストールしています。後で再試行してください。'
    },
    'CertImportFailed': {
        'errorMessage': '証明書のインポートに失敗しました。'
    },
    'LicenseVerifyFailed': {
        'errorMessage': 'ライセンスの認証に失敗しました。'
    },
    'LicenseActivateFailed': {
        'errorMessage': 'ライセンスの有効化に失敗しました。'
    },
    'LicenseExporting': {
        'errorMessage': 'ライセンスをエクスポートしています。後で再試行してください。'
    },
    'LicenseNotInstall': {
        'errorMessage': 'ライセンスがインストールされていません。'
    },
    'PortIdModificationFailed': {
        'errorMessage': 'このポートは使用されています'
    },
    'PropertyUnknown': {
        'errorMessage': '操作に失敗'
    },
    'MalformedJSON': {
        'errorMessage': '操作に失敗'
    },
    'IncorrectSenderInfo': {
        'errorMessage': 'ユーザー名、パスワード、または送信者のアドレスが正しくありません。'
    },
    'SmtpConnectionFailed': {
        'errorMessage': 'SMTPサーバーへの接続に失敗したか、SMTPサーバーが匿名ログインをサポートしていません。'
    },
    'FeatureDisabledAndNotSupportOperation': {
        'errorMessage': '現在の状態ではこの操作がサポートされません。'
    },
    'TrapMemberIDNotExist': {
        'errorMessage': 'トラップサービスまたはトラップサーバーが無効になっているか、トラップサーバーのアドレスが空白であるため、トラップテストに失敗しました。'
    },
    'SyslogTestFailed': {
        'errorMessage': 'ログサービスまたはログサーバーが無効になっているか、ログサーバーのアドレスが空白であるため、システムログテストに失敗しました。'
    },
    'SendingEmailFailed': {
        'errorMessage': 'メールの送信に失敗したか、受信者のアドレスが正しくありません。'
    },
    'TrapSendTestErr': {
        'errorMessage': 'トラップサービスまたはトラップサーバーが無効になっているか、トラップサーバーのアドレスが空白であるため、トラップテストに失敗しました。'
    },
    'CurrentStatusNotSupportOperation': {
        'errorMessage': '操作に失敗'
    },
    'RAIDControllerNotSupported': {
        'errorMessage': '操作に失敗'
    },
    'PropertyNotSupportedModificationByCurrentStatus': {
        'errorMessage': '操作に失敗'
    },
    'LocatedFailedByPDRebuilding': {
        'errorMessage': '操作に失敗'
    },
    'PropertyModificationNotSupportedByPDStatus': {
        'errorMessage': '操作に失敗'
    },
    'PropertyModificationFailedByRAIDlevelNotSupported': {
        'errorMessage': '操作に失敗'
    },
    'PropertyModificationFailedByIterfaceConflict': {
        'errorMessage': '操作に失敗'
    },
    'PropertyModificationFailedByMediaTypeConflict': {
        'errorMessage': '操作に失敗しました。 追加するメディアの種類が既存のメディアの種類と競合しています。'
    },
    'PropertyConflict': {
        'errorMessage': '操作に失敗'
    },
    'ModifyfailedWithRAIDCardState': {
        'errorMessage': 'RAIDコントローラカード管理の準備ができていないため'
    },
    'ModifyfailedWithCachecade': {
        'errorMessage': '操作に失敗'
    },
    'ModifyfailedWithRaidControllerNotSupport': {
        'errorMessage': '操作に失敗'
    },
    'ModifyfailedWithSSD': {
        'errorMessage': '操作に失敗'
    },
    'InvalidVolumeName': {
        'errorMessage': '操作に失敗'
    },
    'CurrentStatusNotSupport': {
        'errorMessage': '現在のステータスでは、この操作は実行できません。'
    },
    'OperationNotSupported': {
        'errorMessage': 'この操作は実行できません。'
    },
    'StripeSizeError': {
        'errorMessage': 'ストライプサイズはサポートされていません。'
    },
    'VolumeCapacityOutRange': {
        'errorMessage': '論理ドライブの容量が範囲外です。'
    },
    'ArraySpaceInsufficient': {
        'errorMessage': 'アレイの容量が不足しています。'
    },
    'ArraySpaceNoFree': {
        'errorMessage': 'アレイに空き容量がありません。'
    },
    'NumberofDrivesForEachSpanInvalid': {
        'errorMessage': 'Span数が無効です。'
    },
    'NumberOfSpansInvalid': {
        'errorMessage': '物理ドライブの数がサービス要件を満たしていません。'
    },
    'StorageMediaInconsistent': {
        'errorMessage': '物理ドライブに異なるタイプのメディアが存在します。'
    },
    'DriveInterfacesInconsistent': {
        'errorMessage': '物理ドライブに異なるタイプのインターフェースが存在します。'
    },
    'DriveStatusNotSupported': {
        'errorMessage': '現在の物理ドライブのステータスでは、この操作は実行できません。'
    },
    'DriveListDuplicateValue': {
        'errorMessage': '物理ドライブリストが重複しています。'
    },
    'RaidControllerLevelInvalid': {
        'errorMessage': 'RAIDレベルはサポートされていません。'
    },
    'RAIDControllerNotSupportedOperation': {
        'errorMessage': 'RAIDカードはこの操作をサポートしていません。'
    },
    'RAIDReachedVolumesNumberLimit': {
        'errorMessage': '論理ドライブの数が上限に達しました。'
    },
    'SSDNotExist': {
        'errorMessage': '物理ドライブはSSDではありません。'
    },
    'PhysicalDiskIdCountError': {
        'errorMessage': '物理ドライブの数が正しくありません。'
    },
    'PhysicalDiskIdInvalid': {
        'errorMessage': '物理ドライブIDが無効です。'
    },
    'CryptoEraseNotSupported': {
        'errorMessage': 'コントローラまたは物理ドライブはこの操作をサポートしていません。'
    },
    'IncompleteForeignConfig': {
        'errorMessage': '外部構成情報が不完全です。'
    },
    'NoForeignConfig': {
        'errorMessage': '外部構成情報が存在しません。'
    },
    'ResourceMissingAtURI': {
        'errorMessage': '操作に失敗'
    },
    'PropertyValueFormatError': {
        'errorMessage': '操作に失敗'
    },
    'FirmwareUploadError': {
        'errorMessage': 'ファームウェアのアップロードに失敗しました。 しばらく待ってからもう一度やり直してください。'
    },
    'FirmwareUpgrading': {
        'errorMessage': 'システムはアップデート中です。'
    },
    'FirmwareNotMatchSignFile': {
        'errorMessage': 'アップグレードファイル名は署名ファイル名と一致していません。'
    },
    'UploadFileOutRange': {
        'errorMessage': 'ファイルのサイズは制限を超えました。'
    },
    'EmergencyLoginUserSettingFail': {
        'errorMessage': '非常時ユーザーは有効な管理者アカウントである必要があります。'
    },
    'ConflictWithLinuxDefaultUser': {
        'errorMessage': '操作に失敗',
        'attrMessage': 'ユーザー名の設定に失敗しました。'
    },
    'SpecifiedFileExist': {
        'errorMessage': '同じ名前のファイルが既に存在します。ファイル名を変更し、もう一度アップロードしてください。'
    },
    'ActionFailedByBbuExist': {
        'errorMessage': 'バックアップバッテリ機能が有効なため、強制リセットは無効です。'
    },
    'IPv4AddressConflict': {
        'errorMessage': 'IPv4アドレスセグメントは他のネットワークと競合しました。'
    },
    'SystemLockdownForbid': {
        'errorMessage': 'システムロックが有効になっているため、パスワードの変更に失敗しました。管理者に連絡してください。'
    },
    'ResetOperationNotAllowed': {
        'errorMessage': '操作に失敗しました。この操作は現在の状態ではサポートされていません。後で再試行してください。'
    },
    'CertificateExpired': {
        'errorMessage': '証明書の有効期限が切れています。'
    },
    'CustomFirmwareUploadError': {
        '    errorTips': '公開鍵のアップロードに失敗しました。後でもう一度試してください。'
    },
    'UserLdapAuthFail': {
        '    errorTips': 'ユーザー名またはパスワードが正しくありません。もう一度入力してください。それでもログインできない場合、LDAPサーバーのTLSバージョンが1.2以上であることを確認してください。'
    },
    'SameRWCommunityName': {
        'errorMessage': '読み取り/書き込みコミュニティ名を読み取り専用コミュニティ名と同じにすることはできません。'
    },
    'SameROCommunityName': {
        'errorMessage': '読み取り専用コミュニティ名を読み取り/書き込みコミュニティ名と同じにすることはできません。'
    },
    'RWCommunitySimilarWithHistory': {
        'errorMessage': '読み取り/書き込みコミュニティ名の新しいパスワードと古いパスワードは少なくとも2文字以上異なる必要があります。'
    },
    'ROCommunitySimilarWithHistory': {
        'errorMessage': '読み取り専用コミュニティ名の新しいパスワードと古いパスワードは少なくとも2文字以上異なる必要があります。'
    },
    'TwoFactorCertificationFailedWithTLS': {
        'errorMessage': '操作に失敗しました。[セキュリティ管理]でTLS 1.2が有効になっていることを確認してください。'
    },
    'TLSFailedWithTwoFactorCertification': {
        'errorMessage': '二要素認証が有効になっており、TLS 1.2を無効にすることはできません。'
    },
    'CrlIssuingDateInvalid': {
        'errorMessage': '発行日付がより新しいCRLが存在するため、インポートに失敗しました。'
    },
    'TrapV3loginUser': {
        'errorMessage': '操作に失敗',
        'attrMessage': 'トラップが有効な場合、SNMP v3トラップのユーザー名は変更できません。'
    },
    'SessionLimitExceeded': {
        'errorMessage': 'セッション数が上限に達しています。'
    },
    'UserLocked': {
        'errorMessage': 'アカウントがロックされました。'
    },
    'SameCommunityName': {
        'errorMessage': '読み込み専用コミュニティ名は、読み込み/書き込みコミュニティ名と異なる必要があります。'
    },
    'LoginUserLocked': {
        'errorMessage': 'アカウントがロックされました。管理者に連絡してください。'
    },
    'LDInitInProgress': {
        'errorMessage': '論理ドライブが初期化されているため、操作に失敗しました。'
    },
    'NotSupportZhAndEnDisabled': {
        'errorMessage': '中国語と英語を無効にすることはできません。'
    },
    'OnlyOneLanguageEnabled': {
        'errorMessage': 'この言語は唯一有効な言語なので、無効にすることはできません。'
    },
    'LanguageNotEnabled': {
        'errorMessage': '言語が無効になっています。画面を更新してください。'
    },
    'LanguageNotSupport': {
        'errorMessage': 'この言語はこの操作をサポートしていません。'
    },
    'CANotSupportCrlSignature': {
        'errorMessage': 'ルート証明書はCRL署名をサポートしていません。'
    },
    'LanguageNotDisabled': {
        'errorMessage': '言語はすでに有効になっています。画面を更新して最新情報を確認してください。'
    },
    'KeyFileTypeNotSupported': {
        'errorMessage': 'このタイプの鍵ファイルはサポートされていません。'
    },
    'BodyExceedsMaxLength': {
        'errorMessage': '資産情報の長さが上限に達しています。300文字以内の文字数をお勧めします。'
    },
    'EncryptedCertImportFailed': {
        'errorMessage': '暗号化証明書のインポートに失敗しました。'
    },
    'EncryptionAlgorithmIsWeak': {
        'errorMessage': '弱い署名アルゴリズム(MD5)の証明書はサポートされていません。別の証明書を作成してください。'
    },
    'EncryptionAlgorithmSHA1IsWeak': {
        'errorMessage': '弱い署名アルゴリズム(SHA1)の証明書はサポートされていません。別の証明書を作成してください。'
    },
    'ConfigurationExist': {
        'errorMessage': '論理ドライブまたは強化型物理ドライブが存在するため、操作に失敗しました。'
    },
    'NoRestorePoint': {
        'errorMessage': '復元ポイントは存在しません。'
    },
    'CertificateAlreadyExists': {
        'errorMessage': '証明書ファイルは既に存在します。'
    },
    'FileNameError': {
        'errorMessage': 'ファイル名が正しくありません。'
    },
    'NonSSDExist': {
        'errorMessage': '操作に失敗しました。RAIDグループにSSD以外のディスクが含まれています。'
    },
    'PropertyModificationNotSupportedByBBUStatus': {
        'errorMessage': '操作に失敗しました。BBUが所定位置にあり、完全に充電されているか確認してください。'
    },
    'VolumeShrinkNotAllowed': {
        'errorMessage': '作に失敗しました。論理ドライブの容量削減ができません。'
    },
    'AssociatedVolumeCapacityOutRange': {
        'errorMessage': '操作に失敗しました。関連付けられた論理ドライブの容量が範囲外です。'
    },
    'OperationFailed': {
        'errorMessage': '現在の状態では、この操作はサポートされていません。後で再試行してください。'
    },
    'ModifyfailedWithRCPNotInRange': {
        'errorMessage': '操作に失敗しました。読み取りキャッシュの割合が有効範囲内であることを確認してください。'
    }
};
