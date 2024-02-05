import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { SystemComponent } from './system.component';
import { PartityGuard } from 'src/app/common-app/guard/partity-guard.service';
import { FansGuard } from 'src/app/common-app/guard/fans-guard.service';

export const routes: Routes = [
    {
        path: '',
        component: SystemComponent,
        children: [
            {
                path: 'monitor',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.MonitorModule)
            },
            {
                path: 'info',
                loadChildren: () => import('./info/info.module').then((m) => m.InfoModule)
            },
            {
                path: 'bios',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.BiosModule)
            },
            {
                path: 'storage',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.StorageModule)
            },
            {
                path: 'board',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.BoardModule)
            },
            {
                path: 'battery',
                loadChildren: () => import('./battery/battery.module').then((m) => m.BatteryModule)
            },
            {
                path: 'asset',
                loadChildren: () => import('./asset/asset.module').then((m) => m.AssetModule)
            },
            {
                path: 'power',
                loadChildren: () => import('./power/power.module').then((m) => m.PowerModule)
            },
            {
                path: 'fans',
                loadChildren: () => import('./fans/fans.module').then((m) => m.FansModule),
                canActivate: [FansGuard]
            },
            {
                path: 'partity',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.HardPartityModule),
                canActivate: [PartityGuard]
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'info'
            }
        ]
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class SystemRoutingModule { }
