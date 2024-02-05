import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { VmmComponent } from './vmm.component';

const routes: Routes = [
    {
        path: '',
        component: VmmComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class VmmRoutingModule { }
