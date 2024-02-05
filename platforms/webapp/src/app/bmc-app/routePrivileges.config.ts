/**
 * BMC路由权限列表
 * access说明：访问该路由需要具有的权限
 */

export const routePrivilegesConfig = {
    '/login': {
        access: ['Login']
    },
    '/navigate/home': {
        access: ['Login']
    },

    // 系统管理
    '/navigate/system/info': {
        access: ['Login']
    },
    '/navigate/system/info/others': {
        access: ['Login']
    },
    '/navigate/system/info/product': {
        access: ['Login']
    },
    '/navigate/system/info/processor': {
        access: ['Login']
    },
    '/navigate/system/info/memory': {
        access: ['Login']
    },
    '/navigate/system/info/sensor': {
        access: ['Login']
    },
    '/navigate/system/info/net': {
        access: ['Login']
    },
    '/navigate/system/info/units': {
        access: ['Login']
    },
    '/navigate/system/monitor': {
        access: ['Login']
    },
    '/navigate/system/storage': {
        access: ['Login']
    },
    '/navigate/system/power': {
        access: ['Login']
    },
    '/navigate/system/fans': {
        access: ['Login']
    },
    '/navigate/system/partity': {
        access: ['Login']
    },
    '/navigate/system/bios': {
        access: ['Login']
    },

    // 维护诊断
    '/navigate/maintance/alarm': {
        access: ['Login']
    },
    '/navigate/maintance/report/syslog': {
        access: ['Login']
    },
    '/navigate/maintance/report/email': {
        access: ['Login']
    },
    '/navigate/maintance/report/trap': {
        access: ['Login']
    },
    '/navigate/maintance/fdm': {
        access: ['Login']
    },
    '/navigate/maintance/video': {
        access: ['OemDiagnosis']
    },
    '/navigate/maintance/systemlog': {
        access: ['OemDiagnosis']
    },
    '/navigate/maintance/ibmclog': {
        access: ['Login']
    },
    '/navigate/maintance/worknote': {
        access: ['Login']
    },

    // 用户与安全
    '/navigate/user/localuser': {
        access: ['Login']
    },
    '/navigate/user/ldap': {
        access: ['Login']
    },
    '/navigate/user/kerberos': {
        access: ['Login']
    },
    '/navigate/user/two-factor/root': {
        access: ['ConfigureUsers']
    },
    '/navigate/user/two-factor/client': {
        access: ['ConfigureUsers']
    },
    '/navigate/user/online-user': {
        access: ['ConfigureUsers']
    },
    '/navigate/user/security': {
        access: ['Login']
    },
    '/navigate/user/tpcm': {
        access: ['Login']
    },
    '/navigate/user/cert-update': {
        access: ['Login']
    },
    '/navigate/user/cert-update/cert-update-ssl': {
        access: ['Login']
    },
    '/navigate/user/cert-update/cert-update-bios': {
        access: ['Login']
    },

    // 服务管理
    '/navigate/service/port': {
        access: ['Login']
    },
    '/navigate/service/web': {
        access: ['Login']
    },
    '/navigate/service/kvm': {
        access: ['Login']
    },
    '/navigate/service/vmm': {
        access: ['Login']
    },
    '/navigate/service/vnc': {
        access: ['Login']
    },
    '/navigate/service/snmp': {
        access: ['Login']
    },

    // 网络配置
    '/navigate/manager/network': {
        access: ['Login']
    },
    '/navigate/manager/ntp': {
        access: ['Login']
    },
    '/navigate/manager/upgrade': {
        access: ['ConfigureComponents']
    },
    '/navigate/manager/configupgrade': {
        access: ['ConfigureUsers']
    },
    '/navigate/manager/language': {
        access: ['ConfigureComponents']
    },
    '/navigate/manager/license': {
        access: ['Login']
    },
    '/navigate/manager/ibma': {
        access: ['Login']
    },
    '/navigate/manager/sp': {
        access: ['Login']
    }
    ,
    '/navigate/manager/usb': {
        access: ['Login']
    }
};
