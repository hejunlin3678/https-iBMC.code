import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { ManagerComponent } from './manager.component';

const routes: Routes = [
    {
        path: '',
        component: ManagerComponent,
        children: [
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
                loadChildren: () => import('./upgrade/upgrade.module').then((m) => m.UpgradeModule)
            },
            {
                path: 'switch',
                loadChildren: () => import('./main-back-switch/switch.module').then((m) => m.SwitchModule)
            },
            {
                path: 'network',
                loadChildren: () => import('./network/network.module').then((m) => m.NetworkModule)
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
