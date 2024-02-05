import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { LdapComponent } from './ldap.component';

const routes: Routes = [
    {
        path: '',
        component: LdapComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class LdapRoutingModule { }
