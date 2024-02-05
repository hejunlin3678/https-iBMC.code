import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { MainBackSwitchComponent } from './main-back-switch.component';

const routes: Routes = [
    {
        path: '',
        component: MainBackSwitchComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class SwitchRoutingModule { }
