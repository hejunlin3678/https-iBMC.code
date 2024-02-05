import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { SnmpComponent } from './snmp.component';

const routes: Routes = [
    {
        path: '',
        component: SnmpComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class SnmpRoutingModule { }
