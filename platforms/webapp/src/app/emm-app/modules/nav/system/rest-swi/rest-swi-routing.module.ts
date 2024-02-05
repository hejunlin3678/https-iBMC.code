import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { RestSwiComponent } from './rest-swi.component';

const routes: Routes = [
    {
        path: '',
        component: RestSwiComponent,
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class RestSwiRoutingModule { }
