import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { CertUpdateComponent } from './cert-update.component';
import { CertUpdateSslComponent } from './components/cert-update-ssl/cert-update-ssl.component';
import { CertUpdateBiosComponent } from './components/cert-update-bios/cert-update-bios.component';

const routes: Routes = [
    {
        path: '',
        component: CertUpdateComponent,
        children: [
            {
                path: 'cert-update-ssl',
                component: CertUpdateSslComponent
            },
            {
                path: 'cert-update-bios',
                component: CertUpdateBiosComponent
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'cert-update-ssl'
            }
        ]
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class CertUpdateRoutingModule { }
