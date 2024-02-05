import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { FansComponent } from './fans.component';
import { AuthGuard } from 'src/app/common-app/guard';


const routes: Routes = [
    {
        path: '',
        component: FansComponent,
        canActivateChild: [AuthGuard]
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class FansRoutingModule { }
