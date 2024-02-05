import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { IpmiComponent } from './ipmi.component';

const routes: Routes = [
    {
        path: '',
        component: IpmiComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class IpmiRoutingModule { }
