
#include "redfish_provider.h"
#include "redfish_provider_chassis.h"
#include "redfish_provider_system.h"
#include "redfish_provider_cert_update.h"
#include "redfish_provider_chassisoverview.h"
#include "redfish_provider_tpcm.h"
#include "redfish_provider_chassis_pciedevices.h"
#include "redfish_route.h"

LOCAL gsize g_resource_provider_info_size = 0;
LOCAL gsize g_hmm_resource_provider_info_size = 0;

LOCAL gint32 provider_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
LOCAL gint32 get_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL RESOURCE_PROVIDER_S g_resource_provider_info[] = {
    {"^/redfish/v1$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1", serviceroot_provider_entry, NULL, NULL},

    {"^/redfish/v1/systems$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/systems", system_provider_collection_entry, NULL, NULL},
    {"^/redfish/v1/chassis$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/chassis",    chassis_provider_collection_entry, get_chassis_collection_rsc, NULL},
    {"^/redfish/v1/managers$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers",  manager_provider_collection_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/storages$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/systems/1/storages", collection_storage_provider_entry, NULL, NULL},
    { "^/redfish/v1/systemoverview$", PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/systemoverview",          system_overview_provider_collection_entry,   NULL, NULL},
    {"^/redfish/v1/chassisoverview$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/chassisoverview", chassis_overview_provider_collection_entry, get_chassis_overview, NULL},
    {"^/redfish/v1/dataacquisitionservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/dataacquisitionservice", dataacquisition_provider_entry, get_data_acquisition_svc_rsc, NULL},
    {RACK_DA_CONFIG_PATTERN, PROVIDER_URI_PATTERN_DEFAULT_MODE, RACK_DA_CONFIG_RSC_PATH, rack_data_acquisition_cfg_provider_entry, NULL, NULL},
    {URI_PATTERN_DATA_DA_REPORT, PROVIDER_URI_PATTERN_DEFAULT_MODE, RSC_PATH_DATA_DA_REPORT, dataacquisition_report_provider_entry, get_data_acquisition_report, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/licenseservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/licenseservice", manager_licsrv_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/licenseservice/installlicenseactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/licenseservice/installlicenseactioninfo", licsrv_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/licenseservice/exportlicenseactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/licenseservice/exportlicenseactioninfo", licsrv_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/licenseservice/revokelicenseactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/licenseservice/revokelicenseactioninfo", licsrv_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/licenseservice/deletelicenseactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/licenseservice/deletelicenseactioninfo", licsrv_actioninfo_provider_entry, NULL, NULL},

    {"^/redfish/v1/accountservice/accounts/[a-zA-Z0-9]+/oem/huawei$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/accountservice/accounts/template", account_provider_oem_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/accounts/[a-zA-Z0-9]+/importmutualauthclientcertactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/accountservice/accounts/template/importmutualauthclientcertactioninfo", account_provider_action_info_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/accounts/[a-zA-Z0-9]+/deletemutualauthclientcertactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/accountservice/accounts/template/deletemutualauthclientcertactioninfo", account_provider_action_info_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/accounts/[a-zA-Z0-9]+/importsshpublickeyactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,           "/redfish/v1/accountservice/accounts/template/importsshpublickeyactioninfo",         account_provider_action_info_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/accounts/[a-zA-Z0-9]+/deletesshpublickeyactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,           "/redfish/v1/accountservice/accounts/template/deletesshpublickeyactioninfo",         account_provider_action_info_entry, NULL, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/dumpactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                  "/redfish/v1/managers/1/dumpactioninfo",        manager_provider_action_info_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/exportconfigurationactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/managers/1/exportactioninfo",      manager_provider_action_info_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/importconfigurationactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/managers/1/importactioninfo", manager_provider_action_info_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/poweronpermitactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,         "/redfish/v1/managers/1/poweractioninfo",       manager_provider_action_info_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/deletelanguageactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/managers/1/deletelanguageactioninfo",    manager_provider_action_info_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/restorefactoryactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/managers/1/restorefactoryactioninfo",    manager_provider_action_info_entry, NULL, NULL},
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/generaldownloadactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/generaldownloadactioninfo", manager_provider_action_info_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/setfusionpartitionactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/setfusionpartitionactioninfo", manager_provider_action_info_entry, NULL, NULL },

    {URI_PATTERN_MANAGER_RESET_ACTIONINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                  RSC_PATH_MANAGER_RESET_ACTIONINFO,             manager_provider_action_info_entry, NULL, NULL},
    {URI_PATTERN_MANAGER_ROLL_BACK_ACTIONINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,              RSC_PATH_MANAGER_ROLLBACK_ACTIONINFO,          manager_provider_action_info_entry, NULL, NULL},

    {"^/redfish/v1/accountservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                         "/redfish/v1/accountservice/",                  accountservice_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/ldapservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,           "/redfish/v1/accountservice/ldapservice",    ldapservice_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/ldapservice/ldapcontrollers$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/ldapservice/ldapcontrollers",    ldapservice_collection_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/ldapservice/ldapcontrollers/[0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/ldapservice/ldapcontrollers/template", ldapservice_controller_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/ldapservice/ldapcontrollers/[0-9]+/importcertactioninfo",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/accountservice/ldapservice/ldapcontrollers/template/importcertactioninfo", ldapservice_controller_provider_import_entry, NULL, NULL},
    { "^/redfish/v1/accountservice/ldapservice/ldapcontrollers/[0-9]+/importcrlactioninfo", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/accountservice/ldapservice/ldapcontrollers/template/importcrlactioninfo", ldapservice_controller_provider_import_crl_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/kerberosservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/kerberosservice",    kerberosservice_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/kerberosservice/kerberoscontrollers$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/kerberosservice/kerberoscontrollers",    kerberosservice_collection_provider_entry, NULL, NULL},

    {"^/redfish/v1/accountservice/kerberosservice/kerberoscontrollers/[a-zA-Z0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/kerberosservice/kerberoscontrollers/template", kerberos_controller_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/kerberosservice/kerberoscontrollers/[0-9]+/importkeytableactioninfo",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/accountservice/kerberosservice/kerberoscontrollers/template/actioninfo", kerberos_controller_provider_import_entry, NULL, NULL},

    {"^/redfish/v1/accountservice/importrootcertificateactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/importrootcertificateactioninfo/", accountservice_provider_import_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/deleterootcertificateactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/deleterootcertificateactioninfo/", accountservice_provider_delete_entry, NULL, NULL},

    {"^/redfish/v1/accountservice/importcrlactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/importcrlactioninfo/", accountservice_provider_import_crl_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/accounts$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                "/redfish/v1/accountservice/accounts",          accounts_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/accounts/[a-zA-Z0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/accountservice/accounts/template", account_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/roles$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                   "/redfish/v1/accountservice/roles",             roles_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/roles/Administrator$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
     "/redfish/v1/accountservice/roles/administrator",  role_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/roles/Operator$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/accountservice/roles/operator", role_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/roles/Commonuser$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/accountservice/roles/commonuser", role_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/roles/NoAccess$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/accountservice/roles/noaccess", role_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/roles/[a-zA-Z0-9]+$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/accountservice/roles/template", role_provider_entry, NULL, NULL},

    {"^/redfish/v1/sessionservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                         "/redfish/v1/sessionservice/",                  sessionservice_provider_entry, NULL, NULL},
    {"^/redfish/v1/sessionservice/sessions$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                "/redfish/v1/sessionservice/sessions",          session_collection_provider_entry, NULL, NULL},
    {"^/redfish/v1/sessionservice/sessions/[a-zA-Z0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/sessionservice/sessions/template", session_provider_entry, NULL, NULL},
    {"^/redfish/v1/sessionservice/sessions/[a-zA-Z0-9]+/refreshactivationstateactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/sessionservice/sessions/template/refreshactivationstateactioninfo", session_actioninfo_provider_entry, NULL, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,         "/redfish/v1/managers/1/",                      manager_provider_entry, NULL, manager_etag_del_property},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/syslogservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/syslogservice", manager_syslog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/syslogservice/importrootcertificateactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/syslogservice/importrootcertificateactioninfo", manager_syslog_import_root_cert_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/syslogservice/importclientcertificateactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/syslogservice/importclientcertificateactioninfo", manager_syslog_import_client_cert_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/syslogservice/submittesteventactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/syslogservice/submittesteventactioninfo", manager_syslog_submit_test_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/syslogservice/importcrlactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/syslogservice/importcrlactioninfo", manager_syslog_import_crl_actioninfo_entry, NULL, NULL },
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/syslogservice/deletecrlactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/syslogservice/deletecrlactioninfo", manager_syslog_delete_crl_actioninfo_entry, NULL, NULL },
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/nics$",                PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/nics",          manager_nic_collection_provider_entry, get_nic_collection_rsc, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/nics/[a-zA-Z0-9]+$",   PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/nics/template", manager_nic_provider_entry, get_nic_rsc, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/ethernetinterfaces$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,    "/redfish/v1/managers/1/ethernetinterfaces", manager_ethernetinterfaces_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/ethernetinterfaces/[a-zA-Z0-9\\.]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/ethernetinterfaces/template", manager_ethernetinterface_provider_entry, NULL, NULL},
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/lldpservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/1/lldpservice", manager_lldpservice_provider_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/usbmgmtservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
      "/redfish/v1/managers/1/usbmgmtservice", manager_usbmgmt_provider_entry, NULL, NULL },

    { "^/redfish/v1/managers/[a-zA-Z0-9]+/energysavingservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/1/EnergySavingService", manager_energysaving_provider_entry, NULL, NULL },

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/snmpservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                  "/redfish/v1/managers/1/snmpservice",    snmp_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/snmpservice/submittesteventactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                  "/redfish/v1/managers/1/snmpservice/submittesteventactioninfo",    snmp_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/snmpservice/configsnmpv3privpasswdactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                  "/redfish/v1/managers/1/snmpservice/configsnmpv3privpasswdactioninfo",    snmp_actioninfo_provider_entry, NULL, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/smsservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                  "/redfish/v1/managers/1/smsservice",    sms_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/smsservice/refreshinstallablebmaactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/smsservice/refreshinstallablebmaactioninfo",    sms_refresh_bma_action_info_entry, NULL, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/smtpservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/smtpservice",       manager_smtp_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/smtpservice/submittesteventactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/smtpservice/submittesteventactioninfo",       manager_smtp_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/networkprotocol$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/networkprotocol",       networkprotocol_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                              "/redfish/v1/systems/1/",                       system_provider_entry, NULL, systems_delete_etag_prop},


    {URI_PATTERN_SYSTEM_RESET_ACTIONINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                  RSC_PATH_SYSTEM_RESET_ACTIONINFO,               system_actioninfo_provider_entry, NULL, NULL},
    {URI_PATTERN_SYSTEM_FRU_CONTROL_ACTIONINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,            RSC_PATH_SYSTEM_FRU_CONTROL_ACTIONINFO,         system_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/Systems/[a-zA-Z0-9]+/ClearNetworkHistoryUsageRateActionInfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/systems/1/clearnetworkhistoryusagerateactioninfo",         provider_actioninfo_entry, NULL, NULL},

    {URI_PATTERN_SYSTEM_ADD_SWI_IPV6ADDR_ACTIONINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,            RSC_PATH_SYSTEM_ADD_SWI_IPV6ADDR_ACTIONINFO,         system_actioninfo_provider_entry, NULL, NULL},
    {URI_PATTERN_SYSTEM_DEL_SWI_IPV6ADDR_ACTIONINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,            RSC_PATH_SYSTEM_DEL_SWI_IPV6ADDR_ACTIONINFO,         system_actioninfo_provider_entry, NULL, NULL},
    {URI_PATTERN_SYSTEM_ADD_SWI_IPV6ROUTE_ACTIONINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,            RSC_PATH_SYSTEM_ADD_SWI_IPV6ROUTE_ACTIONINFO,         system_actioninfo_provider_entry, NULL, NULL},
    {URI_PATTERN_SYSTEM_DEL_SWI_IPV6ROUTE_ACTIONINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,            RSC_PATH_SYSTEM_DEL_SWI_IPV6ROUTE_ACTIONINFO,         system_actioninfo_provider_entry, NULL, NULL},

    {"^/redfish/v1/systems/[a-zA-Z0-9]+/memoryview$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/memoryview", system_memory_view_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/processorview$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/processorview", system_processor_view_provider_entry, NULL, system_process_view_etag_del_property},

    {"^/redfish/v1/systems/[a-zA-Z0-9]+/processors$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/processors", system_processors_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/processors/[a-zA-Z0-9]+(-[0-9]+)?$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/processors/template", system_processor_provider_entry, NULL, processor_etag_del_property},

    {"^/redfish/v1/systems/[a-zA-Z0-9]+/processorshistoryusagerate$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/processorshistoryusagerate", system_processors_usage_rate_provider_entry, NULL, NULL},

    {"^/redfish/v1/systems/[a-zA-Z0-9]+/digitalwarranty$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/systems/1/digitalwarranty", system_digital_warranty_provider_entry, NULL, NULL},

    {"^/redfish/v1/systems/[a-zA-Z0-9]+/storages/[a-zA-Z0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                   "/redfish/v1/systems/1/storages/template",             storage_provider_entry, NULL, NULL},

    {URI_PATTERN_STORAGE_RESTORE_CTRLER_DFLT_SET_ACTIONNINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,         RSC_PATH_STORAGE_RESTORE_CTRLER_DFLT_SET_ACTIONNINFO, storage_actioninfo_provider_entry, NULL, NULL},

    {URI_PATTERN_STORAGE_IMPORT_FOREIGN_CONFIG_ACTIONNINFO,    PROVIDER_URI_PATTERN_DEFAULT_MODE,
    RSC_PATH_STORAGE_IMPORT_FOREIGN_CONFIG_ACTIONNINFO,    storage_actioninfo_provider_entry, NULL, NULL},
    {URI_PATTERN_STORAGE_CLEAR_FOREIGN_CONFIG_ACTIONNINFO,     PROVIDER_URI_PATTERN_DEFAULT_MODE,
    RSC_PATH_STORAGE_CLEAR_FOREIGN_CONFIG_ACTIONNINFO,     storage_actioninfo_provider_entry, NULL, NULL},

    {"^/redfish/v1/systems/[a-zA-Z0-9]+/storages/[a-zA-Z0-9]+/volumes$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                   "/redfish/v1/systems/1/storages/template/volumes",             system_storage_lds_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/storages/[a-zA-Z0-9]+/volumes/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/systems/1/storages/template/volumes/template",    system_storage_ld_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/storages/[a-zA-Z0-9]+/volumes/[^/]+/initializeactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/systems/1/storages/template/volumes/template/initializeactioninfo", system_storage_ld_init_actioninfo_provider_entry, NULL, NULL},

    {"^/redfish/v1/systems/[a-zA-Z0-9]+/ethernetinterfaces$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,           "/redfish/v1/systems/1/ethernetinterfaces",     system_ethernetinterfaces_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/ethernetinterfaces/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/ethernetinterfaces/template", system_ethernetinterface_provider_entry, NULL, system_eth_etag_del_property},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/ethernetinterfaces/[^/]+/vlans$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/ethernetinterfaces/template/vlans", system_ethernetinterface_vlans_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/ethernetinterfaces/[^/]+/vlans/[0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/ethernetinterfaces/template/vlans/template", system_ethernetinterface_vlan_provider_entry, NULL, NULL},

    {"^/redfish/v1/systems/[a-zA-Z0-9]+/ethernetinterfaces/[^/]+/configureactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/ethernetinterfaces/template/configureactioninfo", system_ethernetinterface_acinfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/ethernetinterfaces/[^/]+/vlans/[0-9]+/configureactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/ethernetinterfaces/template/vlans/template/configureactioninfo", system_vlan_acinfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/networkbondings$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,           "/redfish/v1/systems/1/networkbondings",     system_networkbondings_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/networkbondings/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,    "/redfish/v1/systems/1/networkbondings/template", system_networkbonding_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/networkbondings/[^/]+/configureactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,    "/redfish/v1/systems/1/networkbondings/template/configureactioninfo", system_networkbonding_action_info_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/infinibandinterfaces$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,         "/redfish/v1/systems/1/infinibandinterfaces",     system_infinibands_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/infinibandinterfaces/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/systems/1/infinibandinterfaces/template", system_infiniband_provider_entry, NULL, NULL},

    {"^/redfish/v1/systems/[a-zA-Z0-9]+/networkbridge$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,              "/redfish/v1/systems/1/networkbridge",     system_network_bridge_collection_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/networkbridge/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,    "/redfish/v1/systems/1/networkbridge/template", system_network_bridge_provider_entry, NULL, NULL},

    {"^/redfish/v1/chassis/smm[0-9]+$",  PROVIDER_URI_SPECICAL_PATTERN_MODE,                                 "/redfish/v1/chassis/smm",                       chassis_provider_entry, get_chassis_rsc, chassis_etag_del_property},
    {"^/redfish/v1/chassis/drawer[0-9]+$",  PROVIDER_URI_SPECICAL_PATTERN_MODE,                              "/redfish/v1/chassis/drawer",                       chassis_provider_entry, get_chassis_rsc, chassis_etag_del_property},
    {"^/redfish/v1/chassis/iom[0-9]+$",  PROVIDER_URI_SPECICAL_PATTERN_MODE,                                 "/redfish/v1/chassis/iom",                       chassis_provider_entry, get_chassis_rsc, chassis_etag_del_property},
    {"^/redfish/v1/chassis/blade[0-9]+$",  PROVIDER_URI_SPECICAL_PATTERN_MODE,                               "/redfish/v1/chassis/blade",                       chassis_provider_entry, get_chassis_rsc, chassis_etag_del_property},
    {"^/redfish/v1/chassis/enc$",  PROVIDER_URI_SPECICAL_PATTERN_MODE,                                       "/redfish/v1/chassis/enc",                        chassis_provider_entry, get_chassis_rsc, chassis_etag_del_property},
    {"^/redfish/v1/chassis/rack$",  PROVIDER_URI_SPECICAL_PATTERN_MODE,                        "/redfish/v1/chassis/rack",      chassis_location_provider_entry, get_rack_rsc, NULL},

    {CHASSIS_SET_DIMENSIONS_ACTIONINFO_PATTERN, PROVIDER_URI_PATTERN_DEFAULT_MODE, CHASSIS_SET_DIMENSIONS_ACTIONINFO_RSC_PATH, chassis_set_dimensions_actioninfo_provider_entry, NULL, NULL},

    {"^/redfish/v1/chassis/rack/exportumarklabeltemplateactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,           "/redfish/v1/chassis/rack/exportumarklabeltemplateactioninfo", chassis_export_template_file_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/rack/importconfigfileactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,           "/redfish/v1/chassis/rack/importconfigfileactioninfo", chassis_import_config_file_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/U[0-9]+$",  PROVIDER_URI_SPECICAL_PATTERN_MODE,                                       "/redfish/v1/chassis/unit",                        chassis_provider_unit_entry, get_chassis_unit_rsc, chassis_etag_del_property},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+$",  PROVIDER_URI_WILDCARD_PATTERN_MODE,                              "/redfish/v1/chassis/template",                       chassis_provider_entry, get_chassis_rsc, chassis_etag_del_property},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/power$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,        "/redfish/v1/chassis/template/power",      chassis_power_provider_entry, NULL, chassis_power_etag_del_property},

    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/power/powerhistorydata$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/chassis/template/power/powerhistorydata",                power_history_data_provider_entry, NULL, NULL},

    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/thermal$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                      "/redfish/v1/chassis/template/thermal",                thermal_provider_entry, get_thermal_src, chassis_thermal_etag_del_property},

    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/thermal/inlethistorytemperature$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/chassis/template/thermal/inlethistorytemperature",                thermal_inlet_history_temperature_provider_entry, NULL, NULL},

    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/thermal/clearinlethistorytemperatureactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/chassis/template/thermal/clearinlethistorytemperatureactioninfo", thermal_clear_history_temp_actioninfo_provider_entry, NULL, NULL},

    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/controlindicatorledactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/chassis/template/controlindicatorledactioninfo", chassis_control_led_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/setunitrfidinfoactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/chassis/unit/setunitrfidinfoactioninfo", chassis_set_unit_rfid_info_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/setdeviceinstalledstatusactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/chassis/unit/setdeviceinstalledstatusactioninfo", chassis_set_device_installed_status_entry, NULL, NULL},

    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/power/resethistorydataactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/chassis/template/power/resethistorydataactioninfo", power_reset_data_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/power/resetstatisticsactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/chassis/template/power/resetstatisticsactioninfo", power_reset_statistics_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/power/collecthistorydataactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/chassis/template/power/collecthistorydataactioninfo", power_collect_history_data_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/power/setpsusupplysourceactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/chassis/template/power/setpsusupplysourceactioninfo",
        power_set_psu_supply_source_actioninfo_provider_entry, NULL, NULL},

    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/bios", system_bios_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios/Settings$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/bios/settings", system_bios_settings_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios/ResetBiosActionInfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/bios/resetbiosactioninfo", bios_reset_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios/Settings/SettingsRevokeActionInfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/bios/Settings/SettingsRevokeActionInfo", revoke_biossetting_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios/PolicyConfig$",
        PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/bios/policyconfig",
        system_bios_policyconfig_reg_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios/PolicyConfig/Settings$",
        PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/bios/policyconfig/settings",
        system_bios_policyconfig_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios/Oem/Huawei/BootCertificates$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/Bios/Oem/Huawei/BootCertificates", system_bios_oem_bootcert_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios/Oem/Huawei/BootCertificates/ImportBootCertActionInfo$",
        PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/Bios/Oem/Huawei/BootCertificates/ImportBootCertActionInfo",
        system_bios_oem_bootcert_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios/Oem/Huawei/BootCertificates/ImportBootCrlActionInfo$",
        PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/Bios/Oem/Huawei/BootCertificates/ImportBootCrlActionInfo",
        system_bios_oem_bootcert_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios/Oem/Huawei/BootCertificates/ResetBootCertActionInfo$",
        PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/Bios/Oem/Huawei/BootCertificates/ResetBootCertActionInfo",
        system_bios_oem_bootcert_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios/Oem/Huawei/BootCertificates/ResetBootCrlActionInfo$",
        PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/Bios/Oem/Huawei/BootCertificates/ResetBootCrlActionInfo",
        system_bios_oem_bootcert_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios/Oem/Huawei/BootCertificates/ImportSecureBootCertActionInfo$",
        PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/Bios/Oem/Huawei/BootCertificates/ImportSecureBootCertActionInfo",
        system_bios_oem_bootcert_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Bios/Oem/Huawei/BootCertificates/ResetSecureBootCertActionInfo$",
        PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/Bios/Oem/Huawei/BootCertificates/ResetSecureBootCertActionInfo",
        system_bios_oem_bootcert_actioninfo_provider_entry, NULL, NULL},
    
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Memory$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/memory", system_memorys_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Memory/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/systems/1/memory/template",        system_memory_provider_entry, NULL, system_memory_etag_del_property},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/Memory/[^/]+/MemoryMetrics$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/memory/template/memorymetrics", system_memory_metrics_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/memoryhistoryusagerate$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/memoryhistoryusagerate", system_memory_usage_rate_provider_entry, NULL, NULL},

    
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/log1/entries/[a-zA-Z0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/systems/1/logservices/log1/entries/template",        system_provider_log_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/log1/entries$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/systems/1/logservices/log1/entries",        system_provider_logcollection_entry, NULL, NULL},

    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/boards$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,       "/redfish/v1/chassis/template/boards", chassis_boards_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/boards/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,       "/redfish/v1/chassis/template/boards/template", chassis_board_provider_entry, get_chassis_board_rsc, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/boards/[a-zA-Z0-9]+/arcardcontrolactioninfo$",
        PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/chassis/template/boards/template/arcardcontrolactioninfo",
        chassis_boards_arcard_control_actioninfo_provider_entry, NULL, NULL},
#ifdef ARM64_HI1711_ENABLED
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/switches$",     PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/chassis/template/switches", chassis_switches_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/switches/[^/]+$",     PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/chassis/template/switches/template", chassis_switch_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/switches/[a-zA-Z0-9]+/ports$",     PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/chassis/template/switches/template/ports", chassis_switch_ports_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/switches/[a-zA-Z0-9]+/ports/([a-zA-Z0-9]|[a-zA-Z_][a-zA-Z0-9_.])+$",     PROVIDER_URI_PATTERN_DEFAULT_MODE,
     "/redfish/v1/chassis/template/switches/template/ports/portinfo", chassis_switch_port_provider_entry, NULL, NULL },
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/switches/[a-zA-Z0-9]+/ports/([a-zA-Z0-9]|[a-zA-Z_][a-zA-Z0-9_.])+/"
     "setportenableactioninfo", PROVIDER_URI_PATTERN_DEFAULT_MODE,
     "/redfish/v1/chassis/template/switches/template/ports/portinfo/setportenableactioninfo",
     chassis_switch_port_enable_entry, NULL, NULL},
#endif

    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/ThresholdSensors$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,       "/redfish/v1/chassis/template/ThresholdSensors", chassis_thresholdsensor_provider_entry, NULL, threshold_sensor_etag_del_property},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/DiscreteSensors$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,       "/redfish/v1/chassis/template/DiscreteSensors", chassis_discretesensor_provider_entry, NULL, NULL},

    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/BackupBatteryUnits$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,       "/redfish/v1/chassis/template/backupbatteryunits", chassis_bbus_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/BackupBatteryUnits/[a-zA-Z0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,       "/redfish/v1/chassis/template/backupbatteryunits/template", chassis_bbu_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/LedGroup$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/chassis/template/ledgroup", chassis_led_group_provider_entry, NULL, NULL },

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/virtualmedia/ibmausbstick$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/virtualmedia/ibmausbstick", manager_provider_usbstick_ibma_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/virtualmedia/ibmausbstick/usbstickcontrolactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/virtualmedia/ibmausbstick/usbstickcontrolactioninfo",   manager_provider_usbstick_ibma_actinfo_entry, NULL, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/virtualmedia/usbstick$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/virtualmedia/usbstick",        manager_provider_usbstick_entry, NULL, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/virtualmedia/cd$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/virtualmedia/cd",        manager_provider_vmm_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/virtualmedia$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/virtualmedia",        manager_provider_vmmcollection_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/virtualmedia/cd/vmmcontrolactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/virtualmedia/cd/vmmcontrolactioninfo",        manager_provider_vmmaction_entry, NULL, NULL},

    
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/ntpservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/ntpservice",        managers_provider_ntpservice_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/ntpservice/importntpkeyactionInfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/ntpservice/importntpKeyactionInfo",        managers_provider_ntpaction_entry, NULL, NULL},
    
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/kvmservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/kvmservice",        managers_provider_kvmservice_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/kvmservice/setkvmkeyactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/kvmservice/setkvmkeyactioninfo",       manager_provider_kvm_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/kvmservice/exportkvmstartupfileactionInfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/kvmservice/exportkvmstartupfileactioninfo",       manager_provider_kvm_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/vncservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/vncservice",        managers_provider_vncservice_entry, NULL, NULL},

    
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/diagnosticservice", managers_provider_diagnosticservice_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice/stopvideoplaybackactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/diagnosticservice/stopvideoplaybackactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice/exportvideoactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/diagnosticservice/exportvideoactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice/exportvideoplaybackstartupfileactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/diagnosticservice/exportvideoplaybackstartupfileactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice/capturescreenshotactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/diagnosticservice/capturescreenshotactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice/deletescreenshotactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/diagnosticservice/deletescreenshotactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice/exportblackboxactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/diagnosticservice/exportblackboxactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice/exportserialportdataactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/managers/1/diagnosticservice/exportserialportdataactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice/exportnpulogactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/managers/1/diagnosticservice/exportnpulogactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice/workrecord$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/1/diagnosticservice/workrecord", workrecord_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice/workrecord/addrecordactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/1/diagnosticservice/workrecord/addrecordactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice/workrecord/deleterecordactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/1/diagnosticservice/workrecord/deleterecordactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/diagnosticservice/workrecord/modifyrecordactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/1/diagnosticservice/workrecord/modifyrecordactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices", manager_provider_logservice_collection_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/operatelog$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/operatelog", manager_provider_logservice_operatelog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/runlog$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/runlog", manager_provider_logservice_runlog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/securitylog$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/securitylog", manager_provider_logservice_securitylog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/operatelog/exportlogactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/operatelog/exportlogactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/runlog/exportlogactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/runlog/exportlogactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/securitylog/exportlogactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/securitylog/exportlogactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/operatelog/entries$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/operatelog/entries", manager_provider_operatelog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/runlog/entries$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/runlog/entries", manager_provider_runlog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/securitylog/entries$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/securitylog/entries", manager_provider_securitylog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/operatelog/entries/[0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/operatelog/entries/template", NULL, get_operatelog_item_details, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/runlog/entries/[0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/runlog/entries/template", NULL, get_runlog_item_details, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/securitylog/entries/[0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/securitylog/entries/template", NULL, get_securitylog_item_details, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                             "/redfish/v1/managers/1/spservice",        manager_spservice_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/fpcservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/fpcservice", manager_provider_fpcservice_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/fpcservice/memory$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/fpcservice/memory", manager_provider_fpcservice_memory_entry, NULL, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spraidcurrentconfigurations$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/1/spservice/spraidcurrentcfgs", manager_sp_raid_currents_provider_entry, get_sp_raid_currents_file_src, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spraidcurrentconfigurations/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/1/spservice/spraidcurrentcfgs/template", manager_sp_raid_current_provider_entry, get_sp_raid_current_file_src, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/exportspraidconfigurationsactionInfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/spservice/exportcfgactioninfo", manager_spraid_export_config_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/updateschemafilesactionInfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/spservice/updatefileactioninfo", manager_update_sp_schema_actioninfo_provider_entry, NULL, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spraid$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                   "/redfish/v1/managers/1/spservice/spraid",        manager_sp_raids_provider_entry, get_sp_raids_file_src, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spraid/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,         "/redfish/v1/managers/1/spservice/spraid/template",  manager_sp_raid_provider_entry, get_sp_raid_file_src, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/sposinstallpara$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,               "/redfish/v1/managers/1/spservice/sposinstallpara",        manager_sp_osinstalls_provider_entry, get_sp_osinstalls_file_src, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/sposinstallpara/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/spservice/sposinstallpara/template",  manager_sp_osinstall_provider_entry, get_sp_osinstall_file_src, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spfwupdate$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,               "/redfish/v1/managers/1/spservice/spfwupdate",        manager_sp_updates_provider_entry, get_sp_updates_file_src, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spfwupdate/1$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/spservice/spfwupdate/1",  manager_sp_update_provider_entry, NULL, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spresult$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,               "/redfish/v1/managers/1/spservice/spresult",        manager_sp_results_provider_entry, get_sp_results_file_src, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spresult/1$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/spservice/spresult/1",  manager_sp_result_provider_entry, get_sp_result_file_src, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spcfg$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,               "/redfish/v1/managers/1/spservice/spcfg",        manager_sp_cfgs_provider_entry, get_sp_cfgs_file_src, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spcfg/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/spservice/spcfg/template",  manager_sp_cfg_provider_entry, get_sp_cfg_file_src, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/deletefileactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/spservice/deletefileactioninfo",       manager_sp_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spfwupdate/1/simpleupdateactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/spservice/spfwupdate/1/simpleupdateactioninfo",  manager_sp_up_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/updateservice/startsyncupdateactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/updateservice/startsyncupdateactioninfo", sync_up_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/updateservice/activatebiosactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/updateservice/activatebiosactioninfo", activate_biosinfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/deviceinfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,               "/redfish/v1/managers/1/spservice/deviceinfo",        manager_sp_deviceinfo_provider_entry, get_sp_deviceinfo_file_src, NULL},

    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spdiagnose$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,       "/redfish/v1/managers/1/spservice/spdiagnose",    manager_sp_diagnose_collection_entry,   get_sp_diagnose_collection_rsc, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spdiagnose/1$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/spservice/spdiagnose/template",  manager_sp_diagnose_entry,              get_sp_diagnose_rsc, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spdriveerase$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,       "/redfish/v1/managers/1/spservice/spdriveerase",    manager_sp_drive_erase_collection_entry,   get_sp_drive_erase_collection_rsc, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/spservice/spdriveerase/1$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,     "/redfish/v1/managers/1/spservice/spdriveerase/template",  manager_sp_drive_erase_entry,              get_sp_drive_erase_rsc, NULL},

    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/drives$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/chassis/template/drives", chassis_drives_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/drives/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/chassis/template/drives/template",    chassis_drive_provider_entry, NULL, chassis_drive_etag_del_property},

    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/drives/[^/]+/cryptoeraseactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/chassis/template/drives/template/cryptoeraseactioninfo", chassis_drive_actioninfo_provider_entry, NULL, NULL},

    {"^/redfish/v1/updateservice/firmwareinventory$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,        "/redfish/v1/updateservice/firmwareinventory",  firmwareinventories_provider_entry, NULL, NULL},

    {URI_PATTERN_PERIPHERAL_FW,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                             RSC_PATH_FIRMWARE_INVENTORY,                   peripheral_fw_inventory_provider_entry, get_peripheral_fw_inventory_rsc, NULL},

    {"^/redfish/v1/updateservice/firmwareinventory/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/updateservice/firmwareinventory/template", firmwareinventory_provider_entry, NULL, NULL},

    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/pciedevices$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/chassis/template/pciedevices", chassis_pciedevices_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/pciedevices/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/chassis/template/pciedevices/template", chassis_pciedevice_provider_entry, NULL, chassis_pciedevice_etag_del_property},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/pciedevices/[^/]+/SetNMIActionInfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/chassis/template/pciedevices/template/setnmiactioninfo", chassis_pciedevices_nmi_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/pciedevices/[^/]+/functions/[0-9]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/chassis/template/pciedevices/template/functions/template", chassis_pciefunction_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/forcerestartservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/forcerestartservice", system_restart_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/forcerestartservice/restartosactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/forcerestartservice/restartosactioninfo",
        system_restartos_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/log1$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/logservices/log1", system_provider_logservice_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/logservices$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/logservices", system_provider_logservice_collection_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/log1/clearlogactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/logservices/log1/clearlogactioninfo", system_provider_logservice_action_info_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/log1/CollectSelactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/logservices/log1/CollectSelactioninfo", system_provider_logservice_collect_sel_action_info_entry, NULL, NULL},

    {"^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/log1/querysellogentriesactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/logservices/log1/querysellogentriesactioninfo", system_provider_logservice_query_sel_log_action_info_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/hostlog$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/logservices/hostlog", system_provider_logservice_hostlog_hostlog_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/hostlog/pushactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/logservices/hostlog/pushactioninfo", system_provider_logservice_hostlog_action_info_entry, NULL, NULL},

    {"^/redfish/v1/systems/[a-zA-Z0-9]+/networkhistoryusagerate$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                                 "/redfish/v1/systems/1/networkhistoryusagerate",     system_network_usage_rate_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/networkinterfaces$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                                             "/redfish/v1/systems/1/networkinterfaces",                                system_networkinterfaces_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/networkinterfaces/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                                "/redfish/v1/systems/1/networkinterfaces/template",                       system_networkinterface_provider_entry, NULL, NULL},
    {"^/redfish/v1/systems/[a-zA-Z0-9]+/networkinterfaces/[^/]+/networkports$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                   "/redfish/v1/systems/1/networkinterfaces/template/networkports",          system_networkports_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/networkadapters$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                                      "/redfish/v1/chassis/template/networkadapters",                                          chassis_networkadapters_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/networkadapters/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                               "/redfish/v1/chassis/template/networkadapters/template",                                 chassis_networkadapter_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/networkadapters/[^/]+/networkports$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                   "/redfish/v1/chassis/template/networkadapters/template/networkports",          chassis_networkports_provider_entry, NULL, NULL},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/networkadapters/[^/]+/networkports/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/chassis/template/networkadapters/template/networkports/template", chassis_networkport_provider_entry, NULL, chassis_networkport_etag_del_property},
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/networkadapters/[^/]+/networkports/[^/]+/opticalmodule$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/chassis/template/networkadapters/template/networkports/template/opticalmodule", chassis_opticalmodule_provider_entry, NULL, chassis_opticalmodule_etag_del_property},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/fdmservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/managers/1/fdmservice", manager_fdmservice_provider_entry, get_fdmservice_rsc, manager_fdmservice_etag_del_property},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/fdmservice/[0-9]+/eventreports$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/managers/1/fdmservice/template/eventreports", fdm_event_reports_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/fdmservice/fdmservice.regenerateactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/fdmservice/regenerateactioninfo", fdm_regenerate_action_info_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/fdmservice/fdmservice.removealarmactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/fdmservice/removealarmactioninfo", fdm_remove_alarm_action_info_provider_entry, NULL, NULL},

    {TASK_MONITOR_REGEX,  PROVIDER_URI_PATTERN_DEFAULT_MODE, TASK_MONITOR_PATH, task_monitor_provider_entry, get_task_monitor_rsc, NULL},

    {"^/redfish/v1/jsonschemas$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/jsonschemas",        provider_jsonschemas_entry, NULL, NULL},

    {RACK_MGNT_POWER_AGENT_PATTERN,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                       RACK_MGNT_POWER_AGENT_RSC_PATH,                  rack_mgnt_power_agent_provider_entry, get_power_agent, rack_power_agent_etag_del_property},
    {RACK_MGNT_SET_RACK_POWER_CAPPING_ACTION_INFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,        RACK_MGNT_SET_RACK_POWER_CAPPING_ACTION_TEMPLATE, rack_mgnt_set_pwr_capping_actioninfo_provider_entry, NULL, NULL},
    {RACK_MGNT_PERIOD_PRE_RACK_POWER_CAPPING_ACTION_INFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE, RACK_MGNT_PERIOD_PRE_RACK_POWER_CAPPING_ACTION_TEMPLATE, rack_mgnt_period_pre_pwr_capping_actioninfo_provider_entry, NULL, NULL},

    {BIOS_ATTRIBUTE_REGISTRY_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/registrystore/attributeregistries/en/biosattributeregistry_template", registrystore_bios_provider_entry, get_bios_registry_file_rsc, NULL},
    {RFPROP_REGRIST_ODTAID_RESOURCE,  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/Registries/BiosAttributeRegistry.v1_0_1", registries_bios_provider_entry, NULL, NULL},
    {TASK_RSC_REGEX,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                                        TASK_TEMPLATE_PATH,                            task_provider_entry, get_task_rsc, NULL},
    {TASK_COLLECTION_RSC_REGEX,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                             TASK_COL_TEMPLATE_PATH,                        task_collection_provider_entry, get_task_collection_rsc, NULL},
    {TASK_SVC_RSC_REGEX,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                                    TASK_SVC_TEMPLATE_PATH,                        task_svc_provider_entry, get_task_svc_rsc, NULL},
    {UPGRADE_SVC_RSC_REGEX,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                                 UPDATE_SVC_TEMPLATE_PATH,                      update_svc_provider_entry, NULL, NULL},
    {HTTPS_CERT_REGEX,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                                      HTTPS_CERT_TEMPLATE_PATH,                      https_cert_provider_entry, NULL, NULL},

    {URI_PATTERN_HTTPS_CERT_GEN_CSR_ACTIONINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,             RSC_PATH_HTTPS_CERT_GEN_CSR_ACTIONINFO,        https_cert_actioninfo_entry, NULL, NULL},
    {URI_PATTERN_HTTPS_CERT_EXPORT_CSR_ACTIONINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,          RSC_PATH_HTTPS_CERT_EXPORT_CSR_ACTIONINFO,     https_cert_actioninfo_entry, NULL, NULL},
    {URI_PATTERN_HTTPS_CERT_IMPORT_CERT_ACTIONINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,         RSC_PATH_HTTPS_CERT_IMPORT_CERT_ACTIONINFO,    https_cert_actioninfo_entry, NULL, NULL},
    {URI_PATTERN_HTTPS_CERT_IMPORT_CUST_CERT_ACTIONINFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,    RSC_PATH_HTTPS_CERT_IMPORT_CUST_CERT_ACTIONINFO, https_cert_actioninfo_entry, NULL, NULL},

  #ifdef ARM64_HI1711_ENABLED
    {DICE_CERT_REGEX,                                       PROVIDER_URI_PATTERN_DEFAULT_MODE,  DICE_CERT_TEMPLATE_PATH,                            dice_cert_provider_entry, NULL, NULL },
    {URI_PATTERN_DICE_CERT_EXPORT_CSR_ACTIONINFO,           PROVIDER_URI_PATTERN_DEFAULT_MODE,  RSC_PATH_DICE_CERT_EXPORT_CSR_ACTIONINFO,           dice_cert_actioninfo_entry, NULL, NULL},
    {URI_PATTERN_DICE_CERT_IMPORT_CERT_ACTIONINFO,          PROVIDER_URI_PATTERN_DEFAULT_MODE,  RSC_PATH_DICE_CERT_IMPORT_CERT_ACTIONINFO,          dice_cert_actioninfo_entry, NULL, NULL},
    {URI_PATTERN_DICE_CERT_EXPORT_CERT_CHAIN_ACTIONINFO,    PROVIDER_URI_PATTERN_DEFAULT_MODE,  RSC_PATH_DICE_CERT_EXPORT_CERT_CHAIN_ACTIONINFO,    dice_cert_actioninfo_entry, NULL, NULL},
  #endif

    {SECURITY_SVC_REGEX,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                                    SECURITY_SVC_TEMPLATE_PATH,                    security_svc_provider_entry, NULL, NULL},
    {UPDATEMK_ACTIONINFO_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, UPDATEMK_ACTIONINFO_TEMPLATE_PATH, security_srv_actioninfo_provider_entry, NULL, NULL},
    {IMPORTROOTCA_ACTIONINFO_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, IMPORTROOTCA_ACTIONINFO_TEMPLATE_PATH, security_srv_actioninfo_provider_entry, NULL, NULL},
    {DELETEROOTCA_ACTIONINFO_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, DELETEROOTCA_ACTIONINFO_TEMPLATE_PATH, security_srv_actioninfo_provider_entry, NULL, NULL},
    {IMPORTCRL_ACTIONINFO_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, IMPORTCRL_ACTIONINFO_TEMPLATE_PATH, security_srv_actioninfo_provider_entry, NULL, NULL},
    {DELETECRL_ACTIONINFO_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, DELETECRL_ACTIONINFO_TEMPLATE_PATH, security_srv_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/TpcmService$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/Managers/1/SecurityService/TpcmService", tpcm_config_provider_entry, NULL, NULL},
    {"^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/CertUpdateService$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/Managers/1/SecurityService/CertUpdateService", cert_update_svc_provider_entry, NULL, NULL},
    {"^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/CertUpdateService/ImportCAActionInfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/Managers/1/SecurityService/CertUpdateService/ImportCAActionInfo", cert_update_svc_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/CertUpdateService/ImportCrlActionInfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/Managers/1/SecurityService/CertUpdateService/ImportCrlActionInfo", cert_update_svc_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/CertUpdateService/deleteCrlActionInfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/Managers/1/SecurityService/CertUpdateService/deleteCrlActionInfo", cert_update_svc_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/CertUpdateService/ImportClientCertActionInfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/Managers/1/SecurityService/CertUpdateService/ImportClientCertActionInfo", cert_update_svc_actioninfo_provider_entry, NULL, NULL},
    {"^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/CertUpdateService/UpdateCertFromCAActionInfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/Managers/1/SecurityService/CertUpdateService/UpdateCertFromCAActionInfo", cert_update_svc_actioninfo_provider_entry, NULL, NULL},
    {EVENT_SVC_REGEX,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                                       EVT_SVC_TEMPLATE_PATH,                         evt_svc_provider_entry, get_evt_svc_rsc, NULL},
    {EVENT_SUBSCRIPTION_COLLECTION_REGEX,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                   EVT_SUBSCRIPTION_COL_TEMPLATE_PATH,            evt_subscription_col_provider_entry, get_evt_subscription_collection_rsc, NULL},
    {EVENT_SUBSCRIPTION_REGEX,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                              SUBSCRIPTION_TEMPLATE_PATH,                    evt_subscription_provider_entry, get_evt_subscription_rsc, NULL},
    {REGISTRY_COLLECTION_PATTERN,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                           REGISTRY_COLLECTION_PATH,                      registry_collection_provider_entry, get_registry_file_collection_rsc, NULL},
    {IBMC_EVENTS_REGISTRY_RSC_PATTERN,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                      IBMC_EVENTS_REGISTRY_PATH,                     registry_rsc_provider_entry, get_event_registry_file_rsc, NULL},
    {PRIVILEGE_MAP_PATTERN,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                                 PRIVILEGE_MAP_RSC_PATH,                        NULL, get_privilege_map_rsc, NULL},

    {RACK_MGNT_SVC_PATTERN,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                                 RACK_MGNT_SVC_RSC_PATH,                        rack_mgnt_service_provider_entry, get_rack_mgnt_service, NULL},
    {RACK_MGNT_SESSION_AGENT_PATTERN,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                       RACK_MGNT_SESSION_AGENT_RSC_PATH,              rack_mgnt_session_agent_provider_entry, get_session_agent, NULL},
    {RACK_MGNT_SET_DEVICE_TOKEN_ACTION_INFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                RACK_MGNT_SET_DEVICE_TOKEN_ACTION_TEMPLATE,    rack_mgnt_set_dev_token_actioninfo_provider_entry, NULL, NULL},

    {RACK_MGNT_NETWORK_AGENT_PATTERN,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                       RACK_MGNT_NETWORK_AGENT_RSC_PATH,              rack_mgnt_network_agent_provider_entry, get_network_agent, NULL},
    {RACK_MGNT_SET_DEVICE_IPINFO_ACTION_INFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                RACK_MGNT_SET_DEVICE_IPINFO_ACTION_TEMPLATE,  rack_mgnt_set_dev_ipinfo_actioninfo_provider_entry, NULL, NULL},

    {URI_PATTERN_FABRIC_COLLECTION,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                         RSC_PATH_FABRIC,                               fabric_collection_provider_entry, NULL, NULL},
    {URI_PATTERN_FABRIC_PCIE,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                               RSC_PATH_FABRIC_PCIE,                          pcie_fabric_provider_entry, get_pcie_fabric_rsc, NULL},
    {URI_PATTERN_FABRIC_PCIE_CONFIG_TOPO_ACT_INFO,  PROVIDER_URI_PATTERN_DEFAULT_MODE,          RSC_PATH_PCIE_FABRIC_CONFIG_TOPO_ACT_INFO,     config_composition_action_info_provider_entry, NULL, NULL},
    {URI_PATTERN_FABRIC_PCIE_ZONES,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                         RSC_PATH_FABRIC_ZONE_COLLECTION,               zone_collection_provider_entry, get_zone_collection_rsc, NULL},
    {URI_PATTERN_FABRIC_PCIE_ZONE,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                          RSC_PATH_FABRIC_ZONE,                          zone_provider_entry, get_zone_rsc, NULL},
    {URI_PATTERN_FABRIC_PCIE_ENDPOINTS,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                     RSC_PATH_FABRIC_EP_COLLECTION,                 endpoint_collection_provider_entry, get_endpoint_collection_rsc, NULL},
    {URI_PATTERN_FABRIC_PCIE_ENDPOINT,  PROVIDER_URI_PATTERN_DEFAULT_MODE,                      RSC_PATH_FABRIC_EP,                            endpoint_provider_entry, get_endpoint_rsc, NULL},
};


LOCAL RESOURCE_PROVIDER_S g_hmm_resource_provider_info[] = {
    { "^/redfish/v1$", PROVIDER_URI_PATTERN_DEFAULT_MODE,               "/redfish/v1",                   serviceroot_provider_entry,         NULL, NULL},
    { "^/redfish/v1/systems$", PROVIDER_URI_PATTERN_DEFAULT_MODE,       "/redfish/v1/systems",           NULL,   smm_get_systems_collection, NULL},
    { "^/redfish/v1/chassisoverview$", PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/chassisoverview",          NULL,   smm_get_overview, NULL},
    { "^/redfish/v1/systemoverview$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/systemoverview", system_overview_provider_collection_entry,   NULL, NULL},
    
    { "^/redfish/v1/sessionservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE,                         "/redfish/v1/sessionservice/",                  sessionservice_provider_entry,      NULL, NULL},
    { "^/redfish/v1/sessionservice/sessions$", PROVIDER_URI_PATTERN_DEFAULT_MODE,                "/redfish/v1/sessionservice/sessions",          session_collection_provider_entry,  NULL, NULL},
    { "^/redfish/v1/sessionservice/sessions/[a-zA-Z0-9]+$", PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/sessionservice/sessions/template", session_provider_entry,             NULL, NULL},
    
    { "^/redfish/v1/chassis$", PROVIDER_URI_PATTERN_DEFAULT_MODE,       "/redfish/v1/chassis",           smm_chassis_provider_collection_entry,  NULL, NULL },
    { "^/redfish/v1/chassis/ipv4config$", PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/chassis/ipv4config",          NULL,   smm_get_ipv4_config, NULL},
    { "^/redfish/v1/chassis/ipv6config$", PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/chassis/ipv6config",          NULL,   smm_get_ipv6_config, NULL},
    { "^/redfish/v1/chassis/hmm[12]$", PROVIDER_URI_SPECICAL_PATTERN_MODE,  "/redfish/v1/chassis/smm", smm_chassis_hmm_provider_entry, NULL, NULL },
    { "^/redfish/v1/chassis/(blade([1-9]|[1-2][0-9]|3[0-27-8])|swi[1-4])$", PROVIDER_URI_SPECICAL_PATTERN_MODE,  "/redfish/v1/chassis/blade", NULL, smm_get_chassis_bmc, NULL },
    { "^/redfish/v1/chassis/enclosure$", PROVIDER_URI_SPECICAL_PATTERN_MODE, "/redfish/v1/chassis/enclosure",
      smm_chassis_enclosure_provider_entry, NULL, NULL },
    { "^/redfish/v1/chassis/enclosure/hdddrawer$", PROVIDER_URI_SPECICAL_PATTERN_MODE,
        "/redfish/v1/chassis/enclosure/hdddrawer", smm_chassis_enclosure_hdd_drawer_entry, NULL, NULL },
    #ifdef ARM64_HI1711_ENABLED
    { URI_PATTERN_CHASSIS_LANSWITCHES,     PROVIDER_URI_PATTERN_DEFAULT_MODE, RSC_PATH_CHASSIS_LANSWITCHES,
      lanswitch_collection_provider_entry, NULL, NULL },
    { URI_PATTERN_CHASSIS_LANSWITCH,       PROVIDER_URI_PATTERN_DEFAULT_MODE, RSC_PATH_CHASSIS_LANSWITCH,
      lanswitch_provider_entry, NULL, NULL },
    { URI_PATTERN_CHASSIS_LANSWITCH_PORTS, PROVIDER_URI_PATTERN_DEFAULT_MODE, RSC_PATH_CHASSIS_LANSWITCH_PORTS,
      lanswitch_ports_provider_entry, NULL, NULL },
    { URI_PATTERN_CHASSIS_LANSWITCH_PORT,  PROVIDER_URI_PATTERN_DEFAULT_MODE, RSC_PATH_CHASSIS_LANSWITCH_PORT,
      lanswitch_port_provider_entry, NULL, NULL },
    { URI_PATTERN_CHASSIS_PORT_ENABLE,  PROVIDER_URI_PATTERN_DEFAULT_MODE, RSC_PATH_CHASSIS_PORT_ENABLE,
      lanswitch_port_enable_entry, NULL, NULL },
    #endif
    
    {"^/redfish/v1/accountservice/kerberosservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
     "/redfish/v1/accountservice/kerberosservice", kerberosservice_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/kerberosservice/kerberoscontrollers$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
     "/redfish/v1/accountservice/kerberosservice/kerberoscontrollers", kerberosservice_collection_provider_entry,
     NULL, NULL},
    {"^/redfish/v1/accountservice/kerberosservice/kerberoscontrollers/[a-zA-Z0-9]+$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
     "/redfish/v1/accountservice/kerberosservice/kerberoscontrollers/template", kerberos_controller_provider_entry,
     NULL, NULL},
    {"^/redfish/v1/accountservice/kerberosservice/kerberoscontrollers/[0-9]+/importkeytableactioninfo",
     PROVIDER_URI_PATTERN_DEFAULT_MODE,
     "/redfish/v1/accountservice/kerberosservice/kerberoscontrollers/template/actioninfo",
     kerberos_controller_provider_import_entry, NULL, NULL},

    { "^/redfish/v1/chassis/[a-zA-Z0-9]+$", PROVIDER_URI_WILDCARD_PATTERN_MODE,  "/redfish/v1/chassis/enclosure", smm_chassis_enclosure_provider_entry, NULL, NULL },
    { "^/redfish/v1/chassis/[a-zA-Z0-9]+/power$", PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/chassis/enclosure/power", chassis_enclosure_power_provider_entry, NULL, chassis_enclosure_power_etag_del_property},
    { "^/redfish/v1/chassis/[a-zA-Z0-9]+/power/powerhistorydata$", PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/chassis/template/power/powerhistorydata",                power_history_data_provider_entry, NULL, NULL },
    { "^/redfish/v1/chassis/[a-zA-Z0-9]+/power/resethistorydataactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/chassis/template/power/resethistorydataactioninfo", power_reset_data_actioninfo_provider_entry, NULL, NULL },
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/power/resetstatisticsactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/chassis/template/power/resetstatisticsactioninfo",
        power_reset_statistics_actioninfo_provider_entry, NULL, NULL},
    { "^/redfish/v1/chassis/[a-zA-Z0-9]+/power/collecthistorydataactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/chassis/template/power/collecthistorydataactioninfo", power_collect_history_data_actioninfo_provider_entry, NULL, NULL },
    {"^/redfish/v1/chassis/[a-zA-Z0-9]+/power/setpsusupplysourceactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/chassis/template/power/setpsusupplysourceactioninfo",
        power_set_psu_supply_source_actioninfo_provider_entry, NULL, NULL},
    { "^/redfish/v1/chassis/[a-zA-Z0-9]+/thermal$", PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/chassis/enclosure/thermal", smm_chassis_enclosure_thermal_provider_entry, smm_chassis_enclosure_thermal_rsc, smm_chassis_enclosure_thermal_etag_del_property },
    { "^/redfish/v1/chassis/[a-zA-Z0-9]+/ThresholdSensors$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,       "/redfish/v1/chassis/template/ThresholdSensors", chassis_thresholdsensor_provider_entry, NULL, threshold_sensor_etag_del_property},
    { "^/redfish/v1/chassis/[a-zA-Z0-9]+/DiscreteSensors$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,       "/redfish/v1/chassis/template/DiscreteSensors", chassis_discretesensor_provider_entry, NULL, NULL},
    { "^/redfish/v1/chassis/[a-zA-Z0-9]+/opticalmodule$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/chassis/opticalmodule", chassis_direct_opticalmodule_list_provider_entry, NULL, NULL},
    { "^/redfish/v1/chassis/[a-zA-Z0-9]+/opticalmodule/[^/]+$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/chassis/opticalmodule/opticalmoduleinfo",
        chassis_direct_opticalmodule_provider_entry, NULL, chassis_opticalmodule_etag_del_property},

    
    { "^/redfish/v1/managers$", PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/managers", NULL, smm_get_managers_collection, NULL },
    { "^/redfish/v1/managers/(hmm[12]|swi[1-4])$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/hmm", smm_manager_provider_entry,  NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/poweronpermitactioninfo", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/hmm/poweronpermitactioninfo", manager_provider_action_info_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/ShelfPowerControlActionInfo", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/managers/hmm/ShelfPowerControlActionInfo", manager_provider_action_info_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/resetactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
      "/redfish/v1/managers/1/resetactioninfo", manager_provider_action_info_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/dumpactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/dumpactioninfo", manager_provider_action_info_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/deletelanguageactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/deletelanguageactioninfo", manager_provider_action_info_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/forcefailoveractioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/hmm/forcefailoveractioninfo", manager_provider_action_info_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/generaldownloadactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/generaldownloadactioninfo", manager_provider_action_info_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/deleteswitchprofileactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/hmm/deleteswitchprofileactioninfo", manager_provider_action_info_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/ethernetinterfaces$", PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/managers/1/ethernetinterfaces",          manager_ethernetinterfaces_provider_entry,  NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/ethernetinterfaces/[^/]+$", PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/managers/1/ethernetinterfaces/template",          manager_ethernetinterface_provider_entry,  NULL, NULL},
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/ntpservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/ntpservice", managers_provider_ntpservice_entry, NULL, NULL},
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/ntpservice/importntpkeyactionInfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/ntpservice/importntpKeyactionInfo", managers_provider_ntpaction_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/syslogservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/syslogservice", manager_syslog_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/syslogservice/importrootcertificateactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/syslogservice/importrootcertificateactioninfo", manager_syslog_import_root_cert_actioninfo_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/syslogservice/importclientcertificateactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/syslogservice/importclientcertificateactioninfo", manager_syslog_import_client_cert_actioninfo_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/syslogservice/submittesteventactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/syslogservice/submittesteventactioninfo", manager_syslog_submit_test_actioninfo_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/syslogservice/importcrlactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
      "/redfish/v1/managers/1/syslogservice/importcrlactioninfo", manager_syslog_import_crl_actioninfo_entry,
      NULL, NULL },

    
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices", manager_provider_logservice_collection_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/operatelog$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/operatelog", manager_provider_logservice_operatelog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/runlog$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/runlog", manager_provider_logservice_runlog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/securitylog$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/securitylog", manager_provider_logservice_securitylog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/operatelog/exportlogactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/operatelog/exportlogactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/runlog/exportlogactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/runlog/exportlogactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/securitylog/exportlogactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/securitylog/exportlogactioninfo", provider_actioninfo_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/operatelog/entries$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/operatelog/entries", manager_provider_operatelog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/runlog/entries$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/runlog/entries", manager_provider_runlog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/securitylog/entries$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/logservices/securitylog/entries", manager_provider_securitylog_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/operatelog/entries/[0-9]+$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/1/logservices/operatelog/entries/template", NULL, get_operatelog_item_details, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/runlog/entries/[0-9]+$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/1/logservices/runlog/entries/template", NULL, get_runlog_item_details, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/logservices/securitylog/entries/[0-9]+$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/1/logservices/securitylog/entries/template", NULL, get_securitylog_item_details, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/fpcservice$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/fpcservice", manager_provider_fpcservice_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/fpcservice/memory$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/fpcservice/memory", manager_provider_fpcservice_memory_entry, NULL, NULL},

    { "^/redfish/v1/managers/[a-zA-Z0-9]+/networkprotocol$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/networkprotocol", networkprotocol_provider_entry, NULL, NULL },
    { SECURITY_SVC_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, SECURITY_SVC_TEMPLATE_PATH, security_svc_provider_entry, NULL, NULL },
    { IMPORTROOTCA_ACTIONINFO_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, IMPORTROOTCA_ACTIONINFO_TEMPLATE_PATH,
        security_srv_actioninfo_provider_entry, NULL, NULL },
    { DELETEROOTCA_ACTIONINFO_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, DELETEROOTCA_ACTIONINFO_TEMPLATE_PATH,
        security_srv_actioninfo_provider_entry, NULL, NULL },
    { IMPORTCRL_ACTIONINFO_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, IMPORTCRL_ACTIONINFO_TEMPLATE_PATH,
        security_srv_actioninfo_provider_entry, NULL, NULL },
    { DELETECRL_ACTIONINFO_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, DELETECRL_ACTIONINFO_TEMPLATE_PATH,
        security_srv_actioninfo_provider_entry, NULL, NULL },

    { HTTPS_CERT_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, HTTPS_CERT_TEMPLATE_PATH, https_cert_provider_entry, NULL, NULL },
    { URI_PATTERN_HTTPS_CERT_GEN_CSR_ACTIONINFO, PROVIDER_URI_PATTERN_DEFAULT_MODE, RSC_PATH_HTTPS_CERT_GEN_CSR_ACTIONINFO, https_cert_actioninfo_entry, NULL, NULL },
    { URI_PATTERN_HTTPS_CERT_EXPORT_CSR_ACTIONINFO, PROVIDER_URI_PATTERN_DEFAULT_MODE, RSC_PATH_HTTPS_CERT_EXPORT_CSR_ACTIONINFO, https_cert_actioninfo_entry, NULL, NULL },
    { URI_PATTERN_HTTPS_CERT_IMPORT_CERT_ACTIONINFO, PROVIDER_URI_PATTERN_DEFAULT_MODE, RSC_PATH_HTTPS_CERT_IMPORT_CERT_ACTIONINFO, https_cert_actioninfo_entry, NULL, NULL },
    { URI_PATTERN_HTTPS_CERT_IMPORT_CUST_CERT_ACTIONINFO, PROVIDER_URI_PATTERN_DEFAULT_MODE, RSC_PATH_HTTPS_CERT_IMPORT_CUST_CERT_ACTIONINFO, https_cert_actioninfo_entry, NULL, NULL },

#ifdef ARM64_HI1711_ENABLED
    { DICE_CERT_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, DICE_CERT_TEMPLATE_PATH,
        dice_cert_provider_entry, NULL, NULL },
    { URI_PATTERN_DICE_CERT_EXPORT_CSR_ACTIONINFO, PROVIDER_URI_PATTERN_DEFAULT_MODE,
        RSC_PATH_DICE_CERT_EXPORT_CSR_ACTIONINFO, dice_cert_actioninfo_entry, NULL, NULL },
    { URI_PATTERN_DICE_CERT_IMPORT_CERT_ACTIONINFO, PROVIDER_URI_PATTERN_DEFAULT_MODE,
        RSC_PATH_DICE_CERT_IMPORT_CERT_ACTIONINFO, dice_cert_actioninfo_entry, NULL, NULL },
    { URI_PATTERN_DICE_CERT_EXPORT_CERT_CHAIN_ACTIONINFO, PROVIDER_URI_PATTERN_DEFAULT_MODE,
        RSC_PATH_DICE_CERT_EXPORT_CERT_CHAIN_ACTIONINFO, dice_cert_actioninfo_entry, NULL, NULL },
#endif

    { "^/redfish/v1/managers/[a-zA-Z0-9]+/snmpservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE,                  "/redfish/v1/managers/1/snmpservice",    snmp_provider_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/snmpservice/submittesteventactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,                  "/redfish/v1/managers/1/snmpservice/submittesteventactioninfo",    snmp_actioninfo_provider_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/snmpservice/configsnmpv3privpasswdactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,                  "/redfish/v1/managers/1/snmpservice/configsnmpv3privpasswdactioninfo",    snmp_actioninfo_provider_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/restoreswitchprofileactionInfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/hmm/restoreswitchprofileactionInfo", manager_provider_action_info_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/parseswitchprofileactionInfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/hmm/parseswitchprofileactionInfo", manager_provider_action_info_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/switchprofiles$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/hmm/switchprofiles", managers_provider_switch_profiles_entry, NULL, NULL },
    { "^/redfish/v1/managers/[a-zA-Z0-9]+/lldpservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/managers/1/lldpservice", manager_lldpservice_provider_entry, NULL, NULL },
    
    {"^/redfish/v1/systems/hmm[12]$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                              "/redfish/v1/systems/1/",                       system_provider_entry, NULL, systems_delete_etag_prop},
    { "^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/log1$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/logservices/log1", system_provider_logservice_entry, NULL, NULL },
    { "^/redfish/v1/systems/[a-zA-Z0-9]+/logservices$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/logservices", system_provider_logservice_collection_entry, NULL, NULL },
    { "^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/log1/clearlogactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/logservices/log1/clearlogactioninfo", system_provider_logservice_action_info_entry, NULL, NULL },
    { "^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/log1/entries/[a-zA-Z0-9]+$", PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/systems/1/logservices/log1/entries/template",        system_provider_log_entry, NULL, NULL },
    { "^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/log1/entries$", PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/systems/1/logservices/log1/entries",        system_provider_logcollection_entry, NULL, NULL },
    { "^/redfish/v1/systems/[a-zA-Z0-9]+/logservices/log1/querysellogentriesactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/systems/1/logservices/log1/querysellogentriesactioninfo", system_provider_logservice_query_sel_log_action_info_entry, NULL, NULL },
    { "^/redfish/v1/systems/[a-zA-Z0-9]+/digitalwarranty$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/systems/1/digitalwarranty", system_digital_warranty_provider_entry, NULL, NULL },
    
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/smtpservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/smtpservice", manager_smtp_provider_entry, NULL, NULL},
    {"^/redfish/v1/managers/[a-zA-Z0-9]+/smtpservice/submittesteventactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/managers/1/smtpservice/submittesteventactioninfo", manager_smtp_actioninfo_provider_entry, NULL, NULL},

    {"^/redfish/v1/jsonschemas$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,                 "/redfish/v1/jsonschemas",        provider_jsonschemas_entry, NULL, NULL},

    
    { "^/redfish/v1/accountservice/importrootcertificateactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/importrootcertificateactioninfo/", accountservice_provider_import_entry, NULL, NULL },
    { "^/redfish/v1/accountservice/deleterootcertificateactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/deleterootcertificateactioninfo/", accountservice_provider_delete_entry, NULL, NULL },
    { "^/redfish/v1/accountservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/", accountservice_provider_entry, NULL, NULL },
    { "^/redfish/v1/accountservice/importcrlactioninfo$",  PROVIDER_URI_PATTERN_DEFAULT_MODE,
      "/redfish/v1/accountservice/importcrlactioninfo/", accountservice_provider_import_crl_actioninfo_entry,
      NULL, NULL},
    { "^/redfish/v1/accountservice/accounts$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/accounts", accounts_provider_entry, NULL, NULL },
    { "^/redfish/v1/accountservice/accounts/[a-zA-Z0-9]+$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/accounts/template", account_provider_entry, NULL, NULL },
    { "^/redfish/v1/accountservice/oem/huawei/nodes$", PROVIDER_URI_PATTERN_DEFAULT_MODE,   "/redfish/v1/accountservice/oem/huawei/nodes", smm_account_node_provider_collection_entry, NULL, NULL},
    { "^/redfish/v1/accountservice/roles$", PROVIDER_URI_PATTERN_DEFAULT_MODE,                   "/redfish/v1/accountservice/roles",             roles_provider_entry, NULL, NULL },
    {"^/redfish/v1/accountservice/roles/Administrator$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/accountservice/roles/administrator",  role_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/roles/Operator$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/accountservice/roles/operator", role_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/roles/Commonuser$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/accountservice/roles/commonuser", role_provider_entry, NULL, NULL},
    {"^/redfish/v1/accountservice/roles/NoAccess$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/accountservice/roles/noaccess", role_provider_entry, NULL, NULL},
    { "^/redfish/v1/accountservice/roles/[a-zA-Z0-9]+$", PROVIDER_URI_PATTERN_DEFAULT_MODE,      "/redfish/v1/accountservice/roles/template",    role_provider_entry, NULL, NULL },
    { "^/redfish/v1/accountservice/ldapservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE,           "/redfish/v1/accountservice/ldapservice",    ldapservice_provider_entry, NULL, NULL},
    { "^/redfish/v1/accountservice/ldapservice/ldapcontrollers$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/ldapservice/ldapcontrollers",    ldapservice_collection_provider_entry, NULL, NULL},
    { "^/redfish/v1/accountservice/ldapservice/ldapcontrollers/[0-9]+$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/accountservice/ldapservice/ldapcontrollers/template", ldapservice_controller_provider_entry, NULL, NULL},
    { "^/redfish/v1/accountservice/ldapservice/ldapcontrollers/[0-9]+/importcertactioninfo", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/accountservice/ldapservice/ldapcontrollers/template/importcertactioninfo", ldapservice_controller_provider_import_entry, NULL, NULL},
    { "^/redfish/v1/accountservice/ldapservice/ldapcontrollers/[0-9]+/importcrlactioninfo", PROVIDER_URI_PATTERN_DEFAULT_MODE, "/redfish/v1/accountservice/ldapservice/ldapcontrollers/template/importcrlactioninfo", ldapservice_controller_provider_import_crl_entry, NULL, NULL},
    { PRIVILEGE_MAP_PATTERN, PROVIDER_URI_PATTERN_DEFAULT_MODE, PRIVILEGE_MAP_RSC_PATH, NULL, get_privilege_map_rsc, NULL },
    
    { "^/redfish/v1/updateservice$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/updateservice", update_svc_provider_entry, NULL, NULL },
    { "^/redfish/v1/updateservice/firmwareinventory$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/updateservice/firmwareinventory", firmwareinventories_provider_entry, NULL, NULL },
    { "^/redfish/v1/updateservice/firmwareinventory/[^/]+$", PROVIDER_URI_PATTERN_DEFAULT_MODE,  "/redfish/v1/updateservice/firmwareinventory/template", firmwareinventory_provider_entry, NULL, NULL },
    {"^/redfish/v1/updateservice/startsyncupdateactioninfo$", PROVIDER_URI_PATTERN_DEFAULT_MODE,
        "/redfish/v1/updateservice/startsyncupdateactioninfo", sync_up_actioninfo_provider_entry, NULL, NULL},
    
    { TASK_RSC_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, TASK_TEMPLATE_PATH, task_provider_entry, get_task_rsc, NULL },
    { TASK_COLLECTION_RSC_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, TASK_COL_TEMPLATE_PATH, task_collection_provider_entry, get_task_collection_rsc, NULL },
    { TASK_SVC_RSC_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, TASK_SVC_TEMPLATE_PATH, task_svc_provider_entry, get_task_svc_rsc, NULL },
    { TASK_MONITOR_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE, TASK_MONITOR_PATH, task_monitor_provider_entry, get_task_monitor_rsc, NULL },
    
    { EVENT_SVC_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE,                                       EVT_SVC_TEMPLATE_PATH,                         evt_svc_provider_entry, get_evt_svc_rsc, NULL },
    { EVENT_SUBSCRIPTION_COLLECTION_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE,                   EVT_SUBSCRIPTION_COL_TEMPLATE_PATH,            evt_subscription_col_provider_entry, get_evt_subscription_collection_rsc, NULL },
    { EVENT_SUBSCRIPTION_REGEX, PROVIDER_URI_PATTERN_DEFAULT_MODE,                              SUBSCRIPTION_TEMPLATE_PATH,                    evt_subscription_provider_entry, get_evt_subscription_rsc, NULL },
    { REGISTRY_COLLECTION_PATTERN, PROVIDER_URI_PATTERN_DEFAULT_MODE,                           REGISTRY_COLLECTION_PATH,                      registry_collection_provider_entry, get_registry_file_collection_rsc, NULL},
    { IBMC_EVENTS_REGISTRY_RSC_PATTERN, PROVIDER_URI_PATTERN_DEFAULT_MODE,                      IBMC_EVENTS_REGISTRY_PATH,                     registry_rsc_provider_entry, get_event_registry_file_rsc, NULL},
};

LOCAL PROPERTY_PROVIDER_S g_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};

RESOURCE_PROVIDER_S *get_resource_provider_info(void)
{
    return g_resource_provider_info;
}

gsize get_resource_provider_info_size(void)
{
    if (g_resource_provider_info_size == 0) {
        g_resource_provider_info_size = G_N_ELEMENTS(g_resource_provider_info);
    }

    return g_resource_provider_info_size;
}

RESOURCE_PROVIDER_S *get_hmm_resource_provider_info(void)
{
    return g_hmm_resource_provider_info;
}

gsize get_hmm_resource_provider_info_size(void)
{
    if (g_hmm_resource_provider_info_size == 0) {
        g_hmm_resource_provider_info_size = G_N_ELEMENTS(g_hmm_resource_provider_info);
    }

    return g_hmm_resource_provider_info_size;
}

LOCAL gint32 provider_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    if (FALSE == redfish_check_chassis_uri_valid(i_paras->uri, &i_paras->obj_handle) &&
        FALSE == redfish_check_manager_uri_valid(i_paras->uri) &&
        FALSE == redfish_check_system_uri_valid(i_paras->uri)) {
        debug_log(DLOG_ERROR, "%s %d: rsc at %s doesn't exist", __FUNCTION__, __LINE__, i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_actioninfo_provider;
    *count = sizeof(g_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


LOCAL gint32 get_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet = -1;
#define LOW_SYSTEM_URI "/redfish/v1/systems/1/"
#define LOW_MANAGER_URI "/redfish/v1/managers/1/"
#define LOW_CHASSIS_URI "/redfish/v1/chassis/1/"

    gchar *low_origin_uri = NULL;
    const gchar *origin_uri = NULL;
    gchar slot_uri[PSLOT_URI_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: redfish_get_board_slot fail.", __FUNCTION__, __LINE__));

    origin_uri = dal_json_object_get_str(*o_result_jso);
    return_val_do_info_if_expr(NULL == origin_uri, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json get string fail.", __FUNCTION__, __LINE__));

    low_origin_uri = g_ascii_strdown(origin_uri, strlen(origin_uri));
    return_val_do_info_if_expr(NULL == low_origin_uri, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d: change uri to lower fail", __FUNCTION__, __LINE__));

    if (NULL != strstr(low_origin_uri, LOW_SYSTEM_URI)) {
        iRet = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, "%s%s%s", RF_SYSTEMS_URI, slot,
            origin_uri + strlen(LOW_SYSTEM_URI) - 1);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    } else if (NULL != strstr(low_origin_uri, LOW_MANAGER_URI)) {
        iRet = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, "%s%s%s", RF_MANAGERS_URI, slot,
            origin_uri + strlen(LOW_MANAGER_URI) - 1);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    } else if (NULL != strstr(low_origin_uri, LOW_CHASSIS_URI)) {
        iRet = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, "%s%s%s", RF_CHASSIS_UI, slot,
            origin_uri + strlen(LOW_CHASSIS_URI) - 1);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    } else {
        debug_log(DLOG_ERROR, "%s %d: invalid uri : %s ", __FUNCTION__, __LINE__, low_origin_uri);
        g_free(low_origin_uri);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    g_free(low_origin_uri);

    *o_result_jso = json_object_new_string((const gchar *)slot_uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}