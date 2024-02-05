import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { AlarmEventComponent } from './alarm-event.component';

const routes: Routes = [
    {
        path: '',
        component: AlarmEventComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class AlarmEventRoutingModule { }
