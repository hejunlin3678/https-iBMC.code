import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { PowerComponent } from './power.component';
import { AuthGuard } from 'src/app/common-app/guard';

const routes: Routes = [
    {
        path: '',
        component: PowerComponent,
        canActivateChild: [AuthGuard]
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class PowerRoutingModule { }
