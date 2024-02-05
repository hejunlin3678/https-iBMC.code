import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { OnlineUserComponent } from './online-user.component';

const routes: Routes = [
    {
        path: '',
        component: OnlineUserComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class OnlineUserRoutingModule { }
