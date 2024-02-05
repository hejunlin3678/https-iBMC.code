import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { VncComponent } from './vnc.component';

const routes: Routes = [
    {
        path: '',
        component: VncComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class VncRoutingModule { }
