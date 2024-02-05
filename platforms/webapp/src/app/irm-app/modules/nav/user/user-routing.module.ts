import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { UserComponent } from './user.component';

const routes: Routes = [
    {
        path: '',
        component: UserComponent,
        children: [
            {
                path: 'localuser',
                loadChildren: () => import('src/app/common-app/modules/user').then((m) => m.LocaluserModule)
            },
            {
                path: 'ldap',
                loadChildren: () => import('src/app/common-app/modules/user').then((m) => m.LdapModule)
            },
            {
                path: 'kerberos',
                loadChildren: () => import('src/app/common-app/modules/user').then((m) => m.KerberosModule)
            },
            {
                path: 'online-user',
                loadChildren: () => import('src/app/common-app/modules/user').then((m) => m.OnlineUserModule)
            },
            {
                path: 'security',
                loadChildren: () => import('src/app/common-app/modules/user').then((m) => m.SecurityModule)
            },
            {
                path: 'cert-update',
				loadChildren: () => import('src/app/common-app/modules/user').then((m) => m.CertUpdateModule)
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'localuser'
            }
        ]
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class UserRoutingModule { }
