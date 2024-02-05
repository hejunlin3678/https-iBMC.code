import { Routes } from '@angular/router';
import { ServiceComponent } from '../service.component';

export const atlanticSMMRoutes: Routes = [{
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
        loadChildren: () => import('src/app/common-app/modules/service').then((m) => m.KvmModule)
    }, {
        path: 'vmm',
        loadChildren: () => import('src/app/common-app/modules/service').then((m) => m.VmmModule)
    }, {
        path: 'ipmi',
        loadChildren: () => import('src/app/common-app/modules/service').then((m) => m.IpmiModule)
    }, {
        path: '',
        pathMatch: 'full',
        redirectTo: 'port'
    }]
}];
