import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { TwoFactorComponent } from './two-factor.component';
import { RootCertificateComponent } from './component/root-certificate/root-certificate.component';
import { ClientCertificateComponent } from './component/client-certificate/client-certificate.component';

const routes: Routes = [
    {
        path: '',
        component: TwoFactorComponent,
        children: [{
            path: 'root',
            component: RootCertificateComponent
        }, {
            path: 'client',
            component: ClientCertificateComponent
        }, {
            path: '',
            pathMatch: 'full',
            redirectTo: 'root'
        }]
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class TwoFactorRoutingModule { }
