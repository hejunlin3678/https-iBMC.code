import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { ManagerComponent } from './manager.component';
import { LicenseGuard, IbmaGuard, SpGuard, USBGuard } from 'src/app/common-app/guard';

const routes: Routes = [
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
                path: 'language',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.LanguageModule)
            },
            {
                path: 'upgrade',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.UpgradeModule)
            },
            {
                path: 'switch',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.SwitchModule)
            },
            {
                path: 'configupgrade',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.ConfigUpgradeModule)
            },
            {
                path: 'license',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.LicenseModule),
                canActivate: [LicenseGuard]
            },
            {
                path: 'ibma',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.IbmaModule),
                canActivate: [IbmaGuard]
            },
            {
                path: 'sp',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.SpModule),
                canActivate: [SpGuard]
            },
            {
                path: 'usb',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.UsbModule),
                canActivate: [USBGuard]
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'network'
            }
        ]
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class ManagerRoutingModule { }
