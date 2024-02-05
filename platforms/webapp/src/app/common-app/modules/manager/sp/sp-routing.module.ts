import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { SpComponent } from './sp.component';

const routes: Routes = [
    {
        path: '',
        component: SpComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class SpRoutingModule { }
