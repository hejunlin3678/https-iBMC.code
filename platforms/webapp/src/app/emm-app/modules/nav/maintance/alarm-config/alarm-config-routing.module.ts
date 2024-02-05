import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { AlarmConfigComponent } from './alarm-config.component';

const routes: Routes = [
    {
        path: '',
        component: AlarmConfigComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class AlarmConfigRoutingModule { }
