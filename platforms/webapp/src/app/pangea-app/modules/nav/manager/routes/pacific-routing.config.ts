import { Routes } from '@angular/router';
import { IbmaGuard } from 'src/app/common-app/guard';
import { ManagerComponent } from '../manager.component';

export const pacificRoutes: Routes = [
    {
        path: '',
        component: ManagerComponent,
        children: [
            {
                path: 'network',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.NetworkModule)
            },
            {
                path: 'ntp',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.NTPModule)
            },
            {
                path: 'upgrade',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.UpgradeModule)
            },
            {
                path: 'configupgrade',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.ConfigUpgradeModule)
            },
            {
                path: 'ibma',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.IbmaModule),
                canActivate: [IbmaGuard]
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'network'
            }
        ]
    }
];
