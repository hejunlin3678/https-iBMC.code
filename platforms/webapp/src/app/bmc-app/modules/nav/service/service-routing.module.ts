import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { ServiceComponent } from './service.component';
import { KVMGuard, VMMGuard, VNCGuard } from 'src/app/common-app/guard';

const routes: Routes = [{
    path: '',
    component: ServiceComponent,
    children: [{
        path: 'port',
        loadChildren: () => import('src/app/common-app/modules/service').then((m) => m.PortModule)
    }, {
        path: 'web',
        loadChildren: () => import('src/app/common-app/modules/service').then((m) => m.WebModule)
    }, {
        path: 'kvm',
        loadChildren: () => import('src/app/common-app/modules/service').then((m) => m.KvmModule),
        canActivate: [KVMGuard]
    }, {
        path: 'vmm',
        loadChildren: () => import('src/app/common-app/modules/service').then((m) => m.VmmModule),
        canActivate: [VMMGuard]
    }, {
        path: 'vnc',
        loadChildren: () => import('src/app/common-app/modules/service').then((m) => m.VncModule),
        canActivate: [VNCGuard]
    }, {
        path: 'snmp',
        loadChildren: () => import('src/app/common-app/modules/service').then((m) => m.SnmpModule),
    }, {
        path: 'ipmi',
        loadChildren: () => import('src/app/common-app/modules/service').then((m) => m.IpmiModule)
    }, {
        path: '',
        pathMatch: 'full',
        redirectTo: 'port'
    }]
}
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class ServiceRoutingModule { }
