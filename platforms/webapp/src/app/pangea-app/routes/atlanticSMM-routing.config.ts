import { Routes } from '@angular/router';
import { PangeaAppComponent } from '../pangea-app.component';
import { AppGuard } from 'src/app/common-app/guard/app-guard.service';
import { PangeaGuard } from '../guard/pangea-guard.service';
import { KvmLoginGuard, AuthGuard } from 'src/app/common-app/guard';

export const atlanticSMMRoutes: Routes = [
    {
        path: '',
        component: PangeaAppComponent,
        canActivate: [AppGuard, PangeaGuard],
        children: [
            {
                path: 'login',
                loadChildren: () => import('../modules/login/login.module').then((m) => m.LoginModule),
                canActivateChild: [KvmLoginGuard]
            },
            {
                path: 'navigate',
                loadChildren: () => import('../modules/nav/nav.module').then((m) => m.NavModule),
                canActivateChild: [AuthGuard]
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'login'
            },
            {
                path: '**',
                pathMatch: 'full',
                redirectTo: 'login'
            }
        ]
    }
];

/**
 * BMC路由权限列表
 * access说明：访问该路由需要具有的权限
 */

export const atlanticSMMRoutePrivilegesConfig = {
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
    '/navigate/system/info/product': {
        access: ['Login']
    },
    '/navigate/system/info/sensor': {
        access: ['Login']
    },
    '/navigate/system/info/net': {
        access: ['Login']
    },
    '/navigate/system/power': {
        access: ['Login']
    },
    '/navigate/system/fans': {
        access: ['Login']
    },

    // 维护诊断
    '/navigate/maintance/alarm': {
        access: ['Login']
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
    '/navigate/user/online-user': {
        access: ['ConfigureUsers']
    },
    '/navigate/user/security': {
        access: ['Login']
    },

    // 服务管理
    '/navigate/service/port': {
        access: ['Login']
    },
    '/navigate/service/web': {
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
    '/navigate/manager/ibma': {
        access: ['Login']
    }
};
