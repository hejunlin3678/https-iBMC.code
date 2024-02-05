import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { HardPartityComponent } from './hard-partity.component';

const routes: Routes = [
    {
        path: '',
        component: HardPartityComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class HardPartityRoutingModule { }
