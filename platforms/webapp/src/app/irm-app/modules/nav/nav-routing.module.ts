import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { NavComponent } from './nav.component';

const routes: Routes = [
    {
        path: '',
        component: NavComponent,
        children: [
            {
                path: 'home',
                loadChildren: () => import('./home/home.module').then(m => m.HomeModule)
            },
            {
                path: 'maintance',
                loadChildren: () => import('./maintance/maintance.module').then(m => m.MaintanceModule)
            },
            {
                path: 'service',
                loadChildren: () => import('./service/service.module').then(m => m.ServiceModule)
            },
            {
                path: 'user',
                loadChildren: () => import('./user/user.module').then(m => m.UserModule)
            },
            {
                path: 'manager',
                loadChildren: () => import('./manager/manager.module').then(m => m.ManagerModule)
            },
            {
                path: 'system',
                loadChildren: () => import('./system/system.module').then(m => m.SystemModule)
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'home'
            }
        ]
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class NavRoutingModule { }
