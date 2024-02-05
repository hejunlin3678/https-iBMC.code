import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { LocaluserComponent } from './localuser.component';

const routes: Routes = [
    {
        path: '',
        component: LocaluserComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class LocaluserRoutingModule { }
