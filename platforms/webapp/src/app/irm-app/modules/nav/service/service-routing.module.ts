import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { ServiceComponent } from './service.component';

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
        path: 'snmp',
        loadChildren: () => import('src/app/common-app/modules/service').then((m) => m.SnmpModule)
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
