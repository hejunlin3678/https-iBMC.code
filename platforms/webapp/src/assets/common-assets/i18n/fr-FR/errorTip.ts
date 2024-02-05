export const errorMessage = {
    'ServiceException': {
        'errorMessage': 'Le service a expiré. Veuillez-vous connecter à nouveau.'
    },
    'InvalidSession': {
        'errorMessage': 'Session non valide. Veuillez vous reconnecter.'
    },
    'SessionTimeout': {
        'errorMessage': 'Votre session de connexion a expiré. Veuillez vous reconnecter.'
    },
    'NoValidSession': {
        'errorMessage': 'Votre session de connexion a expiré. Veuillez vous reconnecter.'
    },
    'SessionKickout': {
        'errorMessage': 'Vous avez été déconnecté par l\'administrateur système.'
    },
    'SessionRelogin': {
        'errorMessage': 'Un utilisateur est déjà connecté à votre compte depuis un autre périphérique.'
    },
    'SessionChanged': {
        'errorMessage': 'Les informations utilisateur ont été modifiées. Veuillez vous reconnecter.'
    },
    'InsufficientPrivilege': {
        'errorMessage': 'Vous n\'êtes pas autorisé à effectuer cette opération.'
    },
    'PropertyModificationNeedPrivilege': {
        'errorMessage': 'Vous n\'êtes pas autorisé à effectuer cette opération.'
    },
    'AuthorizationFailed': {
        'attrMessage': 'Mot de passe incorrect ou compte utilisateur verrouillé.'
    },
    'AuthorizationUserNoAccess': {
        'attrMessage': 'L\'utilisateur ne dispose d\'aucun accès.'
    },
    'AuthorizationUserPasswordExpired': {
        'attrMessage': 'Le mot de passe a expiré.'
    },
    'AuthorizationUserRestricted': {
        'attrMessage': 'Connexion restreinte.'
    },
    'AuthorizationUserLocked': {
        'attrMessage': 'Votre compte a été verrouillé.'
    },
    'InvalidUserName': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Mot de passe incorrect ou compte utilisateur verrouillé.'
    },
    'ResourceAlreadyExists': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'L\'utilisateur existe déjà.'
    },
    'UserNameIsRestricted': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Le nom d\'utilisateur est restreint.'
    },
    'InvalidPasswordLength': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Mot de passe non valide.'
    },
    'InvalidPassword': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Mot de passe non valide.'
    },
    'PasswordComplexityCheckFail': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Le mot de passe ne répond pas aux exigences de complexité du mot de passe.'
    },
    'PasswordInWeakPWDDict': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Ne peut pas être ajouté au dictionnaire de mots de passe faibles.'
    },
    'UserIsLoggingIn': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'L\'utilisateur s\'est connecté au système via SSH et ne peut pas être supprimé.'
    },
    'PasswordNotSatisfyComplexity': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Ajoutez deux nouveaux caractères minimum par rapport au mot de passe précédent.'
    },
    'PropertyValueExceedsMaxLength': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Mot de passe non valide.'
    },
    'DuringMinimumPasswordAge': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'La durée de vie du mot de passe est une durée minimum et ne peut être modifiée.'
    },
    'EmergencyLoginUser': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'L\'utilisateur de connexion d\'urgence ne peut pas être changé.'
    },
    'CannotDisableLastAdministrator': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Le dernier administrateur ne peut pas être désactivé.'
    },
    'CannotDeleteLastAdministrator': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Le dernier administrateur ne peut pas être supprimé.'
    },
    'InvalidPasswordSameWithHistory': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Le mot de passe précédent ne peut pas être utilisé.'
    },
    'AccountForbidRemoved': {
        'errorMessage': 'L\'utilisateur est un utilisateur de connexion d\'urgence ou un utilisateur SNMPv3 trap et ne peut pas être supprimé.'
    },
    'PublicKeyNotExist': {
        'errorMessage': 'Le fichier de clé publique n\'existe pas.'
    },
    'PublicKeyImportError': {
        'errorMessage': 'Format de la clé publique incorrect'
    },
    'DumpExportingErr': {
        'errorMessage': 'Tâche en cours... Patientez.'
    },
    'DifferentDriveSectorsSize': {
        'errorMessage': 'Les tailles des secteurs des lecteurs physiques sélectionnés sont différentes.'
    },
    'InsufficientMemoryErr': {
        'errorMessage': 'Impossible de collecter les données. Mémoire insuffisante.'
    },
    'UpgradingErr': {
        'errorMessage': 'Mise à niveau en cours… Veuillez réessayer plus tard.'
    },
    'DisableScreenShotInCurrentState': {
        'errorMessage': 'La capture d\'écran manuelle n\'est pas prise en charge lorsque le KVM est en mode privé.'
    },
    'ActivePsNotNecessary': {
        'errorMessage': 'Vous n\'avez pas besoin de définir le bloc d\'alimentation actif.'
    },
    'ActivePsNecessary': {
        'errorMessage': 'Sélectionnez le PSU actif.'
    },
    'ActivePsNotPosition': {
        'errorMessage': 'Aucun bloc d\'alimentation actif n\'est détecté.'
    },
    'NoStandbyPsPosition': {
        'errorMessage': 'Aucune alimentation de secours n\'est détectée.'
    },
    'ActivePsNotHealth': {
        'errorMessage': 'Le PSU actif/en veille est endommagé.'
    },
    'CurrentPowerHigh': {
        'errorMessage': 'La puissance actuelle est 75 % supérieure à la puissance de tous les PSU actifs.'
    },
    'ActivePsNotEnough': {
        'errorMessage': 'Les PSU actifs sont insuffisants.'
    },
    'PsNotSupport': {
        'errorMessage': 'Les blocs d\'alimentation ne prennent pas en charge le mode actif/veille.'
    },
    'ActiveStandbyModeUnsupport': {
        'errorMessage': 'Le mode actif/en veille n\'est pas pris en charge.'
    },
    'DuplicateExportingErr': {
        'errorMessage': 'Téléchargement en cours.... Veuillez réessayer plus tard.'
    },
    'OperationInProcess': {
        'errorMessage': 'Une tâche de collecte de journaux est en cours d\'exécution. Réessayez plus tard.'
    },
    'ResetOperationFailed': {
        'errorMessage': 'Mise à niveau du BIOS…'
    },
    'ResetOperationFailedVRD': {
        'errorMessage': 'Mise à niveau du VRD…'
    },
    'FileNotExist': {
        'errorMessage': 'Le fichier vidéo n\'existe pas.'
    },
    'DownFileNotExist': {
        'errorMessage': 'Le fichier à télécharger n\'existe pas.'
    },
    'RecordExceedsMaxNum': {
        'errorMessage': 'Le nombre d\'enregistrements de travail a atteint son maximum.'
    },
    'InvalidRecordId': {
        'errorMessage': 'Le fichier vidéo n\'existe pas.'
    },
    'KeyImportError': {
        'errorMessage': 'Format de clé incorrect.'
    },
    'CertificateImportingErr': {
        'errorMessage': 'Téléchargement du certificat en cours… Veuillez réessayer plus tard.'
    },
    'FailedEnableTwoFactorCertification': {
        'errorMessage': 'Configurez les certificats racine et client pour au moins un utilisateur.'
    },
    'CertificateQtyReachLimit': {
        'errorMessage': 'Le nombre de certificats a atteint son maximum.'
    },
    'RootCertificateAlreadyExists': {
        'errorMessage': 'Le fichier existe déjà.'
    },
    'RootCertificateNotExist': {
        'errorMessage': 'Le certificat n\'existe pas.'
    },
    'ClientCertLackProperties': {
        'errorMessage': 'N\'a pas réussi à importer le certificat client utilisé pour l\'authentification à deux facteurs parce que le certificat ne possède pas certaines propriétés.'
    },
    'ClientCertificateAlreadyExists': {
        'errorMessage': 'Le fichier existe déjà.'
    },
    'ClientCertIssuerMismatch': {
        'errorMessage': 'N\'a pas réussi à trouver le certificat racine.'
    },
    'ClientCertQtyExceedLimit': {
        'errorMessage': 'Le nombre de certificats a atteint son maximum.'
    },
    'ClientCertificateNotExist': {
        'errorMessage': 'Le certificat n\'existe pas.'
    },
    'CertificateFormatError': {
        'errorMessage': 'Format de certificat incorrect.'
    },
    'RootCertLackProperties': {
        'errorMessage': 'Le certificat n\'est pas un certificat racine.'
    },
    'RootCertificateMismatch': {
        'errorMessage': 'Le CRL ne se correspond pas au certificate racine.'
    },
    'RootCertificateNotImported': {
        'errorMessage': 'Le CRL ne se correspond pas au certificate racine.'
    },
    'CrlFileFormatError': {
        'errorMessage': 'Fichier CRL non valide ou fichier non codé en Base64.'
    },
    'RootCertificateIdInvalid': {
        'errorMessage': 'ID du certificate racine invalide.'
    },
    'MinPwdAgeAndPwdValidityRestrictEachOther': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'L\'âge minimum du mot de passe doit être inférieur de 10 jours par rapport à sa période de validité.'
    },
    'DisableKVMEncryptionFailed': {
        'errorMessage': 'Le cryptage KVM ne peut être désactivé qu\'une fois le cryptage VMM désactivé et enregistré.'
    },
    'KvmInUse': {
        'errorMessage': 'Le KVM est en cours d\'utilisation. Veuillez réessayer plus tard.'
    },
    'CommunityNameInWeakPWDDict': {
        'errorMessage': 'Ne peut pas être ajouté au dictionnaire de mots de passe faibles.'
    },
    'CommunityNameNotContainSpace': {
        'errorMessage': 'Ne répond pas aux exigences de complexité du mot de passe.'
    },
    'SimpleCommunityName': {
        'errorMessage': 'Ne répond pas aux exigences de complexité du mot de passe.'
    },
    'EnableVMMEncryptionFailed': {
        'errorMessage': 'Le cryptage VMM ne peut être activé qu\'une fois le cryptage KVM activé et enregistré.'
    },
    'VncInUse': {
        'errorMessage': 'Le VNC est en cours d\'utilisation. Veuillez réessayer plus tard.'
    },
    'CSRGeneratingErr': {
        'errorMessage': 'Génération de la CSR en cours… Veuillez réessayer plus tard.'
    },
    'CSRExportingErr': {
        'errorMessage': 'Génération de la CSR en cours… Veuillez réessayer plus tard.'
    },
    'CertMismatch': {
        'errorMessage': 'Le certificat à importer ne correspond pas au fichier CSR généré.'
    },
    'V3UserNameNotUsed': {
        'errorMessage': 'Opération a échoué.'
    },
    'InternalError': {
        'errorMessage': 'Opération a échoué.'
    },
    'NotSupportZhAndEnUninstalled': {
        'errorMessage': 'Les modules linguistiques chinois et anglais ne peuvent pas être supprimés.'
    },
    'OnlyOneLanguageInstalled': {
        'errorMessage': 'C\'est la seule langue utilisée par le système et ne peut pas être supprimée.'
    },
    'LanguageNotInstalled': {
        'errorMessage': 'Le pack de langue est déjà désinstallé. Actualiser la page pour obtenir les dernières données.'
    },
    'LicenseInstalling': {
        'errorMessage': 'Installation de la licence en cours… Veuillez réessayer plus tard.'
    },
    'CertImportFailed': {
        'errorMessage': 'Échec de l\'importation du certificat.'
    },
    'LicenseVerifyFailed': {
        'errorMessage': 'Erreur de vérification de licence.'
    },
    'LicenseActivateFailed': {
        'errorMessage': 'L\'activation de la licence a échoué.'
    },
    'LicenseExporting': {
        'errorMessage': 'Exportation de la licence en cours… Veuillez réessayer plus tard.'
    },
    'LicenseNotInstall': {
        'errorMessage': 'Aucune licence n\'est installée.'
    },
    'PortIdModificationFailed': {
        'errorMessage': 'Le port est en service.'
    },
    'PropertyUnknown': {
        'errorMessage': 'Opération a échoué.'
    },
    'MalformedJSON': {
        'errorMessage': 'Opération a échoué.'
    },
    'IncorrectSenderInfo': {
        'errorMessage': 'Le nom d\'utilisateur, le mot de passe ou l\'adresse de l\'expéditeur sont incorrects.'
    },
    'SmtpConnectionFailed': {
        'errorMessage': 'Impossible de se connecter au serveur SMTP ou le serveur SMTP ne prend pas en charge la connexion anonyme.'
    },
    'FeatureDisabledAndNotSupportOperation': {
        'errorMessage': 'L\'état actuel ne prend pas en charge cette opération.'
    },
    'TrapMemberIDNotExist': {
        'errorMessage': 'Le test Trap a échoué car le service Trap ou le serveur Trap est désactivé ou l\'adresse du serveur Trap est vide.'
    },
    'SyslogTestFailed': {
        'errorMessage': 'Le test du journal système a échoué car le service de journal système est désactivé ou le serveur ou l\'adresse du serveur est vide.'
    },
    'SendingEmailFailed': {
        'errorMessage': 'Impossible d\'envoyer un e-mail ou l\'adresse du destinataire est incorrecte.'
    },
    'TrapSendTestErr': {
        'errorMessage': 'Le test Trap a échoué car le service Trap ou le serveur Trap est désactivé ou l\'adresse du serveur Trap est vide.'
    },
    'CurrentStatusNotSupportOperation': {
        'errorMessage': 'Opération a échoué.'
    },
    'RAIDControllerNotSupported': {
        'errorMessage': 'Opération a échoué.'
    },
    'PropertyNotSupportedModificationByCurrentStatus': {
        'errorMessage': 'Opération a échoué.'
    },
    'LocatedFailedByPDRebuilding': {
        'errorMessage': 'Opération a échoué.'
    },
    'PropertyModificationNotSupportedByPDStatus': {
        'errorMessage': 'Opération a échoué.'
    },
    'PropertyModificationFailedByRAIDlevelNotSupported': {
        'errorMessage': 'Opération a échoué.'
    },
    'PropertyModificationFailedByIterfaceConflict': {
        'errorMessage': 'Opération a échoué.'
    },
    'PropertyModificationFailedByMediaTypeConflict': {
        'errorMessage': 'L\'opération a échoué. Le type de média à ajouter est en conflit avec le type de média existant.'
    },
    'PropertyConflict': {
        'errorMessage': 'Opération a échoué.'
    },
    'ModifyfailedWithRAIDCardState': {
        'errorMessage': 'La gestion de la carte contrôleur RAID n\'est pas prête.'
    },
    'ModifyfailedWithCachecade': {
        'errorMessage': 'Opération a échoué.'
    },
    'ModifyfailedWithRaidControllerNotSupport': {
        'errorMessage': 'Opération a échoué.'
    },
    'ModifyfailedWithSSD': {
        'errorMessage': 'Opération a échoué.'
    },
    'InvalidVolumeName': {
        'errorMessage': 'Opération a échoué.'
    },
    ' CurrentStatusNotSupport': {
        'errorMessage': 'L\'état actuel ne prend pas en charge cette opération.'
    },
    'OperationNotSupported': {
        'errorMessage': 'Cette opération n\'est pas prise en charge.'
    },
    'StripeSizeError': {
        'errorMessage': 'La taille de la bande n\'est pas prise en charge.'
    },
    'VolumeCapacityOutRange': {
        'errorMessage': 'La capacité du lecteur logique est hors de portée.'
    },
    'ArraySpaceInsufficient': {
        'errorMessage': 'Espace réseau insuffisant.'
    },
    'ArraySpaceNoFree': {
        'errorMessage': 'Le tableau n\'a pas d\'espace disponible.'
    },
    'NumberofDrivesForEachSpanInvalid': {
        'errorMessage': 'Le nombre de portées est invalide.'
    },
    'NumberOfSpansInvalid': {
        'errorMessage': 'Le nombre de lecteurs physiques ne répond pas aux exigences de service.'
    },
    'StorageMediaInconsistent': {
        'errorMessage': 'Les lecteurs physiques ont différents types de supports.'
    },
    'DriveInterfacesInconsistent': {
        'errorMessage': 'Les lecteurs physiques ont différents types d\'interfaces.'
    },
    'DriveStatusNotSupported': {
        'errorMessage': 'L\'état actuel du lecteur physique ne prend pas en charge cette opération.'
    },
    'DriveListDuplicateValue': {
        'errorMessage': 'Dupliquer la liste des lecteurs physiques.'
    },
    'RaidControllerLevelInvalid': {
        'errorMessage': 'Le niveau RAID n\'est pas pris en charge.'
    },
    'RAIDControllerNotSupportedOperation': {
        'errorMessage': 'La carte contrôleur RAID ne prend pas en charge cette opération.'
    },
    'RAIDReachedVolumesNumberLimit': {
        'errorMessage': 'Le nombre de lecteurs logiques a atteint son maximum.'
    },
    'SSDNotExist': {
        'errorMessage': 'Le lecteur physique n\'est pas un SSD.'
    },
    'PhysicalDiskIdCountError': {
        'errorMessage': 'Le nombre de lecteurs physiques est incorrect.'
    },
    'PhysicalDiskIdInvalid': {
        'errorMessage': 'L\'ID du lecteur physique est invalide.'
    },
    'CryptoEraseNotSupported': {
        'errorMessage': 'Le contrôleur ou le lecteur physique ne prend pas en charge cette opération.'
    },
    'IncompleteForeignConfig': {
        'errorMessage': 'La configuration étrangère est incomplète.'
    },
    'NoForeignConfig': {
        'errorMessage': 'La configuration étrangère n\'existe pas.'
    },
    'ResourceMissingAtURI': {
        'errorMessage': 'Opération a échoué.'
    },
    'PropertyValueFormatError': {
        'errorMessage': 'Opération a échoué.'
    },
    'FirmwareUploadError': {
        'errorMessage': 'Impossible de télécharger le firmware. Veuillez réessayer plus tard.'
    },
    'FirmwareUpgrading': {
        'errorMessage': 'Mise à jour en cours...'
    },
    'FirmwareNotMatchSignFile': {
        'errorMessage': 'Le nom du fichier de mise à niveau ne correspond pas à celui du fichier de signature.'
    },
    'UploadFileOutRange': {
        'errorMessage': 'La taille du fichier dépasse la limite.'
    },
    'EmergencyLoginUserSettingFail': {
        'errorMessage': 'L\'utilisateur de connexion d\'urgence doit être un administrateur valide.'
    },
    'ConflictWithLinuxDefaultUser': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Impossible de définir le nom d\'utilisateur.'
    },
    'SpecifiedFileExist': {
        'errorMessage': 'Un fichier portant le même nom existe déjà. Modifiez le nom du fichier et téléchargez à nouveau le fichier.'
    },
    'ActionFailedByBbuExist': {
        'errorMessage': 'Le redémarrage forcé n\'est pas valable lorsque la fonction d\'alimentation de secours est activée.'
    },
    'IPv4AddressConflict': {
        'errorMessage': 'Le segment d\'adresse IPv4 est en conflit avec un autre réseau.'
    },
    'SystemLockdownForbid': {
        'errorMessage': 'Impossible de changer le mot de passe parce que le verrouillage du système est activé. Veuillez contacter l\'administrateur.'
    },
    'ResetOperationNotAllowed': {
        'errorMessage': 'Échec de l\'opération. Cette opération n\'est pas prise en charge dans l\'état actuel. Réessayez ultérieurement.'
    },
    'CertificateExpired': {
        'errorMessage': 'Le certificat a expiré.'
    },
    'CustomFirmwareUploadError': {
        'errorTips': 'Le téléchargement de la clé publique a échoué. Veuillez réessayer plus tard.'
    },
    'UserLdapAuthFail': {
        'errorTips': 'Nom d\'utilisateur ou mot de passe incorrect. Veuillez le saisir à nouveau. Si la connexion échoue toujours, vérifiez que la version TLS du serveur LDAP n\'est pas antérieure à la version 1.2.'
    },
    'SameRWCommunityName': {
        'errorMessage': 'Le nom de la communauté en lecture-écriture ne peut pas être le même que celui de la communauté en lecture seule.'
    },
    'SameROCommunityName': {
        'errorMessage': 'Le nom de la communauté en lecture seule ne peut pas être le même que celui de la communauté en lecture-écriture.'
    },
    'RWCommunitySimilarWithHistory': {
        'errorMessage': 'Le nouveau et l\'ancien mots de passe de la communauté en lecture-écriture doivent contenir au moins deux caractères différents.'
    },
    'ROCommunitySimilarWithHistory': {
        'errorMessage': 'Le nouveau et l\'ancien mots de passe de la communauté en lecture seule doivent contenir au moins deux caractères différents.'
    },
    'TwoFactorCertificationFailedWithTLS': {
        'errorMessage': 'L\'opération a échoué. Veuillez vérifier que TLS 1.2 est activé sur la Gestion de la sécurité.'
    },
    'TLSFailedWithTwoFactorCertification': {
        'errorMessage': 'L\'authentification à 2 facteurs est activée, et TLS 1.2 ne peut pas être désactivé.'
    },
    'CrlIssuingDateInvalid': {
        'errorMessage': 'Impossible d\'importer la liste de révocation de certificat (CRL). Une CRL d\'une date ultérieure existe déjà.'
    },
    'TrapV3loginUser': {
        'errorMessage': 'Opération a échoué.',
        'attrMessage': 'Lorsque la fonction Trap est activée, les noms d\'utilisateur de Trap SNMPv3 ne peuvent pas être modifiés.'
    },
    'SessionLimitExceeded': {
        'errorMessage': 'Le nombre maximum de sessions a été atteint.'
    },
    'UserLocked': {
        'errorMessage': 'Votre compte a été verrouillé.'
    },
    'SameCommunityName': {
        'errorMessage': 'Le nom de la communauté de lecture seule ne peut pas être le même que celui de la communauté en lecture-écriture.'
    },
    'LoginUserLocked': {
        'errorMessage': 'Votre compte a été verrouillé. Contactez l\'administrateur.'
    },
    'LDInitInProgress': {
        'errorMessage': 'L\'opération a échoué parce que le lecteur logique est en cours d\'initialisation.'
    },
    'NotSupportZhAndEnDisabled': {
        'errorMessage': 'Les langues chinoise et anglaise ne peuvent pas être désactivées.'
    },
    'OnlyOneLanguageEnabled': {
        'errorMessage': 'Cette langue ne peut pas être désactivée, car elle est la seule langue activée.'
    },
    'LanguageNotEnabled': {
        'errorMessage': 'La langue a été désactivée. Rafraîchissez la page pour obtenir les dernières données.'
    },
    'LanguageNotSupport': {
        'errorMessage': 'Cette langue ne prend pas en charge cette opération.'
    },
    'CANotSupportCrlSignature': {
        'errorMessage': 'Les certificats racine ne prennent pas en charge la signature CRL.'
    },
    'LanguageNotDisabled': {
        'errorMessage': 'La langue est déjà activée. Rafraîchissez la page pour obtenir les dernières données.'
    },
    'KeyFileTypeNotSupported': {
        'errorMessage': 'Ce type de fichier clé n\'est pas pris en charge.'
    },
    'BodyExceedsMaxLength': {
        'errorMessage': 'La longueur des informations sur le bien atteint la limite supérieure. La longueur recommandée est de 300 caractères.'
    },
    'EncryptedCertImportFailed': {
        'errorMessage': 'Échec de l\'importation du certificat de cryptage.'
    },
    'EncryptionAlgorithmIsWeak': {
        'errorMessage': 'Le certificat avec un algorithme de signature faible (MD5) n\'est pas pris en charge. Générez un autre certificat.'
    },
    'EncryptionAlgorithmSHA1IsWeak': {
        'errorMessage': 'Le certificat avec un algorithme de signature faible (SHA1) n\'est pas pris en charge. Générez un autre certificat.'
    },
    'ConfigurationExist': {
        'errorMessage': 'L\'opération a échoué parce qu\'il existe des lecteurs logiques ou des lecteurs physiques améliorés.'
    },
    'NoRestorePoint': {
        'errorMessage': 'Le point de restauration n\'existe pas.'
    },
    'CertificateAlreadyExists': {
        'errorMessage': 'Le fichier de certificat existe déjà.'
    },
    'FileNameError': {
        'errorMessage': 'Nom de fichier incorrect.'
    },
    'NonSSDExist': {
        'errorMessage': 'L\'opération a échoué. Le groupe RAID contient des disques de type non SSD.'
    },
    'PropertyModificationNotSupportedByBBUStatus': {
        'errorMessage': 'Opération échouée. Vérifiez si la BBU est en position et entièrement chargée.'
    },
    'VolumeShrinkNotAllowed': {
        'errorMessage': 'Opération échouée. La capacité du lecteur logique ne peut pas être réduite.'
    },
    'AssociatedVolumeCapacityOutRange': {
        'errorMessage': 'Opération échouée. La capacité du lecteur logique associé est hors de la plage.'
    },
    'OperationFailed': {
        'errorMessage': 'Cette opération n\'est pas prise en charge dans l\'état actuel. Réessayez plus tard.'
    },
    'ModifyfailedWithRCPNotInRange': {
        'errorMessage': 'Operation failed. Ensure that the read cache percentage is within the valid range.'
    }
};
