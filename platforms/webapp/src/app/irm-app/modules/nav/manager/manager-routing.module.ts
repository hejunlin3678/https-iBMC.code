import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { ManagerComponent } from './manager.component';

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
                path: 'configupgrade',
                loadChildren: () => import('src/app/common-app/modules/manager').then((m) => m.ConfigUpgradeModule)
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
