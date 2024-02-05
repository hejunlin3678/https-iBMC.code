import { Routes } from '@angular/router';
import { PangeaAppComponent } from '../pangea-app.component';
import { AppGuard } from 'src/app/common-app/guard/app-guard.service';
import { PangeaGuard } from '../guard/pangea-guard.service';
import { KvmLoginGuard, AuthGuard } from 'src/app/common-app/guard';

export const atlanticRoutes: Routes = [
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
                path: 'videoPlayer/:id',
                loadChildren: () => import('src/app/common-app/modules/virtualControl/video-player/video-player.module').then(
                    (m) => m.VideoPlayerModule)
            },
            {
                path: 'kvm_h5',
                loadChildren: () => import('src/app/common-app/modules/virtualControl/kvm-h5/kvm-h5.module').then((m) => m.KvmH5Module)
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

export const atlanticRoutePrivilegesConfig = {
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
    '/navigate/service/kvm': {
        access: ['Login']
    },
    '/navigate/service/vmm': {
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
    '/navigate/manager/ibma': {
        access: ['Login']
    }
};
