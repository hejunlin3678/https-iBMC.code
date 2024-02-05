/**
 * 路由权限列表
 * belong代表路由所属的产品，取值说明： 1:ibmc  2:irm ,3:emm
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
    '/navigate/system/info/sensor': {
        access: ['Login']
    },
    '/navigate/system/power': {
        access: ['Login']
    },
    '/navigate/system/battery': {
        access: ['Login']
    },
    '/navigate/system/fans': {
        access: ['Login']
    },
    '/navigate/system/asset': {
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
        access: ['OemDiagnosis']
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
    '/navigate/user/cert-update': {
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
    },
    '/navigate/manager/usb': {
        access: ['Login']
    }
};
