
#ifndef REDFISH_PROVIDER_SYSTEMS
#define REDFISH_PROVIDER_SYSTEMS

#define RFPROP_SYSTEM_BIOS "Bios"
#define RF_BIOS_ODATA_ID "/redfish/v1/Systems/%s/Bios"
#define RF_BIOS_ODATA_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/Bios/$entity"

#define RF_BIOS_REG_VERSION "AttributeRegistry"
#define RFPROP_EFFECTIVE_STATUS "EffectiveStatus"
#define BIOS_CURRENTVALUE_FILE_NAME "/opt/pme/conf/bios/currentvalue.json"

#define RFPROP_SYSTEM_BIOS_ACTION_RESET "Bios.ResetBios"
#define RF_BIOS_ACTION_RESET_TARGET "/redfish/v1/Systems/%s/Bios/Actions/Bios.ResetBios"
#define RF_BIOS_ACTION_RESET_ACTION_INFO "/redfish/v1/Systems/%s/Bios/ResetBiosActionInfo"

#define RFPROP_SYSTEM_BIOS_ACTION_CHANGE_PASSWROD "Bios.ChangePassword"
#define RF_BIOS_OLD_PASSWORD    "OldPassword"
#define RF_BIOS_NEW_PASSWORD    "NewPassword"
#define RF_BIOS_PASSWORD_NAME   "PasswordName"
#define RF_BIOS_ACTION_CHANGE_PASSWORD_TARGET "/redfish/v1/Systems/%s/Bios/Actions/Bios.ChangePassword"
#define RF_BIOS_ACTION_CHANGE_PASSWORD_ACTION_INFO "/redfish/v1/Systems/%s/Bios/ChangePasswordActionInfo"

#define RFPROP_SETTINGS "@Redfish.Settings"
#define RF_BIOS_SETTINGS_ODATA_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/Bios/Settings/$entity"
#define RF_BIOS_SETTINGS_ODATA_ID "/redfish/v1/Systems/%s/Bios/Settings"
#define RFACTION_PARAM_BIOS_REVOKE "#Settings.Revoke"
#define RFACTION_BIOS_REVOKE "Oem/Huawei/Settings.Revoke"
#define RFACTION_BIOS_REVOKE_TARGET "/redfish/v1/Systems/%s/Bios/Settings/Actions/Oem/Huawei/Settings.Revoke"
#define RF_BIOS_REVOKE_SETTING_ACTION_INFO "/redfish/v1/Systems/%s/Bios/Settings/SettingsRevokeActionInfo"
#define RF_BIOS_POLICYCONFIG_ODATA_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/Bios/PolicyConfig/Settings/$entity"
#define RF_BIOS_POLICYCONFIG_ODATA_ID "/redfish/v1/Systems/%s/Bios/PolicyConfig/Settings"
#define RF_BIOS_POLICYCFG_REG_ODATA_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/Bios/PolicyConfig/$entity"
#define RF_BIOS_POLICYCFG_REG_ODATA_ID "/redfish/v1/Systems/%s/Bios/PolicyConfig"

#define RF_SYSTEM_BIOS_OEM_HUAWEI_ODATA_ID "/redfish/v1/Systems/%s/Bios/Oem/Huawei/%s"

#define RFPROP_CERTIFICATES                                     "Certificates"
#define RFPROP_BOOT_CERTIFICATES                                "BootCertificates"
#define RFPROP_BIOS_CERTIFICATE_STRING                          "CertificateString"
#define RFPROP_BIOS_CERTIFICATE_TYPE                            "CertificateType"
#define RFPROP_BIOS_RESETKEYS_TYPE                              "ResetKeysType"
#define RFPROP_BIOS_DATABASE                                    "Database"
#define RFPROP_BOOT_CERTIFICATES_ACTION_IMPORT_BOOT_CERT        "BootCertificates.ImportBootCert"
#define RFPROP_BOOT_CERTIFICATES_ACTION_IMPORT_BOOT_CRL         "BootCertificates.ImportBootCrl"
#define RFPROP_BOOT_CERTIFICATES_ACTION_RESET_BOOT_CERT         "BootCertificates.ResetBootCert"
#define RFPROP_BOOT_CERTIFICATES_ACTION_RESET_BOOT_CRL          "BootCertificates.ResetBootCrl"
#define RFPROP_BOOT_CERTIFICATES_ACTION_IMPORT_SECUREBOOT_CERT  "BootCertificates.ImportSecureBootCert"
#define RFPROP_BOOT_CERTIFICATES_ACTION_RESET_SECUREBOOT_CERT   "BootCertificates.ResetSecureBootCert"

#define RF_BIOS_OEM_BOOT_CERTIFICATES_ODATA_ID "/redfish/v1/Systems/%s/Bios/Oem/Huawei/BootCertificates"

#define RF_BIOS_IMPORT_BOOT_CERT_ACTION_INFO   \
    "/redfish/v1/Systems/%s/Bios/Oem/Huawei/BootCertificates/ImportBootCertActionInfo"
#define RF_BIOS_IMPORT_BOOT_CRL_ACTION_INFO    \
    "/redfish/v1/Systems/%s/Bios/Oem/Huawei/BootCertificates/ImportBootCrlActionInfo"
#define RF_BIOS_RESET_BOOT_CERT_ACTION_INFO    \
    "/redfish/v1/Systems/%s/Bios/Oem/Huawei/BootCertificates/ResetBootCertActionInfo"
#define RF_BIOS_RESET_BOOT_CRL_ACTION_INFO     \
    "/redfish/v1/Systems/%s/Bios/Oem/Huawei/BootCertificates/ResetBootCrlActionInfo"
#define RF_IMPORT_SECUREBOOT_CERT_ACTION_INFO \
    "/redfish/v1/Systems/%s/Bios/Oem/Huawei/BootCertificates/ImportSecureBootCertActionInfo"
#define RF_RESET_SECUREBOOT_CERT_ACTION_INFO \
    "/redfish/v1/Systems/%s/Bios/Oem/Huawei/BootCertificates/ResetSecureBootCertActionInfo"


#define VOLUME_READ_POLICY_NOREADAHEAD "NoReadAhead"
#define VOLUME_READ_POLICY_READAHEAD "ReadAhead"
#define VOLUME_READ_POLICY_READAHEAD_ADAPTIVE "ReadAheadAdaptive"

#define VOLUME_WRITE_POLICY_WRITETHROUGH "WriteThrough"
#define VOLUME_WRITE_POLICY_WRITEBACKWITHBBU "WriteBackWithBBU"
#define VOLUME_WRITE_POLICY_WRITEBACK "WriteBack"
#define VOLUME_WRITE_POLICY_READONLY  "ReadOnly"

#define VOLUME_CACHE_POLICY_CACHEIO "CachedIO"
#define VOLUME_CACHE_POLICY_DRECTIO "DirectIO"

#define VOLUME_ACCESS_POLICY_READWRITE "ReadWrite"
#define VOLUME_ACCESS_POLICY_READONLY "ReadOnly"
#define VOLUME_ACCESS_POLICY_BLOCKED "Blocked"
#define VOLUME_ACCESS_POLICY_HIDDEN "Hidden"

#define VOLUME_DISK_CACHE_POLICY_UNCHANGED "Unchanged"
#define VOLUME_DISK_CACHE_POLICY_ENABLE "Enabled"
#define VOLUME_DISK_CACHE_POLICY_DISABLE "Disabled"

#define RFPROP_POWER_ON_AFTER_CPU_THERMALTRIP ("PowerOnAfterCpuThermalTrip")
#define RFPROP_DELAY_SECONDS_AFTER_CPU_THERMALTRIP ("DelaySecondsAfterCpuThermalTrip")
#define RFPROP_POWER_ON_AFTER_CPU_THERMALTRIP_URI ("Oem/Huawei/PowerOnAfterCpuThermalTrip")
#define RFPROP_DELAY_SECONDS_AFTER_CPU_THERMALTRIP_URI ("Oem/Huawei/DelaySecondsAfterCpuThermalTrip")

gint32 get_system_memory_temperature(OBJ_HANDLE obj_handle, gchar *prop, gchar *rf_prop, json_object *huawei);
// 获取操作系统上电状态的通用函数
gint32 rf_get_system_powerstate(guchar *power_state);
gboolean cpu_is_present(OBJ_HANDLE obj_handle);
gboolean npu_is_present(OBJ_HANDLE obj_handle);
gboolean gpu_is_present(OBJ_HANDLE obj_handle);
gint32 get_npu_rsc_link(OBJ_HANDLE obj_handle, char *rsc_link, size_t link_len, const gchar* slot_id);
gint32 get_gpu_rsc_link(OBJ_HANDLE obj_handle, char *rsc_link, size_t link_len, const gchar* slot_id);
gint32 get_processor_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
void get_gpu_oem_huawei(OBJ_HANDLE obj_handle, json_object* huawei_jso);
gchar *rfmemory_type_to_rftype(gchar *type, gint32 length);
gboolean redfish_check_system_uri_valid(gchar *uri_string);

gint32 get_bma_connect_state(PROVIDER_PARAS_S *i_paras);
gint32 get_process_memory_history_supported(PROVIDER_PARAS_S *i_paras);

gint32 system_bios_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 system_bios_policyconfig_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_bios_policyconfig_reg_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 bios_reset_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_bios_settings_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 revoke_biossetting_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_bios_oem_bootcert_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_bios_oem_bootcert_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 set_systems_storages_ld_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 set_systems_storages_ld_capacity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 set_systems_storages_ld_optimum_io_size(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
#endif 
