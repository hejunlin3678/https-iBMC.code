import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { SystemlogComponent } from './system-log.component';

const routes: Routes = [
    {
        path: '',
        component: SystemlogComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class SystemLogRoutingModule { }
