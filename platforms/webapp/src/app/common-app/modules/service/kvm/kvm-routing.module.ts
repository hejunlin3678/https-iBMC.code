import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { KvmComponent } from './kvm.component';

const routes: Routes = [
    {
        path: '',
        component: KvmComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class KvmRoutingModule { }
