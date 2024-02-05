import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { FdmComponent } from './fdm.component';

const routes: Routes = [
    { path: '', component: FdmComponent }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class FdmRoutingModule { }
