export const errorMessage = {
    'ServiceException': {
        'errorMessage': 'Таймаут сервеса. Пожалуйста, войдите снова.'
    },
    'InvalidSession': {
        'errorMessage': 'Недопустимая сессия. Повторите попытку.'
    },
    'SessionTimeout': {
        'errorMessage': 'Таймаут сеанса входа. Повторите попытку.'
    },
    'NoValidSession': {
        'errorMessage': 'Таймаут сеанса входа. Повторите попытку.'
    },
    'SessionKickout': {
        'errorMessage': 'Администратор принудительно завершил ваш сеанс.'
    },
    'SessionRelogin': {
        'errorMessage': 'Выполнен вход в аккаунт с другого устройства.'
    },
    'SessionChanged': {
        'errorMessage': 'Права пользователя были изменены. Выйдите и войдите в систему повторно.'
    },
    'SessionLimitExceeded': {
        'errorMessage': 'Достигнуто максимальное количество сеансов.'
    },
    'InsufficientPrivilege': {
        'errorMessage': 'Недостаточно прав для выполнения операции.'
    },
    'PropertyModificationNeedPrivilege': {
        'errorMessage': 'Недостаточно прав для выполнения операции.'
    },
    'AuthorizationFailed': {
        'attrMessage': 'Неверный пароль или аккаунт пользователя заблокирован.'
    },
    'AuthorizationUserNoAccess': {
        'errorMessage': 'Нет разрешения.'
    },
    'AuthorizationUserPasswordExpired': {
        'errorMessage': 'Срок действия пароля истек.'
    },
    'AuthorizationUserRestricted': {
        'errorMessage': 'Запрет входа.'
    },
    'AuthorizationUserLocked': {
        'errorMessage': 'Аккаунт заблокирован.'
    },
    'InvalidUserName': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Неверный пароль или аккаунт пользователя заблокирован.'
    },
    'ResourceAlreadyExists': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Такой пользователь уже существует.'
    },
    'UserNameIsRestricted': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Такое имя пользователя запрещено.'
    },
    'InvalidPasswordLength': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Неверный пароль.'
    },
    'InvalidPassword': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Неверный пароль.'
    },
    'PasswordComplexityCheckFail': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Пароль не соответствует требованиям сложности.'
    },
    'PasswordInWeakPWDDict': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Нельзя включить в справочник слабых паролей.'
    },
    'UserIsLoggingIn': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Пользователь в системе и не может быть изменен.'
    },
    'PasswordNotSatisfyComplexity': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Новый пароль должен отличаться от старого, как минимум, на два символа.'
    },
    'PropertyValueExceedsMaxLength': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Неверный пароль.'
    },
    'DuringMinimumPasswordAge': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'В период минимального срока действия пароля пароль невозможно изменить.。'
    },
    'EmergencyLoginUser': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Нельзя изменить пользователя для экстренного входа.'
    },
    'CannotDisableLastAdministrator': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Последний администратор не может быть отключен.'
    },
    'CannotDeleteLastAdministrator': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Последний администратор не может быть удален.'
    },
    'InvalidPasswordSameWithHistory': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Нельзя использовать старый пароль.'
    },
    'AccountForbidRemoved': {
        'errorMessage': 'Этот пользователь - пользователь для экстренного входа или пользователь SNMPv3 trap, его нельзя удалить.'
    },
    'PublicKeyNotExist': {
        'errorMessage': 'Файл открытого ключа не найден.'
    },
    'PublicKeyImportError': {
        'errorMessage': 'Неверный формат открытого ключа.'
    },
    'DumpExportingErr': {
        'errorMessage': 'Сбор данных… Повторите попытку позже.'
    },
    'DifferentDriveSectorsSize': {
        'errorMessage': 'Размеры секторов выбранных физических дисков различны.'
    },
    'InsufficientMemoryErr': {
        'errorMessage': 'Сбой сбора данных. Недостаточно памяти.'
    },
    'UpgradingErr': {
        'errorMessage': 'Обновление… Повторите попытку позже.'
    },
    'DisableScreenShotInCurrentState': {
        'errorMessage': 'В приватном режиме KVM создание скриншота вручную не поддерживается.'
    },
    'ActivePsNotNecessary': {
        'errorMessage': 'Настраивать активный блок питания не требуется.'
    },
    'ActivePsNecessary': {
        'errorMessage': 'Выберите активный блок питания.'
    },
    'ActivePsNotPosition': {
        'errorMessage': 'Не обнаружен один или более активных блоков питания.'
    },
    'NoStandbyPsPosition': {
        'errorMessage': 'Не обнаружен резервный блок питания.'
    },
    'ActivePsNotHealth': {
        'errorMessage': 'Сбой активного или резервного блока питания.'
    },
    'CurrentPowerHigh': {
        'errorMessage': 'Текущее энергопотребление превышает 75% мощности всех активных блоков питания.'
    },
    'ActivePsNotEnough': {
        'errorMessage': 'Активных блоков питания недостаточно.'
    },
    'PsNotSupport': {
        'errorMessage': 'Данный блок питания не поддерживает работу в активном/резервном режиме.'
    },
    'ActiveStandbyModeUnsupport': {
        'errorMessage': 'Активный/резервный режим не поддерживается.'
    },
    'DuplicateExportingErr': {
        'errorMessage': 'Загрузка… Повторите попытку позже.'
    },
    'OperationInProcess': {
        'errorMessage': 'Выполняется задача сбора данных журнала. Повторите попытку позже.'
    },
    'ResetOperationFailed': {
        'errorMessage': 'Идет обновление BIOS ...'
    },
    'ResetOperationFailedVRD': {
        'errorMessage': 'Идет обновление VRD ...'
    },
    'FileNotExist': {
        'errorMessage': 'Видеофайл не найден.'
    },
    'RecordExceedsMaxNum': {
        'errorMessage': 'Достигнуто максимальное число рабочих записей.'
    },
    'InvalidRecordId': {
        'errorMessage': 'Рабочая запись не найдена.'
    },
    'CertificateImportingErr': {
        'errorMessage': 'Выгрузка сертификата… Повторите попытку позже.'
    },
    'KeyImportError': {
        'errorMessage': 'Некорректный формат ключа.'
    },
    'FailedEnableTwoFactorCertification': {
        'errorMessage': 'Настройте корневой сертификат и сертификат клиента хотя бы для одного пользователя.'
    },
    'CertificateQtyReachLimit': {
        'errorMessage': 'Достигнуто максимальное количество сертификатов.'
    },
    'RootCertificateAlreadyExists': {
        'errorMessage': 'Такой файл уже существует.'
    },
    'RootCertificateNotExist': {
        'errorMessage': 'Сертификат не найден.'
    },
    'ClientCertLackProperties': {
        'errorMessage': 'Сбой импорта сертификата клиента для двухуровневой аутентификации из-за отсутствия необходимых прав сертификата.'
    },
    'ClientCertificateAlreadyExists': {
        'errorMessage': 'Такой файл уже существует.'
    },
    'ClientCertIssuerMismatch': {
        'errorMessage': 'Не удалось найти корневой сертификат.'
    },
    'ClientCertQtyExceedLimit': {
        'errorMessage': 'Достигнуто максимальное количество сертификатов.'
    },
    'ClientCertificateNotExist': {
        'errorMessage': 'Сертификат не найден.'
    },
    'CertificateFormatError': {
        'errorMessage': 'Неверный формат сертификата.'
    },
    'RootCertLackProperties': {
        'errorMessage': 'Сбой импорта сертификата клиента для двухуровневой аутентификации из-за отсутствия необходимых прав сертификата.'
    },
    'RootCertificateMismatch': {
        'errorMessage': 'CRL не соответствует корневому сертификату.'
    },
    'RootCertificateNotImported': {
        'errorMessage': 'CRL не соответствует корневому сертификату.'
    },
    'CrlFileFormatError': {
        'errorMessage': 'Неверный CRL.'
    },
    'RootCertificateIdInvalid': {
        'errorMessage': 'Неверный идентификатор корневого сертификата.'
    },
    'MinPwdAgeAndPwdValidityRestrictEachOther': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Значение минимального срока действия пароля должно быть, по меньшей мере, на 10 дней меньше срока действия пароля.'
    },
    'DisableKVMEncryptionFailed': {
        'errorMessage': 'Перед отключением шифрования KVM отключите шифрование VMM.。'
    },
    'KvmInUse': {
        'errorMessage': 'KVM используется. Пожалуйста, попробуйте еще раз по'
    },
    'CommunityNameInWeakPWDDict': {
        'errorMessage': 'Нельзя включить в справочник слабых паролей.'
    },
    'CommunityNameNotContainSpace': {
        'errorMessage': 'Пароль не соответствует требованиям сложности.'
    },
    'SimpleCommunityName': {
        'errorMessage': 'Пароль не соответствует требованиям сложности.'
    },
    'SameCommunityName': {
        'errorMessage': 'Имя сообщества только для чтения не может совпадать с именем сообщества чтения-записи.'
    },
    'EnableVMMEncryptionFailed': {
        'errorMessage': 'Включите шифрование KVM до включения шифрования VMM.'
    },
    'VncInUse': {
        'errorMessage': 'Сбой настройки шифрования SSL из-за активного VNC. Повторите попытку позже.'
    },
    'CSRGeneratingErr': {
        'errorMessage': 'Генерация CSR… Повторите попытку позже.'
    },
    'CSRExportingErr': {
        'errorMessage': 'Генерация CSR… Повторите попытку позже.'
    },
    'CertMismatch': {
        'errorMessage': 'Не удалось импортировать сертификат из-за неудачной проверки.'
    },
    'V3UserNameNotUsed': {
        'errorMessage': 'Сбой операции.'
    },
    'NotSupportZhAndEnUninstalled': {
        'errorMessage': 'Китайский и английский языковые пакеты невозможно удалить.'
    },
    'OnlyOneLanguageInstalled': {
        'errorMessage': 'Это единственный языковой пакет, используемый системой. Удаление невозможно.'
    },
    'LanguageNotInstalled': {
        'errorMessage': 'Языковой пакет уже удален. Обновите страницу для получения последних данных.'
    },
    'LicenseInstalling': {
        'errorMessage': 'Это единственный язык, используемый системой. Удаление невозможно.'
    },
    'CertImportFailed': {
        'errorMessage': 'Не удалось импортировать сертификат.'
    },
    'LicenseVerifyFailed': {
        'errorMessage': 'Сбой подтверждения лицензии'
    },
    'LicenseActivateFailed': {
        'errorMessage': 'Сбой активации лицензии'
    },
    'LicenseExporting': {
        'errorMessage': 'Экспорт лицензии… Повторите попытку позже.'
    },
    'LicenseNotInstall': {
        'errorMessage': 'Лицензии не установлены.'
    },
    'PortIdModificationFailed': {
        'errorMessage': 'Порт уже используется.'
    },
    'PropertyUnknown': {
        'errorMessage': 'Сбой операции.'
    },
    'MalformedJSON': {
        'errorMessage': 'Сбой операции.'
    },
    'IncorrectSenderInfo': {
        'errorMessage': 'Неверное имя пользователя, пароль или адрес отправителя.'
    },
    'SmtpConnectionFailed': {
        'errorMessage': 'Не удалось подключиться к SMTP-серверу, или SMTP-сервер не поддерживает анонимный вход.'
    },
    'FeatureDisabledAndNotSupportOperation': {
        'errorMessage': 'Текущее состояние не поддерживает эту операцию.'
    },
    'TrapMemberIDNotExist': {
        'errorMessage': 'Не удалось выполнить тест на улавливание, так как Trap-сервис или Trap-сервер отключен или адрес Trap-сервера пуст.'
    },
    'SyslogTestFailed': {
        'errorMessage': 'Не удалось выполнить тест системного журнала, так как сервис журнала или сервер журнала отключен или адрес сервера журнала пуст.'
    },
    'SendingEmailFailed': {
        'errorMessage': 'Не удалось отправить письмо по электронной почте, или адрес получателя неверный.'
    },
    'TrapSendTestErr': {
        'errorMessage': 'Не удалось выполнить тест на улавливание, так как Trap-сервис или Trap-сервер отключен или адрес Trap-сервера пуст.'
    },
    'CurrentStatusNotSupportOperation': {
        'errorMessage': 'Сбой операции.'
    },
    'RAIDControllerNotSupported': {
        'errorMessage': 'Сбой операции.'
    },
    'PropertyNotSupportedModificationByCurrentStatus': {
        'errorMessage': 'Сбой операции.'
    },
    'LocatedFailedByPDRebuilding': {
        'errorMessage': 'Сбой операции.'
    },
    'PropertyModificationNotSupportedByPDStatus': {
        'errorMessage': 'Сбой операции.'
    },
    'PropertyModificationFailedByRAIDlevelNotSupported': {
        'errorMessage': 'Сбой операции.'
    },
    'PropertyModificationFailedByIterfaceConflict': {
        'errorMessage': 'Сбой операции.'
    },
    'PropertyModificationFailedByMediaTypeConflict': {
        'errorMessage': 'Сбой операции. Тип добавляемого носителя конфликтует с существующим типом носителя.'
    },
    'PropertyConflict': {
        'errorMessage': 'Сбой операции.'
    },
    'ModifyfailedWithRAIDCardState': {
        'errorMessage': 'Управление RAID-контроллером не готово.'
    },
    'ModifyfailedWithCachecade': {
        'errorMessage': 'Сбой операции.'
    },
    'ModifyfailedWithRaidControllerNotSupport': {
        'errorMessage': 'Сбой операции.'
    },
    'ModifyfailedWithSSD': {
        'errorMessage': 'Сбой операции.'
    },
    'InvalidVolumeName': {
        'errorMessage': 'Сбой операции.'
    },
    'InternalError': {
        'errorMessage': 'Сбой операции.'
    },
    'CurrentStatusNotSupport': {
        'errorMessage': 'Данная операция не поддерживается в текущем состоянии.'
    },
    'OperationNotSupported': {
        'errorMessage': 'Операция не поддерживается.'
    },
    'StripeSizeError': {
        'errorMessage': 'Размер полосы не поддерживается.'
    },
    'VolumeCapacityOutRange': {
        'errorMessage': 'Емкость логического диска вне допустимых пределов.'
    },
    'ArraySpaceInsufficient': {
        'errorMessage': 'Недостаточно места в массиве.'
    },
    'ArraySpaceNoFree': {
        'errorMessage': 'В массиве нет доступного места.'
    },
    'NumberofDrivesForEachSpanInvalid': {
        'errorMessage': 'Неверное количество объединений.'
    },
    'NumberOfSpansInvalid': {
        'errorMessage': 'Количество физических накопителей не соответствует рабочим требованиям.'
    },
    'StorageMediaInconsistent': {
        'errorMessage': 'У физических накопителей разные типы носителей.'
    },
    'DriveInterfacesInconsistent': {
        'errorMessage': 'У физических накопителей разные типы интерфейсов.'
    },
    'DriveStatusNotSupported': {
        'errorMessage': 'Текущий статус физического накопителя не поддерживает данную операцию.'
    },
    'DriveListDuplicateValue': {
        'errorMessage': 'Список дубликатов физических накопителей.'
    },
    'RaidControllerLevelInvalid': {
        'errorMessage': 'Уровень RAID не поддерживается.'
    },
    'RAIDControllerNotSupportedOperation': {
        'errorMessage': 'Плата контроллера RAID не поддерживает данную операцию.'
    },
    'RAIDReachedVolumesNumberLimit': {
        'errorMessage': 'Достигнуто максимальное количество логических дисков.'
    },
    'SSDNotExist': {
        'errorMessage': 'Физический накопитель - не SSD.'
    },
    'PhysicalDiskIdCountError': {
        'errorMessage': 'Неверное количество физических накопителей.'
    },
    'PhysicalDiskIdInvalid': {
        'errorMessage': 'Неверный ID физического накопителя.'
    },
    'CryptoEraseNotSupported': {
        'errorMessage': 'Контроллер или физический накопитель не поддерживает данную операцию.'
    },
    'IncompleteForeignConfig': {
        'errorMessage': 'Внешняя конфигурация неполная.'
    },
    'NoForeignConfig': {
        'errorMessage': 'Не существует внешней конфигурации.'
    },
    'ResourceMissingAtURI': {
        'errorMessage': 'Сбой операции.'
    },
    'PropertyValueFormatError': {
        'errorMessage': 'Сбой операции.'
    },
    'FirmwareUploadError': {
        'errorMessage': 'Не удалось загрузить встроенное ПО. Пожалуйста, повторите попытку позже.'
    },
    'FirmwareUpgrading': {
        'errorMessage': 'Выполняется обновление. Подождите.'
    },
    'FirmwareNotMatchSignFile': {
        'errorMessage': 'Имя файла обновления не совпадает с именем файла подписи.'
    },
    'UploadFileOutRange': {
        'errorMessage': 'Размер файла выше допустимого.'
    },
    'EmergencyLoginUserSettingFail': {
        'errorMessage': 'Пользователь для экстренного входа должен использовать действующую учетную запись администратора.'
    },
    'ConflictWithLinuxDefaultUser': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Не удалось установить имя пользователя.'
    },
    'SpecifiedFileExist': {
        'errorMessage': 'Файл с таким именем уже существует. Измените имя файла и загрузите его еще раз.'
    },
    'ActionFailedByBbuExist': {
        'errorMessage': 'Принудительный перезапуск недействителен, если включена функция резервного питания.'
    },
    'ResetOperationNotAllowed': {
        'errorMessage': 'Сбой операции. Эта операция не поддерживается в текущем статусе. Повторите попытку позже.'
    },
    'TrapV3loginUser': {
        'errorMessage': 'Сбой операции.',
        'attrMessage': 'Если функция Trap включена, имя пользователя SNMP v3 trap изменить нельзя.'
    },
    'CertificateExpired': {
        'errorMessage': 'Срок действия сертификата истек.'
    },
    'SameRWCommunityName': {
        'errorMessage': 'Имя сообщества чтения-записи не может совпадать с именем сообщества только для чтения.'
    },
    'SameROCommunityName': {
        'errorMessage': 'Имя сообщества только для чтения не может совпадать с именем сообщества чтения-записи.'
    },
    'RWCommunitySimilarWithHistory': {
        'errorMessage': 'Новый и старый пароли имени сообщества чтения-записи должны иметь как минимум два разных символа.'
    },
    'ROCommunitySimilarWithHistory': {
        'errorMessage': 'Новый и старый пароли имени сообщества только для чтения должны иметь как минимум два разных символа.'
    },
    'TwoFactorCertificationFailedWithTLS': {
        'errorMessage': 'Сбой операции. Убедитесь, что TLS 1.2 включен в разделе «Управление безопасностью».'
    },
    'UserLocked': {
        'errorMessage': 'Аккаунт заблокирован.'
    },
    'IPv4AddressConflict': {
        'errorMessage': 'Сегмент IPv4-адреса конфликтует с другой сетью.'
    },
    'SystemLockdownForbid': {
        'errorMessage': 'Не удалось изменить пароль, так как режим блокировки системы включен. Пожалуйста, свяжитесь с администратором.'
    },
    'TLSFailedWithTwoFactorCertification': {
        'errorMessage': 'Двухфакторная аутентификация включена, TLS 1.2 невозможно выключить.'
    },
    'CrlIssuingDateInvalid': {
        'errorMessage': 'Не удалось импортировать список аннулированных сертификатов (CRL). CRL с более поздней датой выпуска уже существует.'
    },
    'LoginUserLocked': {
        'errorMessage': 'Аккаунт заблокирован. Свяжитесь с администратором.'
    },
    'LDInitInProgress': {
        'errorMessage': 'Сбой операции. Логический диск инициализируется.'
    },
    'NotSupportZhAndEnDisabled': {
        'errorMessage': 'Невозможно отключить китайский и английский языки.'
    },
    'OnlyOneLanguageEnabled': {
        'errorMessage': 'Данный язык нельзя отключить, потому что это единственный включенный язык.'
    },
    'LanguageNotEnabled': {
        'errorMessage': 'Язык уже отключен. Обновите страницу для получения последних данных.'
    },
    'LanguageNotSupport': {
        'errorMessage': 'Данный язык не поддерживает эту операцию.'
    },
    'CANotSupportCrlSignature': {
        'errorMessage': 'Корневые сертификаты не поддерживают подписи CRL.'
    },
    'LanguageNotDisabled': {
        'errorMessage': 'Язык уже включен. Обновите страницу для получения последних данных.'
    },
    'KeyFileTypeNotSupported': {
        'errorMessage': 'Этот тип файла ключей не поддерживается.'
    },
    'BodyExceedsMaxLength': {
        'errorMessage': 'Длина информации об активах достигает верхнего предела. Рекомендуемая длина не превышает 300 символов.'
    },
    'EncryptedCertImportFailed': {
        'errorMessage': 'Не удалось импортировать сертификат шифрования.'
    },
    'EncryptionAlgorithmIsWeak': {
        'errorMessage': 'Сертификат со слабым алгоритмом подписи (MD5) не поддерживается. Создайте другой сертификат.'
    },
    'EncryptionAlgorithmSHA1IsWeak': {
        'errorMessage': 'Сертификат со слабым алгоритмом подписи (SHA1) не поддерживается. Создайте другой сертификат.'
    },
    'ConfigurationExist': {
        'errorMessage': 'Не удалось выполнить операцию из-за существования логических дисков или расширенных физических дисков.'
    },
    'NoRestorePoint': {
        'errorMessage': 'Точка восстановления не существует.'
    },
    'CertificateAlreadyExists': {
        'errorMessage': 'Файл сертификата уже существует.'
    },
    'FileNameError': {
        'errorMessage': 'Некорректное имя файла.'
    },
    'NonSSDExist': {
        'errorMessage': 'Сбой операции. Группа RAID содержит диски, отличные от SSD.'
    },
    'PropertyModificationNotSupportedByBBUStatus': {
        'errorMessage': 'Сбой операции. Проверьте, установлен ли BBU на место и полностью ли заряжен.'
    },
    'VolumeShrinkNotAllowed': {
        'errorMessage': 'Сбой операции. Емкость логического диска невозможно уменьшить.'
    },
    'AssociatedVolumeCapacityOutRange': {
        'errorMessage': 'Сбой операции. Емкость связанного логического диска вне диапазона.'
    },
    'OperationFailed': {
        'errorMessage': 'This operation is not supported in the current state. Try again later.'
    },
    'ModifyfailedWithRCPNotInRange': {
        'errorMessage': 'Сбой операции. Убедитесь, что процент прочитанного кэша находится в заданном диапазоне.'
    }
};
