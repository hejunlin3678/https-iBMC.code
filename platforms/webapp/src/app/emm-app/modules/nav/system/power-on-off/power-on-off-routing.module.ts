import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { PowerOnOffComponent } from './power-on-off.component';

const routes: Routes = [
    {
        path: '',
        component: PowerOnOffComponent,
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class PowerOnOffRoutingModule { }
